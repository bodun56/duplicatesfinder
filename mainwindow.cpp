#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCoreApplication::setApplicationVersion(QString(APP_VERSION));
    QIcon icon(":/icon/logo");
    this->setWindowIcon(icon);

    ui->label->setText("");

    headers = QStringList() << "ID" << tr("File path") << tr("File name") << tr("Size") << tr("Date") << tr("Hash") << tr("Delete");
    ui->tableWidget->setColumnCount(headers.count());
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->hideColumn(0);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &MainWindow::onContextMenu);

    ui->pbStartFind->setEnabled(false);
    ui->pbDeleteFiles->setEnabled(false);
    ui->pbMoveFiles->setEnabled(false);
    ui->actionSave_duplicate_list->setEnabled(false);

    progressBar.setMinimum(0);
    progressBar.setFixedWidth(300);
    lblStatus.setFixedWidth(250);
    lblCurrentFile.setMinimumWidth(300);
    ui->statusbar->addWidget(&lblStatus);
    ui->statusbar->addWidget(&progressBar);
    ui->statusbar->addWidget(&lblTimer);
    ui->statusbar->addWidget(&lblCurrentFile);

    ui->pbOpenPath->setToolTip(tr("Select path to search"));
    ui->pbStartFind->setToolTip(tr("Search duplicates"));
    ui->pbDeleteFiles->setToolTip(tr("Delete selected files"));
    ui->lineEdit->setToolTip(tr("Mask files, separate space key"));
    ui->checkBoxExclude->setToolTip(tr("Exclude file in mask"));
    ui->comboBox->setToolTip(tr("Type files"));

    ui->lineEdit->setPlaceholderText(tr("Example: .jpg .png"));

    settingsFinder.setColorA(QColor(161, 216, 247));
    settingsFinder.setColorB(QColor(255,255,255));
    settingsFinder.setColorRemove(QColor(229, 181, 181));

    maskBoxInit();

    timer.setInterval(250);
    connect(&timer, &QTimer::timeout, this, &MainWindow::timeUpdate);
    clipboard = QApplication::clipboard();

    ui->pbRenameFiles->setVisible(false);
    headers = QStringList() << "ID" << tr("File path") << tr("File name") << tr("Size") << tr("Date") << tr("Hash") << tr("Delete");
    settingsLoad();
    this->setWindowTitle("Duplicates Finder " + QCoreApplication::applicationVersion());
}

MainWindow::~MainWindow()
{
    settingsSave();
    delete ui;
}

void MainWindow::on_pbOpenPath_clicked()
{
    if(findPath.isEmpty()){
        findPath = QFileDialog::getExistingDirectory(0, tr("Search to"), QDir::homePath()); //Выберите где искать
    }else{
        findPath = QFileDialog::getExistingDirectory(0, tr("Search to"), findPath);
    }
    if(!findPath.isEmpty()){
        ui->label->setText(tr("Search in: ") + findPath);
        ui->pbStartFind->setEnabled(true);
    }

}

