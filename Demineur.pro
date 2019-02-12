#-------------------------------------------------
#
# Project created by QtCreator 2012-05-17T11:54:51
#
#-------------------------------------------------

QT       += core gui \
            widgets
TARGET = Demineur
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    casedem.cpp \
    horloge.cpp \
    fin.cpp \
    solver.cpp

HEADERS  += mainwindow.h \
    casedem.h \
    horloge.h \
    fin.h \
    solver.h

FORMS    += \
    fin.ui

RESOURCES += \
    ressource.qrc
