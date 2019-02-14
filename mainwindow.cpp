#include "mainwindow.h"
#include <QtGui>
#include <QGridLayout>
#include <algorithm>

MainWindow::MainWindow()
{
    nbOfLuckyGuesses = 0;
    nbOfForcedLuckyGuesses=0;
    width = 15;
    height = 30;
    nbBombs = 99;
    nbflag = 0;
    tmps = 0;
    clock = new Clock;
    clock->gameEnded=false;
    QGridLayout *mainLayout = new QGridLayout(this);
    QGridLayout *cases = new QGridLayout;
    QWidget *menu = new QWidget(this);
    menu->setStyleSheet("background-color: dark");
    cases->setSpacing(0);
    cases->setMargin(2);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    QHBoxLayout *menulayout = new QHBoxLayout;
    reset = new QPushButton();
    reset->setFixedSize(120,40);
    reset->setIcon(QIcon("://face-smile.svg"));
    reset->setIconSize(QSize(32,32));
    reset->setText(tr("Nouveau"));
    char a[20];
    sprintf(a,"Bombes : %d/%d",nbflag,nbBombs);
    bombs = new QLabel(tr(a));
    menulayout->addWidget(bombs,2,Qt::AlignLeft);
    menulayout->addWidget(reset);
    menulayout->addWidget(clock->time,2,Qt::AlignRight);
    mainLayout->addWidget(menu,0,0);
    menu->setLayout(menulayout);
    mainLayout->addLayout(cases,1,0);
    connect(reset, SIGNAL(clicked()),this, SLOT(gameReset()));
    cellArray = new MineCell**[width];
    unsigned int i,j;
    for(i=0;i<width;i++){
        cellArray[i] = new MineCell*[height];
        for(j=0;j<height;j++){
            cellArray[i][j] = new MineCell(i,j,this);
            cases->addWidget(cellArray[i][j]/*->butdem*/, i+1, j, 1,1);
        }
    }
    setLayout(mainLayout);

    nbOfUnraveledCells = 0;
    connect(this,SIGNAL(startTimer()),clock,SLOT(beat()));
    connect(clock,SIGNAL(timeout()),this,SLOT(timeCount()));
    setWindowIcon(QIcon("gnomine.png"));
}

MainWindow::~MainWindow()
{
    disconnect(this,SIGNAL(startTimer()),clock,SLOT(beat()));
    disconnect(clock,SIGNAL(timeout()),this,SLOT(timeCount()));
    unsigned int i,j;
    for(i=0;i<width;i++){
        for(j=0;j<height;j++){
            delete cellArray[i][j];
        }
        delete cellArray[i];
    }
    delete cellArray;
    delete reset;
    delete bombs;
    delete clock;
}

void coutmap(int **mat, unsigned int inte, unsigned int ext)
{
    for(unsigned int i =0;i<inte;++i)
    {
        for(unsigned int j=0;j<ext+1;++j)
        {
            std::cout << mat[i][j] << " ";
        }
        std::cout << std::endl;
    }
    bool test = true;
    for(unsigned int j=0;j<ext+1;++j)
    {
        bool test2 = false;
        for(unsigned int i =0;i<inte;++i)
        {
            if(mat[i][j]!=0)
            {
                test2=true;
            }
        }
        if(test2==false) test= false;
    }
    if(test == false) std::cout << "Problem" << std::endl;
    std::cout << std::endl;
}

void MainWindow::timeCount()
{
    tmps++;
    char a[30];
    sprintf(a,"Temps : %d secondes",tmps);
    clock->time->setText(tr(a));
    clock->beat();
}

unsigned int MainWindow::celldistance(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) const
{
    unsigned int horizontalDistance = x1<x2?x2-x1:x1-x2;
    unsigned int verticalDistance = y1<y2?y2-y1:y1-y2;
    return horizontalDistance<verticalDistance?verticalDistance:horizontalDistance;
}

