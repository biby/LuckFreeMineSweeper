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
    bombs = new QLabel(tr("Bombes : 0/99"));
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
    if(5*nbBombs > nbOfCells){
        nbBombs = nbOfCells/5;
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
    bombs->setText(tr("Bombes : 0/99"));
    nbflag = 0;
    tmps = 0;
    nbOfUnraveledCells = 0;
    nbOfLuckyGuesses = 0;
    nbOfForcedLuckyGuesses=0;
}

void MainWindow::findSolution(int **mat, unsigned int ext, unsigned int rang, unsigned int** compconnexes, unsigned int* nombremeccomp, unsigned int posext, unsigned int compmec, MineCell **bordext)
{
    unsigned int i,mectest,j,k,l,m;
    int som;
    bool test;
    unsigned int nbbombes1 = 0;
    for(i=0;i<nombremeccomp[compmec];i++){ //Counts the number of bombs in the connected component of mectest.
        if(bordext[compconnexes[compmec][i]]->isBomb())
        {//cellArray[bordext[compconnexes[compmec][i]]->x()][bordext[compconnexes[compmec][i]]->y()]->isBomb()){
            nbbombes1++;
        }
    }
    i= compmec; // I set i to compmec and then never change it. I   don"t know why.
    unsigned int nbbombes2;
    mectest = posext; //Same thing. Why do I do that?
    m=0;
    while(compconnexes[i][m]!=mectest) m++;
    for(j=0;j<(((unsigned int) 1)<<(nombremeccomp[i]));j++){ // Generates all the bomb possibilities as a number (length in bits is the number of boxes, 1 if bomb, 0 if not a bomb for all but the place that has been clicked
        //It is really bad to avoid the box of the guy and run things arround it...
        test = true;
        for(k=0;k<nombremeccomp[i];k++){ //tests if the generated solution is  a valid one.
            if(compconnexes[i][k]<rang){
                som = 0;
                for(l=0;l<m;l++){
                    som+=mat[compconnexes[i][k]][compconnexes[i][l]]*((j>>l)%2);
                }
                for(l=m+1;l<nombremeccomp[i];l++){
                    som+=mat[compconnexes[i][k]][compconnexes[i][l]]*((j>>(l-1))%2);
                }
                if(som!=mat[compconnexes[i][k]][ext]){
                    test = false;
                    break;
                }
            }
        }
        if(test){ // if the solution is valid.
            nbbombes2=0;
            for(l=0;l<nombremeccomp[i]-1;l++){ //counts the number of bombs in the solution and store it in nbbombes2
                if(((j>>l)%2)==1){
                    nbbombes2++;
                }
            }
            //Now it is going to change the board to the solution it found. Two options: either remove bombs or add some.
            if(nbEmptyCells()>=abs(nbbombes2-nbbombes1)){ // I don't think this test makes sense with the abs.
                int x = 0,y=0;
                if(nbbombes2-nbbombes1>0){
                    while(nbbombes2-nbbombes1>0){ //add bombs. Just read the board linearly and add bombs where it can
                        y++;
                        if(y%height ==0){ //Scans over the board, ugly way...
                            y = 0;
                            x++;
                            x %=width;
                        }
                        if(!(cellArray[x][y]->isClicked())||cellArray[x][y]->isFlag()){
                            if(nbOfFlagNeighbors(x,y)==nbOfClickedNeighbors(x,y) && !cellArray[x][y]->isBomb()){
                                cellArray[x][y]->setBomb(true);
                                nbbombes2--;
                            }
                            nbbombes1++;
                        }
                    }
                }else if(nbbombes1-nbbombes2>0){ //remove bombs. Just read the board linearly and remove bombs where it can
                    while(nbbombes1-nbbombes2>0){
                        y++;
                        if(y%height ==0){
                            y = 0;
                            x++;
                            x %=width;
                        }
                        if(!(cellArray[x][y]->isClicked())||cellArray[x][y]->isFlag()){
                            if(nbOfFlagNeighbors(x,y)==nbOfClickedNeighbors(x,y) && cellArray[x][y]->isBomb()){
                                cellArray[x][y]->setBomb(false);
                                nbbombes2++;
                            }
                            nbbombes1--;
                        }
                    }
                }
                //For the boxes in the connected components, set the board to the new solution
                for(l=0;l<m;l++){
                    if(((j>>l)%2)==1){
                        bordext[compconnexes[i][l]]->setBomb(true);
                    }else{
                        bordext[compconnexes[i][l]]->setBomb(false);
                    }
                }
                bordext[compconnexes[i][l]]->setBomb(false);
                for(l=m+1;l<nombremeccomp[i];l++){
                    if(((j>>(l-1))%2)==1){
                        bordext[compconnexes[i][l]]->setBomb(true);
                    }else{
                        bordext[compconnexes[i][l]]->setBomb(false);
                    }
                }
                return;
            }
        }
    }
}

