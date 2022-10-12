#ifndef SETTINGSFINDER_H
#define SETTINGSFINDER_H

#include <QString>
#include <QColor>


class SettingsFinder
{
public:
    SettingsFinder();
    SettingsFinder(QColor aColor, QColor bColor, QColor removeColor);

    void setColorA(QColor color);
    QColor getColorA() const;

    void setColorB(QColor color);
    QColor getColorB() const;

    void setColorRemove(QColor color);
    QColor getColorRemove();

    void setAversion(QString v);
    QString getAversion();

    void setLang(QString v);
    QString getLang();

private:
    QColor colorA;
    QColor colorB;
    QColor colorRemove;
    QString aVersion;
    QString lang;
};

#endif // SETTINGSFINDER_H
