#ifndef CASEDEM_H
#define CASEDEM_H

#include <QPushButton>
#include <iostream>
#include <QObject>
#include "mainwindow.h"
#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPixmap>
 #include <QMouseEvent>
#include "fin.h"

class MainWindow;

class MineCell : public QPushButton
{
    Q_OBJECT
public:
    MineCell(int i, int j,MainWindow *w);
    MineCell();


    MainWindow *w;
    //QPushButton *butdem;

    void mouseReleaseEvent ( QMouseEvent * event );
    void mousePressEvent( QMouseEvent * event );
    void clickCell();
    void displayNeighbors();
    void setBomb(bool bomb){this->bomb=bomb;}
    bool isBomb(){return bomb;}
    void setFlag(bool flag){this->flag=flag;}
    bool isFlag(){return flag;}
    void setClicked(bool clicked){this->clicked = clicked;}
    bool isClicked(){return clicked;}
    unsigned int x(){return xCoordinate;}
    unsigned int y(){return yCoordinate;}
    void setX(int x){xCoordinate=x;}
    void setY(int y){yCoordinate=y;}
    void reset();
private:
    bool bomb;
    bool flag;
    bool clicked;
    unsigned int yCoordinate;
    unsigned int xCoordinate;
};

#endif // CASEDEM_H