void MainWindow::on_pbStartFind_clicked()
{
    if(ui->cbOnlyFiles->isChecked() && ui->lineEdit->text().isEmpty()){
        QToolTip::showText(mapToGlobal(ui->lineEdit->pos()), tr("Need mask files"));
        return;
    }

    if(b_state == false){
        dateTimeStart = QDateTime::currentDateTime();
        lblTimer.setText(QDateTime::fromTime_t(dateTimeStart.secsTo(QDateTime::currentDateTime())).toUTC().toString(format));
        timer.start();
        lblStatus.setText(tr("Start"));
        lblCurrentFile.clear();
        progressBar.setValue(0);
        colorBool = false;

        for(int i = 0;i<6;i++){
            columnWidth[i] = 0;
        }
        QStringList listMask;
        if(!ui->lineEdit->text().isEmpty()) listMask = ui->lineEdit->text().split(" ");

        fromDel.clear();
        ui->tableWidget->clear();

        ui->tableWidget->setRowCount(0);
        ui->tableWidget->setHorizontalHeaderLabels(headers);

        thread = new QThread();
        df = new DuplicatesFinder();
        df->moveToThread(thread);
        df->setPathFind(findPath);
        df->setMaskFiles(listMask);
        df->setExclude(ui->checkBoxExclude->isChecked());
        df->setOnOnlyFiles(ui->cbOnlyFiles->isChecked());
        df->setRunning(true);
        connect(thread, &QThread::started, df, &DuplicatesFinder::checkingFilesStart);
        connect(df, &DuplicatesFinder::finished, this, &MainWindow::onDuplicatesFinished);
        connect(df, &DuplicatesFinder::onDuplicateFined, this, &MainWindow::onGetDuplicates);
        connect(df, &DuplicatesFinder::onCountAllFilesChanged, this, &MainWindow::onGetCounterAllFiles);
        connect(df, &DuplicatesFinder::onAllFiles, this, &MainWindow::onGetAllFiles);
        connect(df, &DuplicatesFinder::onCurrentFile, this, &MainWindow::onGetCurrentFile);
        connect(df, &DuplicatesFinder::onPreLoad, this, &MainWindow::onPreloadGetCount);
        connect(df, &DuplicatesFinder::onPrealodFile, this, &MainWindow::onPreloadGetFileName);
        thread->start();

        ui->pbStartFind->setText(tr("Stop"));
        ui->pbOpenPath->setEnabled(false);
        b_state = !b_state;
        userStopped = false;
    }else{
        timer.stop();
        thread->exit();
        if(df != NULL){
            if(df->getRunning()){
                df->setRunning(false);
            }
        }
        if(ui->cbOnlyFiles->isChecked()) ui->pbStartFind->setText(tr("Search files"));
        else ui->pbStartFind->setText(tr("Search duplicates"));
        ui->pbOpenPath->setEnabled(true);
        lblStatus.setText(tr("Stopped"));
        userStopped = true;
    }
}

void MainWindow::onDuplicatesFinished()
{
    if(fromDel.count() > 0){
        ui->pbDeleteFiles->setEnabled(true);
        ui->pbMoveFiles->setEnabled(true);
    }else{
        ui->pbDeleteFiles->setEnabled(false);
        ui->pbMoveFiles->setEnabled(false);
    }
    if(ui->cbOnlyFiles->isChecked()) ui->pbStartFind->setText(tr("Search files"));
    else ui->pbStartFind->setText(tr("Search duplicates"));
    ui->pbOpenPath->setEnabled(true);
    b_state = !b_state;

    if(userStopped){
        if(ui->tableWidget->rowCount() == 0) lblCurrentFile.setText(tr("Stopped"));
        else{
            if(ui->cbOnlyFiles->isChecked()) lblCurrentFile.setText(tr("Stopped. Fined files: ") + QString::number(ui->tableWidget->rowCount()));
            else lblCurrentFile.setText(tr("Stopped. Fined duplicates: ") + QString::number(ui->tableWidget->rowCount()));
        }
    }else{
        if(ui->tableWidget->rowCount() == 0) {
            if(ui->cbOnlyFiles->isChecked()) lblCurrentFile.setText(tr("Files not fined")); //Дубликаты не обнаружены
            else lblCurrentFile.setText(tr("Duplicates not fined"));
        }
        else {
            if(ui->cbOnlyFiles->isChecked()) lblCurrentFile.setText(tr("Finished. Fined files: ") + QString::number(ui->tableWidget->rowCount()));
            else lblCurrentFile.setText(tr("Finished. Fined duplicates: ") + QString::number(ui->tableWidget->rowCount()));
        }
    }

    on_MenuAction();

    timer.stop();
    thread->exit();
}

void MainWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    lastClickedItem = item->text();
}

