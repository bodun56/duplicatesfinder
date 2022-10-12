QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    duplicatesfinder.cpp \
    fileinfo.cpp \
    formchangelog.cpp \
    main.cpp \
    mainwindow.cpp \
    settingsfinder.cpp \
    sqldatabase.cpp

HEADERS += \
    duplicatesfinder.h \
    fileinfo.h \
    formchangelog.h \
    mainwindow.h \
    settingsfinder.h \
    sqldatabase.h

FORMS += \
    formchangelog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TRANSLATIONS += Lang_ru.ts
TRANSLATIONS += Lang_en.ts

CODECFORSRC     = UTF-8

RESOURCES += \
    res.qrc


VERSION_PE_HEADER = 1.6.1

win32:VERSION = 1.6.1 # major.minor.patch.build
else:VERSION = 1.6.1    # major.minor.patch

DEFINES += APP_VERSION='"\\\"1.6.1\\\""'

DISTFILES += \
    DuplicatesFinder.rc

win32:RC_FILE = DuplicatesFinder.rc