void MainWindow::fillArray(unsigned int xfirstCellClicked, unsigned int yfirstCellClicked) // Fills up the board with bombs, place the bombs at distance at least 2 from the first clicked cell.
{
    srand(time(NULL));
    unsigned int nbOfCells = width*height;
    if(4*nbBombs > nbOfCells){
        nbBombs = nbOfCells/4;
    }
    unsigned int placedBombs = 0, x, y;
    while(placedBombs<nbBombs){
        x = rand()%width;
        y = rand()%height;
        if((cellArray[x][y]->isBomb() == false) && celldistance(x,y,xfirstCellClicked,yfirstCellClicked)>1){
            cellArray[x][y]->setBomb(true);
            placedBombs++;
        }
    }
}

void MainWindow::diplayBombs()
{
    unsigned int x,y;
    for(x=0;x<width;x++){
        for(y=0;y<height;y++){
            if(cellArray[x][y]->isBomb() && !cellArray[x][y]->isFlag()){
                cellArray[x][y]->setIcon(QIcon("://mine.svg"));
                cellArray[x][y]->setIconSize(QSize(28,28));
            }else if(!cellArray[x][y]->isBomb() && cellArray[x][y]->isFlag()){
                cellArray[x][y]->setIcon(QIcon("://face-sad.svg"));
                cellArray[x][y]->setIconSize(QSize(28,28));
            }
        }
    }
}

bool MainWindow::isCorner(unsigned int x, unsigned int y) const
{
    if(x==0 || x==width-1)
    {
        return y==0 || y==height-1;
    }
    return false;
}

bool MainWindow::isSide(unsigned int x, unsigned int y) const
{
    return x==0 || x==width-1 || y==0 || y==height-1;
}

unsigned int MainWindow::numberOfNeighbors(unsigned int x, unsigned int y) const
{
    if(isCorner(x,y))
    {
        return 4;
    }
    else if(isSide(x,y))
    {
        return 6;
    }
    else
    {
        return 9;
    }
}

unsigned int MainWindow::listOfNeighbors(MineCell *** neighborsListPointer, unsigned int x, unsigned int y) const
{
    unsigned int nbOfNeighbors = numberOfNeighbors(x,y);

    unsigned xmin, xmax,ymin,ymax;
    xmin = x>0?x-1:x;
    xmax = x<width-1?x+1:x;
    ymin = y>0?y-1:y;
    ymax = y<height-1?y+1:y;
    *neighborsListPointer = new MineCell*[nbOfNeighbors];
    unsigned int count=0;
    for(unsigned i=xmin;i<=xmax;++i)
    {
        for(unsigned j=ymin;j<=ymax;++j)
        {
            (*neighborsListPointer)[count++]=cellArray[i][j];
        }
    }
    return nbOfNeighbors;
}

int MainWindow::nbOfBombNeighbors(unsigned int x, unsigned int y) const
{
    unsigned int bombNeighborCount = 0;
    MineCell **neighborsList;
    unsigned int numberOfNeighbors = listOfNeighbors(&neighborsList,x,y);
    for(unsigned int i=0;i<numberOfNeighbors;++i)
    {
        if(neighborsList[i]->isBomb())
        {
            ++bombNeighborCount;
        }
    }
    delete neighborsList;
    return bombNeighborCount;
}

unsigned int MainWindow::nbOfFlagNeighbors(unsigned int x,unsigned  int y) const
{
    unsigned int flagNeighborCount = 0;
    MineCell **neighborsList;
    unsigned int numberOfNeighbors = listOfNeighbors(&neighborsList,x,y);
    for(unsigned int i=0;i<numberOfNeighbors;++i)
    {
        if(neighborsList[i]->isFlag())
        {
            ++flagNeighborCount;
        }
    }
    delete neighborsList;
    return flagNeighborCount;
}

void MainWindow::displayNeighbors(unsigned int x, unsigned int y)
{
    MineCell **neighborsList;
    unsigned int numberOfNeighbors = listOfNeighbors(&neighborsList,x,y);
    for(unsigned int i=0;i<numberOfNeighbors;++i)
    {
        neighborsList[i]->clickCell();
    }
    delete neighborsList;
}

