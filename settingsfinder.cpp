#include "settingsfinder.h"

SettingsFinder::SettingsFinder()
{

}

void SettingsFinder::setColorA(QColor color)
{
    colorA = color;
}

QColor SettingsFinder::getColorA() const
{
    return colorA;
}

void SettingsFinder::setColorB(QColor color)
{
    colorB = color;
}

QColor SettingsFinder::getColorB() const
{
    return colorB;
}

void SettingsFinder::setColorRemove(QColor color)
{
    colorRemove = color;
}

QColor SettingsFinder::getColorRemove()
{
    return colorRemove;
}

void SettingsFinder::setAversion(QString v)
{
    aVersion = v;
}

QString SettingsFinder::getAversion()
{
    return aVersion;
}

void SettingsFinder::setLang(QString v)
{
    lang = v;
}

QString SettingsFinder::getLang()
{
    return lang;
}