int MainWindow::emptyCell(int **mat, unsigned int nbOfBoxesInMat, unsigned int matRank,unsigned int** connectedComponents, unsigned int* sizeOfConnectedComponents,unsigned int nbconnectedcomponents, int* bombespossible){
    //count the number of cells that for which the setting can be swap
    unsigned int i,j,m,l,k,mectest;
    int som;
    bool test;
    for(i=0;i<nbconnectedcomponents;i++){
        for(m=0;m<sizeOfConnectedComponents[i];m++){
            mectest = connectedComponents[i][m];
            if(sizeOfConnectedComponents[i]==1 && mectest>=matRank){
                return mectest;
            }
            if(bombespossible[mectest]!=1){
                continue;
            }
            if(bombespossible[mectest]==1){
                for(j=0;j<(((unsigned int) 1)<<(sizeOfConnectedComponents[i]));j++){ //Again test all the possibilities!
                    test = true;
                    for(k=0;k<sizeOfConnectedComponents[i];k++){
                        if(connectedComponents[i][k]<matRank){
                            som = 0;
                            for(l=0;l<m;l++){
                                som+=mat[connectedComponents[i][k]][connectedComponents[i][l]]*((j>>l)%2);
                            }
                            if(mat[connectedComponents[i][k]][mectest]!=0){
                                som+=mat[connectedComponents[i][k]][mectest];
                            }
                            for(l=m+1;l<sizeOfConnectedComponents[i];l++){
                                som+=mat[connectedComponents[i][k]][connectedComponents[i][l]]*((j>>(l-1))%2);
                            }
                            if(som!=mat[connectedComponents[i][k]][nbOfBoxesInMat]){
                                test = false;
                                break;
                            }
                        }
                    }
                    if(test){
                        bombespossible[mectest]=0;
                        for(l=0;l<m;l++){
                            if(((j>>l)%2)==1){
                                bombespossible[connectedComponents[i][l]]=0;
                            }
                        }
                        for(l=m+1;l<sizeOfConnectedComponents[i];l++){
                            if(((j>>(l-1))%2)==1){
                                bombespossible[connectedComponents[i][l]]=0;
                            }
                        }
                        break;
                    }
                }
                if(!test){
                    return mectest;
                }
            }
            for(k=0;k<sizeOfConnectedComponents[i];k++){
                if(bombespossible[connectedComponents[i][k]]==2){
                    return connectedComponents[i][k];
                }
            }
        }
    }
    return -1;
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

void MainWindow::simplify(int **mat, unsigned int nbCol, unsigned int nbRow) const
{
    for(unsigned int i=0;i<nbRow;i++)//We remove cells that are trivially bombs
    {
        int mini = 0;
        int maxi = 0;
        for(unsigned int j=0;j<nbCol;j++)
        {
            if(mat[i][j]>0)
            {
                maxi+=mat[i][j];
            }
            else if(mat[i][j]<0)
            {
                mini+=mat[i][j];
            }
        }
        if(mini==mat[i][nbCol])
        {
            for(unsigned int l=0;l<nbCol;++l)
            {
                if(mat[i][l]>0)
                {
                    for(unsigned int k=0; k<nbRow;++k)
                    {
                        if(k==i) continue;
                        mat[k][l]=0;
                    }

                }
                else if(mat[i][l]<0)
                {
                    for(unsigned int k=0; k<nbRow;++k)
                    {
                        if(k==i) continue;
                        mat[k][nbCol]-=mat[k][l];
                        mat[k][l]=0;
                    }
                }
            }
        }
        else if(maxi==mat[i][nbCol])
        {
            for(unsigned int l=0;l<nbCol;++l)
            {
                if(mat[i][l]<0)
                {
                    for(unsigned int k=0; k<nbRow;++k)
                    {
                        if(k==i) continue;
                        mat[k][l]=0;
                    }

                }
                else if(mat[i][l]>0)
                {
                    for(unsigned int k=0; k<nbRow;++k)
                    {
                        if(k==i) continue;
                        mat[k][nbCol]-=mat[k][l];
                        mat[k][l]=0;
                    }
                }
            }
        }
    }
}

unsigned int MainWindow::gauss(int **mat, unsigned int nbCol, unsigned int nbRow, MineCell **bordext, unsigned int &col) const // The matrix has nbRow rows and nbCol+1 col, but we apply gauss only on the nbCol columns. Col is a column we keep track of during swaps in the process.
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k,l;
    int a;
    for(i=0;i<nbRow;i++)//We remove cells that are trivially bombs
    {
        int numberOfAdjacentCells = 0;
        for(j=0;j<nbCol;j++)
        {
            if(mat[i][j]!=0)
            {
                numberOfAdjacentCells++;
            }
        }
        if(numberOfAdjacentCells == mat[i][nbCol])
        {
            for(j=0;j<nbCol;j++)
            {
                if(mat[i][j]!=0)
                {
                    for(k=0;k<nbRow;k++)
                    {
                        if(k==i) continue;
                        if(mat[k][j]!=0)
                        {
                            mat[k][nbCol]-=mat[k][j];
                            mat[k][j]=0;
                        }
                    }
                }
            }
        }
    }

    i=0;
    j=0;
    while(i<nbRow && j<nbCol){
        if(mat[i][j]==0){
            k=i+1;
            while(k<nbRow && mat[k][j]==0){
                k++;
            }
            if(k==nbRow){
                j++;
                continue;
            }
            std::swap(mat[i],mat[k]);
        }
        for(k=i+1;k<nbRow;k++)
        {
            if(mat[k][j] != 0){
                a = mat[k][j];
                for(l = 0; l< nbCol+1;l++){
                    mat[k][l] = mat[i][j]*mat[k][l]-a*mat[i][l];
                }
            }
        }
        i++;
        j++;
    }
    simplify(mat,nbCol,nbRow);
    unsigned int rang = i;
    j = 0;
    while(j<rang){
        if(mat[j][j]==0){
            k=j+1;
            while(mat[j][k]==0)
            {
                ++k;
            }
            if(k>=nbCol)
            {
                ++j;
                continue;
            }
            for(l = 0; l< nbRow;l++){
                std::swap(mat[l][j],mat[l][k]);
            }
            std::swap(bordext[j],bordext[k]);
            if(col==j){
                col = k;
            }else if(col == k){
                col = j;
            }
        }
        j++;
    }
    j--;

    while(j>0)
    {
        if(mat[j][j]!=0)
        {
            for(k = 0;k<=j-1;++k)
            {
                if(mat[k][j]!=0){
                    a = mat[k][j];
                    for(l = 0; l< nbCol+1;l++){
                        mat[k][l] = mat[j][j]*mat[k][l]-a*mat[j][l];
                    }
                }
            }
        }
        j--;
    }
    return rang;
}

