QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

SOURCES += \
    fileview.cpp \
    ftpclient.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    fileview.h \
    ftpclient.h \
    mainwindow.h

FORMS += \
    fileview.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
