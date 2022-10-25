#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "duplicatesfinder.h"
#include "fileinfo.h"
#include "settingsfinder.h"
#include "formchangelog.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QThread>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QTableWidgetItem>
#include <QProgressBar>
#include <QLabel>
#include <QMessageBox>
#include <QColorDialog>
#include <QSettings>
#include <QToolTip>
#include <QHeaderView>
#include <QTimer>
#include <QTime>
#include <QDateTime>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QDesktopWidget>
#include <QTranslator>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pbOpenPath_clicked();

    void on_pbStartFind_clicked();

    void onDuplicatesFinished();

    void on_tableWidget_itemClicked(QTableWidgetItem *item);

    void on_actionSet_color_1_triggered();

    void on_actionSet_color_2_triggered();

    void on_actionSet_dels_color_triggered();

    void on_actionSave_duplicate_list_triggered();

    void on_lineEdit_textChanged(const QString &arg1);
    void on_MenuAction();

    void on_comboBox_currentIndexChanged(int index);

    void on_actionDefaults_color_triggered();

    void timeUpdate();
    void onPreloadGetCount(int count);
    void onPreloadGetFileName(QString fn);

    void on_actionAbout_Qt_triggered();

    void on_actionAbout_program_triggered();

    //buttons
    void on_pbDeleteFiles_clicked();
    void on_pbRenameFiles_clicked();
    void on_pbMoveFiles_clicked();

    void on_tableWidget_cellChanged(int row, int column);

    void on_actionChangelog_triggered();

    void on_actionRu_triggered();

    void on_actionEn_triggered();

    void on_cbOnlyFiles_stateChanged(int arg1);

    void on_actionIt_triggered();

private:
    Ui::MainWindow *ui;

    QString findPath;
    DuplicatesFinder *df;
    QThread *thread;
    void onGetDuplicates(QStringList list);
    void onGetCounterAllFiles(int countAllFiles);
    int preCountFiles = 0;

    void onGetAllFiles(int value);
    void onGetCurrentFile(QString file);
    int columnWidth[6];
    bool b_state = false;
    bool colorBool = false;
    bool userStopped = false;

    QString lastHash;
    QStringList fromDel;

    //status
    QProgressBar progressBar;
    QLabel lblStatus;
    QLabel lblCurrentFile;
    QLabel lblTimer;

    //settings
    SettingsFinder settingsFinder;
    void settingsLoad();
    void settingsSave();
    void settingsUpdate();
    void setDefaultColor();
    void updateColorTableList();
    //QString homePath;

#ifdef Q_OS_LINUX
    QString homePath = QDir::homePath() + "/.config/";
#else
    QString homePath = QDir::homePath() + "/";
#endif

    QStringList listOfMask;
    QStringList listImages;
    QStringList listVideo;
    QStringList listAudio;
    void maskBoxInit();

    //time
    QTimer timer;
    QDateTime dateTimeStart;
    const QString format = "HH:mm:ss";

    //table actions
    void onClipboard();
    void onContextMenu();
    QString lastClickedItem;
    QClipboard *clipboard;

    QStringList headers;
    QString newName(QString name, int index);

    QTranslator translator;
    void translateUpdate(QString lng);

};
#endif // MAINWINDOW_H
