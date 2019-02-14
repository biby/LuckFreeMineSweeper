#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "casedem.h"
#include <time.h>
#include "horloge.h"
#include <fstream>

class Clock;
class MineCell;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    void diplayBombs();
    void displayNeighbors(unsigned int x, unsigned int y);
    int nbOfBombNeighbors(unsigned int x, unsigned int y) const;
    unsigned int nbOfFlagNeighbors(unsigned int x, unsigned int y) const;
    unsigned int nbOfClickedNeighbors(unsigned int i, unsigned int j) const;
    int nbflag;
    bool isUnclickedAndNeighborOfDisplayedCell(unsigned int i, unsigned int j) const;
    bool isDisplayedAndHasNonFlagNonDisplayedNeighbor(unsigned int i, unsigned int j) const;
    bool isEmptyCell(unsigned int x, unsigned int y) const;
    bool isEmptyBomb(unsigned int x, unsigned int y) const;
    bool isEmptyFreeCell(unsigned int x, unsigned int y) const;
    QLabel *bombs;
    Clock *clock;
    int nbOfUnraveledCells;
    void fillArray(unsigned int xfirstCellClicked, unsigned int yfirstCellClicked);
    void startGame(){
        emit startTimer();
    }
    unsigned int width;
    unsigned int height;
    unsigned int nbBombs;
    int nbEmptyCells();
    int numberOfUnclickedAndNeighborOfDisplayedCell();
    int numberOfCellsDisplayedAndHasNonFlagNonDisplayedNeighbor();
    MineCell ***cellArray;
    int nbOfLuckyGuesses;
    int nbOfForcedLuckyGuesses;
    QPushButton *reset;
    int tmps;
    unsigned int celldistance(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) const;
    unsigned int filterCells(MineCell ***listOfCellsFilteredPointer,bool (MainWindow::*property)(unsigned int, unsigned int) const) const;
    unsigned int countCells(bool (MainWindow::*property)(unsigned int, unsigned int) const) const;

private slots:
   void gameReset();
   void timeCount();
private:

    bool isSide(unsigned x,unsigned y) const;
    bool isCorner(unsigned x,unsigned y) const;
    unsigned int listOfNeighbors(MineCell ***neighborsListPointer, unsigned  int x, unsigned int y) const;
    unsigned int numberOfNeighbors(unsigned int x, unsigned int y) const;



signals:
        void startTimer();

};

#endif