unsigned int MainWindow::nbOfClickedNeighbors(unsigned int x, unsigned int y) const
{
    unsigned int nbOfClikedNeighborsCount = 0;
    MineCell **neighborsList;
    unsigned int numberOfNeighbors = listOfNeighbors(&neighborsList,x,y);
    for(unsigned int i=0;i<numberOfNeighbors;++i)
    {
        if(neighborsList[i]->isClicked()){
            nbOfClikedNeighborsCount++;
        }
    }
    delete neighborsList;
    return nbOfClikedNeighborsCount;
}

void MainWindow::gameReset()
{
    unsigned int x,y;
    for(x=0;x<width;x++){
        for(y=0;y<height;y++){
            cellArray[x][y]->reset();
        }
    }
    clock->firstClick = false;
    clock->gameEnded = false;
    clock->time->setText(tr("Temps : 0 seconde"));
    nbflag = 0;
    char a[20];
    sprintf(a,"Bombes : %d/%d",nbflag,nbBombs);
    bombs->setText(tr(a));

    tmps = 0;
    nbOfUnraveledCells = 0;
    nbOfLuckyGuesses = 0;
    nbOfForcedLuckyGuesses=0;
}


unsigned int MainWindow::filterCells(MineCell ***listOfCellsFilteredPointer, bool (MainWindow::*property)(unsigned int,unsigned int) const) const
{
    unsigned int cellCount = 0;
    unsigned arrayLength = countCells(property);
    MineCell **listOfCellsFiltered = new MineCell*[arrayLength];
    *listOfCellsFilteredPointer = listOfCellsFiltered;
    unsigned int x,y;
    for(x=0;x<width;x++){
        for(y=0;y<height;y++){
            if((this->*property)(x,y)){
                listOfCellsFiltered[cellCount]=cellArray[x][y];
                ++cellCount;
            }
        }
    }
    return arrayLength;
}

unsigned int MainWindow::countCells(bool (MainWindow::*property)(unsigned int,unsigned int) const) const
{
    unsigned arrayLength=0;
    unsigned int x,y;
    for(x=0;x<width;x++){
        for(y=0;y<height;y++){
            if((this->*property)(x,y)){
                ++arrayLength;
            }
        }
    }
    return arrayLength;
}

bool MainWindow::isUnclickedAndNeighborOfDisplayedCell(unsigned int i,unsigned int j) const
{
    if(!(cellArray[i][j]->isClicked()) && nbOfFlagNeighbors(i,j)!=nbOfClickedNeighbors(i,j))
    {
        return true;
    }
    return false;
}

bool MainWindow::isDisplayedAndHasNonFlagNonDisplayedNeighbor(unsigned int i,unsigned int j) const
{
    if((cellArray[i][j]->isClicked()
        && !cellArray[i][j]->isFlag())
            &&   nbOfClickedNeighbors(i,j)!= numberOfNeighbors(i,j) )
    {
        return true;
    }
    return false;
}

bool MainWindow::isEmptyCell(unsigned int x, unsigned int y) const
{
    if(!(cellArray[x][y]->isClicked() ||cellArray[x][y]->isFlag()))
    {
        if(nbOfFlagNeighbors(x,y)==nbOfClickedNeighbors(x,y))
        {
            return true;
        }
    }
    return false;
}

bool MainWindow::isEmptyBomb(unsigned int x, unsigned int y) const
{
    return cellArray[x][y]->isBomb() &&  isEmptyCell(x,y);
}

bool MainWindow::isEmptyFreeCell(unsigned int x, unsigned int y) const
{
    return !cellArray[x][y]->isBomb() &&  isEmptyCell(x,y);
}

int MainWindow::nbEmptyCells(){//Empty cell means that they are not next to a cell that is not clicked has not neighbors that have been clicked
    return countCells(&MainWindow::isEmptyCell);
}

int MainWindow::numberOfUnclickedAndNeighborOfDisplayedCell(){
    return countCells(&MainWindow::isUnclickedAndNeighborOfDisplayedCell);
}

int MainWindow::numberOfCellsDisplayedAndHasNonFlagNonDisplayedNeighbor(){
    return countCells(&MainWindow::isDisplayedAndHasNonFlagNonDisplayedNeighbor);
}
