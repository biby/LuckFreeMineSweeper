#include "horloge.h"


Clock::Clock(QObject *parent) :
    QTimer(parent)
{
    time = new QLabel(tr("Temps : 0 seconde"));
    firstClick = false;
}

void Clock::beat(){
    start(1000);
}