void MainWindow::onGetDuplicates(QStringList list)
{
    QColor color;

    if(lastHash != list.at(5)){
        colorBool = !colorBool;
        lastHash = list.at(5);
    }

    if(colorBool){
            color = settingsFinder.getColorA();
        }else{
            color = settingsFinder.getColorB();
        }

    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    for(int i=0;i<list.count();i++){
        QTableWidgetItem *item = new QTableWidgetItem(list.at(i));
        item->setData(Qt::BackgroundRole, color);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);

        QLabel ll(item->text());
        ll.setScaledContents(false);
        ui->tableWidget->setItem(row,i, item);
        if(columnWidth[i] < fontMetrics().boundingRect(item->text()).size().width()){
            columnWidth[i] = fontMetrics().boundingRect(item->text()).size().width() + 50;
            ui->tableWidget->setColumnWidth(i,columnWidth[i]);
        }
    }

    QTableWidgetItem *item = new QTableWidgetItem();
    item->data(Qt::CheckStateRole);
    item->setCheckState(Qt::Unchecked);
    item->setData(Qt::BackgroundColorRole, color);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    ui->tableWidget->setItem(row,list.count(), item);

    if(ui->cbOnlyFiles->isChecked()) lblCurrentFile.setText(tr("Fined files: ") + QString::number(ui->tableWidget->rowCount()));
    else lblCurrentFile.setText(tr("Fined duplicates: ") + QString::number(ui->tableWidget->rowCount()));
}

void MainWindow::onGetCounterAllFiles(int countAllFiles)
{
    progressBar.setValue(countAllFiles);
    lblStatus.setText(tr("Processed files: ") + QString::number(countAllFiles) + tr(" from ") + QString::number(preCountFiles));
}

void MainWindow::onGetAllFiles(int value)
{
    progressBar.setMaximum(value);
}

void MainWindow::onGetCurrentFile(QString file)
{
    lblCurrentFile.setText(file);
}

void MainWindow::settingsLoad()
{
    QSettings settings(homePath + "DuplicatesFinder.conf", QSettings::IniFormat);
    settings.beginGroup("Settings");
    settingsFinder.setColorA(settings.value("ColorA", QColor(161, 216, 247)).toString());
    settingsFinder.setColorB(settings.value("ColorB", QColor(255,255,255)).toString());
    settingsFinder.setColorRemove(settings.value("ColorRemove", QColor(229, 181, 181)).toString());
    settingsFinder.setLang(settings.value("Lang", "ru").toString());
    QString _s_version = settings.value("version", QCoreApplication::applicationVersion()).toString();
    settings.endGroup();

    translateUpdate(settingsFinder.getLang());

    QString _c_version = QCoreApplication::applicationVersion();
    if(QString::compare(_s_version, _c_version, Qt::CaseInsensitive) != 0){
        QTimer *ts = new QTimer;
        ts->setSingleShot(true);
        connect(ts, &QTimer::timeout, this, &MainWindow::on_actionChangelog_triggered);
        ts->start(250);
    }
}

void MainWindow::settingsSave()
{
    QSettings settings(homePath + "DuplicatesFinder.conf", QSettings::IniFormat );
    settings.beginGroup("Settings");
    settings.setValue("ColorA", settingsFinder.getColorA().name());
    settings.setValue("ColorB", settingsFinder.getColorB().name());
    settings.setValue("ColorRemove", settingsFinder.getColorRemove().name());
    settings.setValue("version", QCoreApplication::applicationVersion());
    settings.setValue("Lang", settingsFinder.getLang());
    settings.endGroup();
}

void MainWindow::settingsUpdate()
{
}

void MainWindow::setDefaultColor()
{
    settingsFinder.setColorA(QColor(161, 216, 247));
    settingsFinder.setColorB(QColor(255,255,255));
    settingsFinder.setColorRemove(QColor(229, 181, 181));

    settingsSave();
}

void MainWindow::updateColorTableList()
{
    QColor color;
    QString _hash;

    colorBool = false;

    for(int row=0; row < ui->tableWidget->rowCount(); row++){
        if(_hash != ui->tableWidget->item(row,5)->text()){
            colorBool = !colorBool;
            _hash = ui->tableWidget->item(row,5)->text();
        }

        if(colorBool){
            color = settingsFinder.getColorA();
        }else{
            color = settingsFinder.getColorB();
        }

        for(int column=1 ; column < 7; column++){
            ui->tableWidget->item(row,column)->setData(Qt::BackgroundColorRole, color);
        }
    }
}

