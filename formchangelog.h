#ifndef FORMCHANGELOG_H
#define FORMCHANGELOG_H

#include <QMainWindow>

namespace Ui {
class FormChangeLog;
}

class FormChangeLog : public QMainWindow
{
    Q_OBJECT

public:
    explicit FormChangeLog(QWidget *parent = nullptr);
    ~FormChangeLog();

private:
    Ui::FormChangeLog *ui;
};

#endif // FORMCHANGELOG_H
