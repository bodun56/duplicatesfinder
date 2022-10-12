#include "formchangelog.h"
#include "ui_formchangelog.h"

FormChangeLog::FormChangeLog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FormChangeLog)
{
    ui->setupUi(this);

    QFile f(":/changelog/log");
    f.open(QIODevice::ReadOnly);
    ui->plainTextEdit->setPlainText(f.readAll());
}

FormChangeLog::~FormChangeLog()
{
    delete ui;
}