void MainWindow::maskBoxInit()
{
    listImages.append("*.jpg");
    listImages.append("*.png");

    listVideo.append("*.avi");
    listVideo.append("*.mp4");

    listAudio.append("*.mp3");
    listAudio.append("*.wav");
}

void MainWindow::onClipboard()
{
    clipboard->setText(lastClickedItem);
}

void MainWindow::onContextMenu()
{
    QMenu menu;
    auto cmenu = menu.addAction(tr("Copy"));
    connect(cmenu, &QAction::triggered, this, &MainWindow::onClipboard);
    menu.exec(QCursor::pos());
}

QString MainWindow::newName(QString name, int index)
{
    QStringList listName = name.split(".");
    QStringList nName;
    nName.append(listName.at(0) + "-(" + QString::number(index) + ")");
    for(int i=1;i<listName.count();i++) nName.append(listName.at(i));
    return nName.join(".");
}

//edit color 1
void MainWindow::on_actionSet_color_1_triggered()
{
    QColorDialog colorDialog;
    colorDialog.setCurrentColor(settingsFinder.getColorA());
    if(colorDialog.exec() == QColorDialog::Accepted){
        settingsFinder.setColorA(colorDialog.currentColor());
        updateColorTableList();
    }
}

//edit color 2
void MainWindow::on_actionSet_color_2_triggered()
{
    QColorDialog colorDialog;
    colorDialog.setCurrentColor(settingsFinder.getColorB());
    if(colorDialog.exec() == QColorDialog::Accepted){
        settingsFinder.setColorB(colorDialog.currentColor());
        updateColorTableList();
    }
}

//edit remove color
void MainWindow::on_actionSet_dels_color_triggered()
{
    QColorDialog colorDialog;
    colorDialog.setCurrentColor(settingsFinder.getColorRemove());
    if(colorDialog.exec() == QColorDialog::Accepted){
        settingsFinder.setColorRemove(colorDialog.currentColor());
        updateColorTableList();
    }
}

//save csv
void MainWindow::on_actionSave_duplicate_list_triggered()
{
    QString filter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("Save File"),
                                QDir::homePath(),
                                tr("Text csv (*.csv);;Text file (*.txt)"), &filter);

    if(fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream.setLocale(QLocale::system());
        if(filter == tr("Text csv (*.csv)")) stream << tr("Path ;\t") << tr("File name ;\t ") << tr("Size ;\t ") << tr("Date ;\t ") << tr("Hash") << "\r\n";
        for(int i = 0;i<ui->tableWidget->rowCount();i++){
            for(int x = 1; x < ui->tableWidget->columnCount() - 1;x++){
                if(filter == tr("Text csv (*.csv)")) stream << ui->tableWidget->item(i,x)->text() << " ;\t ";
                else stream << ui->tableWidget->item(i,x)->text() << " \t ";
            }
            stream << "\r\n";
        }

        stream.flush();
        file.flush();
        file.close();
    }
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    QStringList listMask;
    listMask.append("!");
    listMask.append("@");
    listMask.append("#");
    listMask.append("$");
    listMask.append("%");
    listMask.append("^");
    listMask.append("&");
    listMask.append(":");
    listMask.append(";");
    listMask.append("\\");
    listMask.append("/");
    listMask.append("\"");
    listMask.append("\'");

    for(int i = 0; i<listMask.count();i++){
        if(arg1.indexOf(listMask.at(i)) >= 0){
            ui->lineEdit->setText(arg1.chopped(1));
            QToolTip::showText(mapToGlobal(ui->lineEdit->pos()), tr("Do not use this symbol: ") + listMask.at(i));
        }
    }
}

