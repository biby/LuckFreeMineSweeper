#ifndef HORLOGE_H
#define HORLOGE_H

#include <iostream>
#include <QThread>
#include <QLabel>
#include <time.h>
#include <QMutex>
#include <QTimer>
#include "mainwindow.h"

class MainWindow;

class Clock : public QTimer
{
    Q_OBJECT
public:
    explicit Clock(QObject *parent = 0);
    MainWindow *w;
    time_t T;
    QLabel *time;
    bool firstClick;
    bool gameEnded;
signals:

protected:
public slots:
    void beat();
};

#endif // HORLOGE_H