unsigned int MainWindow::fillConnectedComponents(int **mat, unsigned int rang, unsigned int nbOfUnclickedCellsOnTheBoundary, unsigned int ** compconnexes, unsigned int * nombremeccomp, unsigned int &compmec, unsigned int clickedButtonIndex) const
{

    unsigned int nbcomp = 0;
    unsigned int nbmec;
    unsigned int i,j,k,l;
    bool mis[nbOfUnclickedCellsOnTheBoundary];
    for(k = 0;k<nbOfUnclickedCellsOnTheBoundary;k++){
        mis[k]=false;
    }
    j=0;
    while(j<nbOfUnclickedCellsOnTheBoundary){
        compconnexes[nbcomp] = new unsigned int[nbOfUnclickedCellsOnTheBoundary];
        for(k = 0;k<nbOfUnclickedCellsOnTheBoundary;k++){
            if(!mis[k]){
                break;
            }
        }
        if(k == clickedButtonIndex){
            compmec = nbcomp;
        }
        mis[k] = true;
        compconnexes[nbcomp][0]=k;
        j++;
        k=0;
        nbmec = 1;
        while(k<nbmec){
            l=compconnexes[nbcomp][k];
            for(i=0;i<rang;i++){
                if(mat[i][l]!=0)
                {
                    for(unsigned int iter=0;iter<nbOfUnclickedCellsOnTheBoundary;iter++)
                    {
                        if(mat[i][iter]!=0 && !mis[iter])
                        {
                            compconnexes[nbcomp][nbmec++] = iter;
                            mis[iter] = true;
                            if(iter == clickedButtonIndex)
                            {
                                compmec = nbcomp;
                            }
                            j++;

                        }
                    }
                }
            }
            k++;
        }
        nombremeccomp[nbcomp]=nbmec;
        nbcomp++;
    }
    return nbcomp;
}