void MainWindow::on_MenuAction()
{
    if(ui->tableWidget->rowCount() == 0){
        ui->actionSave_duplicate_list->setEnabled(false);
    }else{
        ui->actionSave_duplicate_list->setEnabled(true);
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if(index == 0){
        ui->lineEdit->clear();
        listOfMask.clear();
    }

    switch (index) {
        case 1:
            listOfMask = listImages;
            break;
        case 2:
            listOfMask = listVideo;
            break;
        case 3:
            listOfMask = listAudio;
            break;
    }

    QString str;
    for(int i = 0; i < listOfMask.count(); i++){
        str += listOfMask.at(i) + " ";
    }

    ui->lineEdit->setText(str);
}

void MainWindow::on_actionDefaults_color_triggered()
{
    setDefaultColor();
    updateColorTableList();
}

void MainWindow::timeUpdate()
{
    QCoreApplication::setApplicationVersion(QString(APP_VERSION));
    this->setWindowTitle("Duplicates Finder " + QCoreApplication::applicationVersion());
    lblTimer.setText(QDateTime::fromTime_t(dateTimeStart.secsTo(QDateTime::currentDateTime())).toUTC().toString(format));
}

void MainWindow::onPreloadGetCount(int count)
{
    preCountFiles = count;
    lblStatus.setText(tr("Preparation: ") + QString::number(count));
}

void MainWindow::onPreloadGetFileName(QString fn)
{
    lblCurrentFile.setText(fn);
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionAbout_program_triggered()
{
    QMessageBox msg;
    msg.setWindowTitle(tr("About"));
    msg.setText(tr("Duplicate Finder") + "\r\n" + tr("Program version: ") + QCoreApplication::applicationVersion() + "\r\n\r\n" +
                tr("A simple duplicate file search program for Linux and Windows") + "\r\n\r\n" +
                tr("Author: BoDun < bodun56@ya.ru >"));
    QIcon icon(":/icon/logo");
    msg.setWindowIcon(icon);

    msg.exec();
}

void MainWindow::on_pbDeleteFiles_clicked()
{
    if(fromDel.count() == 0) return;

    QMessageBox msg;
    msg.setIcon(QMessageBox::Information);
    msg.setWindowTitle(tr("Attention"));
    msg.setText(tr("Files to delete are marked: ") + QString::number(fromDel.count()) + "\r\n" + tr("Delete them?"));
    msg.setStandardButtons(QMessageBox::Yes);
    msg.addButton(QMessageBox::No);
    msg.setDefaultButton(QMessageBox::No);
    if(msg.exec() == QMessageBox::No) return;

    progressBar.setValue(0);
    progressBar.setMaximum(ui->tableWidget->rowCount());
    int remCount = 1;

    for(int i=0; i < ui->tableWidget->rowCount();i++){
        QTableWidgetItem *item = ui->tableWidget->item(i,6);
        if(item->checkState() == Qt::Checked && item->background() != settingsFinder.getColorRemove() && fromDel.count() > 0){
            ui->tableWidget->item(i,1)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i,2)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i,3)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i,4)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i,5)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i,6)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i, 6)->setCheckState(Qt::Unchecked);

            QFile file(ui->tableWidget->item(i,1)->text() + "/" + ui->tableWidget->item(i,2)->text());
            if(file.exists()) file.remove();

            ui->tableWidget->item(i,2)->setText(tr("(deleted) ") + ui->tableWidget->item(i,2)->text());
            ui->tableWidget->update();

            lblCurrentFile.setText(tr("Deleted files: ") + QString::number(remCount));
            remCount++;
        }
        lblStatus.setText(tr("Processed files: ") + QString::number(i + 1));
        progressBar.setValue(i + 1);
    }

    fromDel.clear();
    ui->pbDeleteFiles->setEnabled(false);
    ui->pbMoveFiles->setEnabled(false);
}

void MainWindow::on_pbRenameFiles_clicked()
{
}

