#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "casedem.h"
#include <time.h>
#include "horloge.h"
#include <fstream>
#include <QObject>

class MineCell;
class MainWindow;


class Solver
{
public:
    Solver(MainWindow *w, unsigned int x, unsigned int y);
    ~Solver();
    void luckRemover();

private:
    MainWindow *w;
    unsigned int clickedButtonX;
    unsigned int clickedButtonY;
    int **mat;
    MineCell **bordint;
    unsigned int nbOfClickedCellsOnTheBoundary;

    MineCell **bordext;
    unsigned int nbOfUnclickedCellsOnTheBoundary;
    unsigned int clickedButtonIndex;
    unsigned int rang;
    unsigned int compmec;
    unsigned int ** compconnexes;
    unsigned int * nombremeccomp;
    unsigned int * rankOfConnectedComponent;
    unsigned int * connectedcomponentsrow;
    unsigned int nbcomp;
    bool *bombespossible;
    unsigned int *safesolution;
    bool foundsafesolution;
    unsigned int indexmec;
    unsigned int nbbombstofree;
    MineCell **listOfBombsToFree;
    unsigned  nbbombstoset;
    MineCell **listOfBombsToSet;


    void findClickedButtonIndex();
    void initializeMatrix();
    void gauss();
    void simplify();
    void fillConnectedComponents();
    void possiblebombtester(unsigned int comptotest);
    void coutmap();
    void findsafesolution();
};

#endif // SOLVER_H