void MainWindow::luckRemover(unsigned int larg, unsigned int haut)
{
    MineCell **bordint;
    unsigned int nbOfClickedCellsOnTheBoundary = filterCells(&bordint, &MainWindow::isDisplayedAndHasNonFlagNonDisplayedNeighbor);

    MineCell **bordext;
    unsigned int nbOfUnclickedCellsOnTheBoundary = filterCells(&bordext, &MainWindow::isUnclickedAndNeighborOfDisplayedCell);

    unsigned int clickedButtonIndex=0;
    for(unsigned int i=0;i<nbOfUnclickedCellsOnTheBoundary;++i)
    {
        if(bordext[i]->x() == larg && bordext[i]->y() == haut){
            clickedButtonIndex = i;
            break;
        }
    }
    unsigned int x,y;
    int **mat = new int*[nbOfClickedCellsOnTheBoundary];
    for(unsigned int i=0;i<nbOfClickedCellsOnTheBoundary;++i)
        mat[i] = new int[nbOfUnclickedCellsOnTheBoundary+1]; //Adjacency matrix The last column contains the number of bombs
    for(unsigned int i=0;i<nbOfClickedCellsOnTheBoundary;i++)
    {//We fill up the matrix
        x =bordint[i]->x();
        y = bordint[i]->y();
        for(unsigned int j=0;j<nbOfUnclickedCellsOnTheBoundary;j++){
            if(celldistance(x,y, bordext[j]->x(),bordext[j]->y())<=1)
            {
                mat[i][j]=1;
            }
            else
            {
                mat[i][j]=0;
            }
        }
        mat[i][nbOfUnclickedCellsOnTheBoundary] = nbOfBombNeighbors(x,y)-nbOfFlagNeighbors(x,y);
    }
    unsigned int i,j,k,l;
    // On fait un gauss
    unsigned int rang = gauss(mat,nbOfUnclickedCellsOnTheBoundary,nbOfClickedCellsOnTheBoundary,bordext, clickedButtonIndex);
    //On regarde les composantes connexes independantes
    unsigned int compmec;
    j = 0;


    unsigned int ** compconnexes = new unsigned int *[nbOfUnclickedCellsOnTheBoundary];
    unsigned int * nombremeccomp = new unsigned int [nbOfUnclickedCellsOnTheBoundary];

    unsigned int nbcomp = fillConnectedComponents(mat, rang, nbOfUnclickedCellsOnTheBoundary, compconnexes, nombremeccomp, compmec, clickedButtonIndex);

    int bombespossible[nbOfUnclickedCellsOnTheBoundary];
    for(i=0;i<nbOfUnclickedCellsOnTheBoundary;i++){
        if(bordext[i]->isBomb()){
            bombespossible[i]=0;
        }else{
            bombespossible[i]=1;
        }
    }
    bool test;
    int som;
    unsigned int m;
    //for(i=0;i<nbcomp;i++){

    //for(m=0;m<nombremeccomp[i];m++){
    m=0;
    while(compconnexes[compmec][m]!=clickedButtonIndex) m++;
    //mectest = compconnexes[i][m];
    /*if(nombremeccomp[compmec]==1 && compconnexes[compmec][m]>=rang){
        bombespossible[compconnexes[compmec][m]]=2;
    }*/
    if(bombespossible[clickedButtonIndex]==1){
        for(j=0;j<(((unsigned int) 1)<<(nombremeccomp[compmec]));j++){
            test = true;
            for(k=0;k<nombremeccomp[compmec];k++){
                if(compconnexes[compmec][k]<rang){
                    som = 0;
                    for(l=0;l<m;l++){
                        if(mat[compconnexes[compmec][k]][compconnexes[compmec][l]]!=0){
                            som+=mat[compconnexes[compmec][k]][compconnexes[compmec][l]]*((j>>l)%2);
                        }
                    }
                    if(mat[compconnexes[compmec][k]][clickedButtonIndex]!=0){
                        som+=mat[compconnexes[compmec][k]][clickedButtonIndex];
                    }
                    for(l=m+1;l<nombremeccomp[compmec];l++){
                        if(mat[compconnexes[compmec][k]][compconnexes[compmec][l]]!=0){
                            som+=mat[compconnexes[compmec][k]][compconnexes[compmec][l]]*((j>>(l-1))%2);
                        }
                    }
                    if(som!=mat[compconnexes[compmec][k]][nbOfUnclickedCellsOnTheBoundary]){
                        test = false;
                        break;
                    }
                }
            }
            if(test){
                std::cout << "nb mec comp :" << nombremeccomp[compmec] << std::endl;
                bombespossible[clickedButtonIndex]=0;
                for(l=0;l<m;l++){
                    if(((j>>l)%2)==1){
                        bombespossible[compconnexes[compmec][l]]=0;
                    }
                }
                for(l=m+1;l<nombremeccomp[compmec];l++){
                    if(((j>>(l-1))%2)==1){
                        bombespossible[compconnexes[compmec][l]]=0;
                    }
                }
                break;
            }
        }
    }

    if(bombespossible[clickedButtonIndex]==0){
        int emptyCellReturn = emptyCell(mat,nbOfUnclickedCellsOnTheBoundary,rang,compconnexes,nombremeccomp,nbcomp,&bombespossible[0]);
        if(emptyCellReturn==-1){
            nbOfForcedLuckyGuesses++;
            std::cout << "Chance obligatoire!"<<std::endl;
            if(cellArray[larg][haut]->isBomb()){
                std::cout<<"bombe!"<<std::endl;
                findSolution(mat,nbOfUnclickedCellsOnTheBoundary,rang,compconnexes,nombremeccomp,clickedButtonIndex,compmec,bordext);
            }
        }else{
            std::cout << "Test chance !"<< emptyCellReturn <<" " << nbOfUnclickedCellsOnTheBoundary <<std::endl;
            bordext[emptyCellReturn]->setText(tr("i"));
            std::cout << "Test chance ! "<<std::endl;
        }
        nbOfLuckyGuesses++;
    }
    for(unsigned int i=0;i<nbOfClickedCellsOnTheBoundary;++i)
        delete mat[i];
    delete mat;
    for(unsigned int i=0;i<nbcomp;i++){
        delete compconnexes[i];
    }
    delete bordext;
    delete bordint;
    delete compconnexes;
    delete nombremeccomp;

}

bool MainWindow::isUnclickedAndNeighborOfDisplayedCell(unsigned int i,unsigned int j) const
{
    if(!(cellArray[i][j]->isClicked()) && nbOfFlagNeighbors(i,j)!=nbOfClickedNeighbors(i,j)){
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

int MainWindow::nbEmptyCells(){//Empty cell means that they are not next to a cell that is not clicked has not neighbors that have been clicked
    return countCells(&MainWindow::isEmptyCell);
}

int MainWindow::numberOfUnclickedAndNeighborOfDisplayedCell(){
    return countCells(&MainWindow::isUnclickedAndNeighborOfDisplayedCell);
}

int MainWindow::numberOfCellsDisplayedAndHasNonFlagNonDisplayedNeighbor(){
    return countCells(&MainWindow::isDisplayedAndHasNonFlagNonDisplayedNeighbor);
}
