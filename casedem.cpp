#include "casedem.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <solver.h>
using namespace std;

MineCell::MineCell(int i,int j, MainWindow *w)
{
    xCoordinate = i;
    yCoordinate = j;
    this->w = w;
    setBomb(false);
    setFlag(false);
    //butdem = new QPushButton();
    //butdem->
    setFixedSize(30,30);
    setClicked(false);
}
MineCell::MineCell()
{
}

void MineCell::reset()
{
    setText(tr(""));
    setClicked(false);
    setBomb(false);
    setFlag(false);
    setDown(false);
    setStyleSheet("");
    setIcon(QIcon(""));
}

void MineCell::clickCell(){
    if(isClicked()){
        setDown(true);
        return;
    }

    if(isBomb()){
        setClicked(true);
        w->diplayBombs();
        w->clock->gameEnded = true;
        w->clock->stop();
        setIcon(QIcon("://bang.svg"));
        setIconSize(QSize(28,28));
        w->reset->setIcon(QIcon("://face-sad.svg"));
        w->reset->setIconSize(QSize(32,32));
    }else{
        w->reset->setIcon(QIcon("://face-cool.svg"));
        w->reset->setIconSize(QSize(32,32));
        setClicked(true);
        int n = w->nbOfBombNeighbors(x(),y());
        w->nbOfUnraveledCells++;
        if(n==0){
            setDown(true);
            w->displayNeighbors(x(),y());
        }else{
            switch(n)
            {
            case 1: {
                setStyleSheet("font-size: 17pt;font-weight: bold; color: blue;");
                setText(tr("1"));
                break;
            }
            case 2: {
                setStyleSheet("font-size: 17pt;font-weight: bold; color: green;");
                setText(tr("2"));
                break;
            }
            case 3: {
                setStyleSheet("font-size: 17pt;font-weight: bold; color: red;");
                setText(tr("3"));
                break;
            }
            case 4: {
                setStyleSheet("font-size: 17pt;font-weight: bold; color: darkblue;");
                setText(tr("4"));
                break;
            }
            case 5: {
                setStyleSheet("font-size: 17pt;font-weight: bold; color: darkred;");
                setText(tr("5"));
                break;
            }
            case 6: {
                setStyleSheet("font-size: 17pt;font-weight: bold; color: cyan;");
                setText(tr("6"));
                break;
            }
            case 7: {
                setStyleSheet("font-size: 17pt;font-weight: bold; color: darkmagenta;");
                setText(tr("7"));
                break;
            }
            case 8: {
                setStyleSheet("font-size: 17pt;font-weight: bold; color: black;");
                setText(tr("8"));
            }
            }

        }
    }

    setDown(true);
}

void MineCell::mousePressEvent( QMouseEvent * event){
    if(!w->clock->gameEnded && w->clock->firstClick && event->button()!=Qt::RightButton && !(!isClicked() && event->button()==Qt::MidButton)){
        w->reset->setIcon(QIcon("://face-worried.svg"));
        w->reset->setIconSize(QSize(32,32));
    }
    unsigned int i,j;
    for(i=0;i<w->width;i++){
        for(j=0;j<w->height;j++){
            if(isClicked() && w->celldistance(i,j,x(),y())<=1 && !w->cellArray[i][j]->flag && (event->button()!=Qt::RightButton)){
                w->cellArray[i][j]->setDown(true);
            }else if((i==x()) && j==y() && event->button()==Qt::LeftButton){
                w->cellArray[i][j]->setDown(true);
            }else if(w->cellArray[i][j]->isClicked() && !w->cellArray[i][j]->flag){
                w->cellArray[i][j]->setDown(true);
            }else{
                w->cellArray[i][j]->setDown(false);
            }
        }
    }
}

void MineCell::mouseReleaseEvent( QMouseEvent * event ){
    if(!w->clock->gameEnded){

    if(!(w->clock->firstClick)){
        w->fillArray(x(),y());
        w->clock->firstClick = true;
        w->startGame();
    }
    char a[20];
    if(event->button() == Qt::RightButton){
        if(flag){
            setIcon(QIcon(""));
            flag = false;
            setClicked(false);
            w->nbflag--;
            sprintf(a,"Bombes : %d/%d",w->nbflag,w->nbBombs);
            w->bombs->setText(tr(a));
        }else if(!isClicked()){
            //setStyleSheet("background-color: red;");
            setIcon(QIcon("://flag.svg"));
            setIconSize(QSize(28,28));
            flag = true;
            setClicked(true);
            w->nbflag++;
            sprintf(a,"Bombes : %d/99",w->nbflag);
            w->bombs->setText(tr(a));
        }
    }else if(event->button() == Qt::LeftButton){
        if(w->isUnclickedAndNeighborOfDisplayedCell(x(),y())){
            Solver solve(w,x(),y());
            solve.luckRemover();
        }
        if(!isClicked()){
            clickCell();
        }else{
            displayNeighbors();
        }
    }else if(isClicked() && event->button() == Qt::MidButton){
        displayNeighbors();
    }
    }
    unsigned int i,j;
    for(i=0;i<w->width;i++){
        for(j=0;j<w->height;j++){
            if(w->cellArray[i][j]->isClicked() && !w->cellArray[i][j]->flag){
                w->cellArray[i][j]->setDown(true);
            }else{
                w->cellArray[i][j]->setDown(false);
            }
        }
    }

    if(w->height*w->width == w->nbBombs+w->nbOfUnraveledCells && !w->clock->gameEnded){


        w->clock->gameEnded = true;
        w->reset->setIcon(QIcon("://face-win.svg"));
        w->reset->setIconSize(QSize(32,32));
        w->clock->stop();
        unsigned int nbscores;
        ifstream fichier("scores.txt", ios::in);
        int tp[nbscores+1];
        fichier >> nbscores;
        bool flag = true;
        for(unsigned int i=0;i<nbscores+1;i++){
            if(i==nbscores && flag){
                tp[i] = w->tmps;
            }else{
                fichier >> tp[i];
                if(w->tmps < tp[i] && flag){
                    tp[i+1]=tp[i];
                    tp[i]=w->tmps;
                    i++;
                    flag = false;
                }
            }
        }
        nbscores = (nbscores <10?nbscores+1:10);
        ofstream fichier2("scores.txt", ios::out);
        fichier2 << nbscores<<endl;
        for(unsigned i=0;i<nbscores;i++){
            fichier2 << tp[i]<<endl;
        }
        fichier.close();
        fichier2.close();
        EndGame *f = new EndGame(w,tp[0]);
        f->show();
    }
}

void MineCell::displayNeighbors(){
    w->reset->setIcon(QIcon("://face-cool.svg"));
    w->reset->setIconSize(QSize(32,32));
    if(flag || !isClicked()){
        return;
    }
    int n = w->nbOfFlagNeighbors(x(),y());
    int m = w->nbOfBombNeighbors(x(),y());
    if(n==m){
        w->displayNeighbors(x(),y());
    }
}