void MainWindow::on_pbMoveFiles_clicked()
{
    if(fromDel.count() == 0) return;
    QString moveTo = QFileDialog::getExistingDirectory(this, tr("Where to transfer"), QDir::currentPath());
    if(moveTo.isEmpty()) return;

    QMessageBox msg;
    msg.setIcon(QMessageBox::Information);
    msg.setWindowTitle(tr("Attention"));
    msg.setText(tr("Files to move are marked: ") + QString::number(fromDel.count()) + "\r\n" + tr("Moving them to ") + moveTo + "?");
    msg.setStandardButtons(QMessageBox::Yes);
    msg.addButton(QMessageBox::No);
    msg.setDefaultButton(QMessageBox::No);
    if(msg.exec() == QMessageBox::No) return;

    progressBar.setValue(0);
    progressBar.setMaximum(ui->tableWidget->rowCount());
    int remCount = 1;

    for(int i=0; i < ui->tableWidget->rowCount();i++){
        QTableWidgetItem *item = ui->tableWidget->item(i,6);
        if(item->checkState() == Qt::Checked && item->background() != settingsFinder.getColorRemove() && fromDel.count() > 0){
            ui->tableWidget->item(i,1)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i,2)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i,3)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i,4)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i,5)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i,6)->setBackground(settingsFinder.getColorRemove());
            ui->tableWidget->item(i, 6)->setCheckState(Qt::Unchecked);

            QFile file(ui->tableWidget->item(i,1)->text() + "/" + ui->tableWidget->item(i,2)->text());
            QString fileName = ui->tableWidget->item(i, 2)->text();
            QStringList fn = fileName.split(".");

            if(file.exists()){
                int count = 1;
                if(QFile(moveTo + "/" + fileName).exists()){
                    while(QFile(moveTo + "/" + newName(fileName, count)).exists()){
                        count++;
                    }
                    file.rename(moveTo + "/" + newName(fileName, count));
                }else{
                    file.rename(moveTo + "/" + fileName);
                }

            }

            ui->tableWidget->item(i,2)->setText(tr("(Moved) ") + ui->tableWidget->item(i,2)->text());
            ui->tableWidget->update();

            lblCurrentFile.setText(tr("Moved files: ") + QString::number(remCount));
            remCount++;
        }
        lblStatus.setText(tr("Processed files: ") + QString::number(i + 1));
        progressBar.setValue(i + 1);
    }

    fromDel.clear();
    ui->pbDeleteFiles->setEnabled(false);
    ui->pbMoveFiles->setEnabled(false);
}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{
    if(column == 6){
        if(ui->tableWidget->item(row, column)->background() == settingsFinder.getColorRemove()){
            ui->tableWidget->item(row, column)->setCheckState(Qt::Unchecked);
            return;
        }
        if(ui->tableWidget->item(row, column)->checkState() == Qt::Checked){
            fromDel.append(ui->tableWidget->item(row, 0)->text());
        }
        if(ui->tableWidget->item(row, column)->checkState() == Qt::Unchecked){
            for(int i=0;i<fromDel.count();i++){
                if(fromDel.at(i) == ui->tableWidget->item(row, 0)->text()){
                    fromDel.removeAt(i);
                }
            }
        }

        if(fromDel.count() > 0){
            ui->pbDeleteFiles->setEnabled(true);
            ui->pbMoveFiles->setEnabled(true);
        }else{
            ui->pbDeleteFiles->setEnabled(false);
            ui->pbMoveFiles->setEnabled(false);
        }
    }
}

void MainWindow::on_actionChangelog_triggered()
{
    FormChangeLog *fcl = new FormChangeLog(this);
    fcl->move(this->pos().x(), this->pos().y());
    fcl->show();
}

void MainWindow::translateUpdate(QString lng)
{
    settingsFinder.setLang(lng);
    translator.load(":/lang/" + settingsFinder.getLang());
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);

    headers = QStringList() << "ID" << tr("File path") << tr("File name") << tr("Size") << tr("Date") << tr("Hash") << tr("Delete");
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->update();

    ui->comboBox->clear();
    ui->comboBox->addItem(tr("All files"));
    ui->comboBox->addItem(tr("Images"));
    ui->comboBox->addItem(tr("Video"));
    ui->comboBox->addItem(tr("Audio"));
}

void MainWindow::on_actionRu_triggered()
{
    translateUpdate("ru");
}

void MainWindow::on_actionEn_triggered()
{
    translateUpdate("en");
}

void MainWindow::on_actionIt_triggered()
{
    translateUpdate("it");
}


void MainWindow::on_cbOnlyFiles_stateChanged(int arg1)
{
    ui->checkBoxExclude->setEnabled(!arg1);
    ui->checkBoxExclude->setCheckState(Qt::Unchecked);
    if(arg1) ui->pbStartFind->setText(tr("Search files"));
    else ui->pbStartFind->setText(tr("Search duplicates"));
}


