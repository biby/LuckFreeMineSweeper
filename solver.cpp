#include "solver.h"
#include <casedem.h>
#include <vector>
#include <algorithm>
#include <mainwindow.h>
#include <math.h>

Solver::Solver(MainWindow *w,unsigned int x, unsigned int y):w(w),clickedButtonX(x),clickedButtonY(y)
{
    //Todo need to add access to CellArray
    nbOfClickedCellsOnTheBoundary = w->filterCells(&bordint, &MainWindow::isDisplayedAndHasNonFlagNonDisplayedNeighbor);
    nbOfUnclickedCellsOnTheBoundary = w->filterCells(&bordext, &MainWindow::isUnclickedAndNeighborOfDisplayedCell);
    findClickedButtonIndex();
    initializeMatrix();
    compconnexes = new unsigned int *[nbOfUnclickedCellsOnTheBoundary];
    nombremeccomp = new unsigned int [nbOfUnclickedCellsOnTheBoundary];
    rankOfConnectedComponent= new unsigned int [nbOfUnclickedCellsOnTheBoundary];
    bombespossible = new bool[nbOfUnclickedCellsOnTheBoundary];
    std::fill(bombespossible,bombespossible+nbOfUnclickedCellsOnTheBoundary,false);
    foundsafesolution = false;
    compmec = nbOfUnclickedCellsOnTheBoundary;
}

Solver::~Solver()
{
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
    delete rankOfConnectedComponent;
    delete bombespossible;
    if(foundsafesolution)
    {
        delete safesolution;
    }
    delete connectedcomponentsrow; //Should a check be done to see if it exists?
}

void Solver::findClickedButtonIndex()
{
    clickedButtonIndex=0;
    for(unsigned int i=0;i<nbOfUnclickedCellsOnTheBoundary;++i)
    {
        if(bordext[i]->x() == clickedButtonX && bordext[i]->y() == clickedButtonY)
        {
            clickedButtonIndex = i;
            break;
        }
    }
}

void Solver::initializeMatrix()
{
    unsigned int x,y;
    mat = new int*[nbOfClickedCellsOnTheBoundary];
    for(unsigned int i=0;i<nbOfClickedCellsOnTheBoundary;++i)
        mat[i] = new int[nbOfUnclickedCellsOnTheBoundary+1]; //Adjacency matrix The last column contains the number of bombs
    for(unsigned int i=0;i<nbOfClickedCellsOnTheBoundary;i++)
    {//We fill up the matrix
        x =bordint[i]->x();
        y = bordint[i]->y();
        for(unsigned int j=0;j<nbOfUnclickedCellsOnTheBoundary;j++){
            if(w->celldistance(x,y, bordext[j]->x(),bordext[j]->y())<=1)
            {
                mat[i][j]=1;
            }
            else
            {
                mat[i][j]=0;
            }
        }
        mat[i][nbOfUnclickedCellsOnTheBoundary] = w->nbOfBombNeighbors(x,y)-w->nbOfFlagNeighbors(x,y);
    }
}

void Solver::simplify()
{
    for(unsigned int i=0;i<nbOfClickedCellsOnTheBoundary;i++)//We remove cells that are trivially bombs
    {
        int mini = 0;
        int maxi = 0;
        for(unsigned int j=0;j<nbOfUnclickedCellsOnTheBoundary;j++)
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
        if(mini==mat[i][nbOfUnclickedCellsOnTheBoundary])
        {
            for(unsigned int l=0;l<nbOfUnclickedCellsOnTheBoundary;++l)
            {
                if(mat[i][l]>0)
                {
                    for(unsigned int k=0; k<nbOfClickedCellsOnTheBoundary;++k)
                    {
                        if(k==i) continue;
                        mat[k][l]=0;
                    }

                }
                else if(mat[i][l]<0)
                {
                    for(unsigned int k=0; k<nbOfClickedCellsOnTheBoundary;++k)
                    {
                        if(k==i) continue;
                        mat[k][nbOfUnclickedCellsOnTheBoundary]-=mat[k][l];
                        mat[k][l]=0;
                    }
                }
            }
        }
        else if(maxi==mat[i][nbOfUnclickedCellsOnTheBoundary])
        {
            for(unsigned int l=0;l<nbOfUnclickedCellsOnTheBoundary;++l)
            {
                if(mat[i][l]<0)
                {
                    for(unsigned int k=0; k<nbOfClickedCellsOnTheBoundary;++k)
                    {
                        if(k==i) continue;
                        mat[k][l]=0;
                    }

                }
                else if(mat[i][l]>0)
                {
                    for(unsigned int k=0; k<nbOfClickedCellsOnTheBoundary;++k)
                    {
                        if(k==i) continue;
                        mat[k][nbOfUnclickedCellsOnTheBoundary]-=mat[k][l];
                        mat[k][l]=0;
                    }
                }
            }
        }
    }
}

void Solver::fillConnectedComponents()
{

    nbcomp = 0;
    unsigned int nbmec;
    unsigned int i,placedCells,k,l;
    bool isPlaced[nbOfUnclickedCellsOnTheBoundary];
    std::fill(isPlaced,isPlaced + nbOfUnclickedCellsOnTheBoundary,false);

    connectedcomponentsrow = new int[rang];
    std::fill(connectedcomponentsrow,connectedcomponentsrow+rang,-1);
    placedCells=0;

    while(placedCells<nbOfUnclickedCellsOnTheBoundary){
        rankOfConnectedComponent[nbcomp]=0;
        compconnexes[nbcomp] = new unsigned int[nbOfUnclickedCellsOnTheBoundary];
        for(k = 0;k<nbOfUnclickedCellsOnTheBoundary;k++){
            if(!isPlaced[k]){
                break;
            }
        }
        if(k == clickedButtonIndex){
            compmec = nbcomp;
            indexmec = 0;
        }
        isPlaced[k] = true;
        compconnexes[nbcomp][0]=k;
        placedCells++;
        k=0;
        nbmec = 1;
        while(k<nbmec){
            l=compconnexes[nbcomp][k];
            for(i=0;i<rang;i++){
                if(connectedcomponentsrow[i]!=-1) continue;
                if(mat[i][l]!=0)
                {
                    connectedcomponentsrow[i]=nbcomp;
                    rankOfConnectedComponent[nbcomp]++;
                    for(unsigned int iter=0;iter<nbOfUnclickedCellsOnTheBoundary;iter++)
                    {
                        if(mat[i][iter]!=0 && !isPlaced[iter])
                        {
                            compconnexes[nbcomp][nbmec++] = iter;
                            isPlaced[iter] = true;
                            if(iter == clickedButtonIndex)
                            {
                                compmec = nbcomp;
                            }
                            placedCells++;

                        }
                    }
                }
            }
            k++;
        }
        nombremeccomp[nbcomp]=nbmec;
        std::sort(compconnexes[nbcomp],compconnexes[nbcomp]+nbmec); //Screws up indexmec... Has to be fixed.
        if(compmec==nbcomp)
        {
            indexmec = std::distance(compconnexes[nbcomp],std::find(compconnexes[nbcomp],compconnexes[nbcomp]+nbmec,clickedButtonIndex));
        }
        nbcomp++;

    }

}

void Solver::gauss()// The matrix has nbRow rows and nbCol+1 col, but we apply gauss only on the nbCol columns. Col is a column we keep track of during swaps in the process.
{
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k,l;
    int a;
    for(i=0;i<nbOfClickedCellsOnTheBoundary;i++)//We remove cells that are trivially bombs
    {
        int numberOfAdjacentCells = 0;
        for(j=0;j<nbOfUnclickedCellsOnTheBoundary;j++)
        {
            if(mat[i][j]!=0)
            {
                numberOfAdjacentCells++;
            }
        }
        if(numberOfAdjacentCells == mat[i][nbOfUnclickedCellsOnTheBoundary])
        {
            for(j=0;j<nbOfUnclickedCellsOnTheBoundary;j++)
            {
                if(mat[i][j]!=0)
                {
                    for(k=0;k<nbOfClickedCellsOnTheBoundary;k++)
                    {
                        if(k==i) continue;
                        if(mat[k][j]!=0)
                        {
                            mat[k][nbOfUnclickedCellsOnTheBoundary]-=mat[k][j];
                            mat[k][j]=0;
                        }
                    }
                }
            }
        }
    }
    while(true)
    {
        i=0;
        j=0;
        while(i<nbOfClickedCellsOnTheBoundary && j<nbOfUnclickedCellsOnTheBoundary){
            if(mat[i][j]==0){
                k=i+1;
                while(k<nbOfClickedCellsOnTheBoundary && mat[k][j]==0){
                    k++;
                }
                if(k==nbOfClickedCellsOnTheBoundary){
                    j++;
                    continue;
                }
                std::swap(mat[i],mat[k]);
            }
            for(k=i+1;k<nbOfClickedCellsOnTheBoundary;k++)
            {
                if(mat[k][j] != 0){
                    a = mat[k][j];
                    for(l = 0; l< nbOfUnclickedCellsOnTheBoundary+1;l++){
                        mat[k][l] = mat[i][j]*mat[k][l]-a*mat[i][l];
                    }
                }
            }
            i++;
            j++;
        }
        rang = i;

        simplify();




        j = 0;
        while(j<rang){
            if(mat[j][j]==0){
                k=j+1;
                while(mat[j][k]==0)
                {
                    ++k;
                }
                if(k>=nbOfUnclickedCellsOnTheBoundary)
                {
                    ++j;
                    continue;
                }
                for(l = 0; l< nbOfClickedCellsOnTheBoundary;l++){
                    std::swap(mat[l][j],mat[l][k]);
                }
                std::swap(bordext[j],bordext[k]);
                if(clickedButtonIndex==j){
                    clickedButtonIndex = k;
                }else if(clickedButtonIndex == k){
                    clickedButtonIndex = j;
                }
            }
            j++;
        }
        j--;
        bool flag = true;
        for(i=0;i<rang;++i)
        {
            if(mat[i][i]==0)
            {
                flag=false;
                break;
            }
        }
        if(flag) break;
    }

    for(j=rang-1;j>0;--j)
    {
        //if(mat[j][j]!=0)
        {
            for(k = 0;k<j;++k)
            {
                if(mat[k][j]!=0){
                    a = mat[k][j];
                    for(l = j; l< nbOfUnclickedCellsOnTheBoundary+1;l++){
                        mat[k][l] = mat[j][j]*mat[k][l]-a*mat[j][l];
                    }
                }
            }
        }
        /*else
        {
            throw "ops";
        }*/
    }
}

void Solver::possiblebombtester(unsigned int comptotest)
{
    unsigned int j,k;
    bool test;
    int sum;
    unsigned int bombs[nombremeccomp[comptotest]];
    //std::cout<< "Print connected component" << std::endl;
    /*QColor color(rand()%256,rand()%256,rand()%256);

    for(k=0;k<nombremeccomp[comptotest];k++)
    {
        QPalette pal = bordext[compconnexes[comptotest][k]]->palette();
        pal.setColor(QPalette::Button, color);
        bordext[compconnexes[comptotest][k]]->setAutoFillBackground(true);
        bordext[compconnexes[comptotest][k]]->setPalette(pal);
        bordext[compconnexes[comptotest][k]]->update();
    }*/
    /*for(unsigned int line=0;line<rang;++line)
    {
        if(connectedcomponentsrow[line]!=comptotest)
        {
            continue;
        }
        for(k=0;k<nombremeccomp[comptotest];k++)
        {
            std::cout << mat[line][compconnexes[comptotest][k]] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << rankOfConnectedComponent[comptotest] << std::endl;
    std::cout << pow(2,(nombremeccomp[comptotest]-rankOfConnectedComponent[comptotest])) << " " << (1<<(nombremeccomp[comptotest]-rankOfConnectedComponent[comptotest])) << std::endl;
    if(pow(2,(nombremeccomp[comptotest]-rankOfConnectedComponent[comptotest])) != (1<<(nombremeccomp[comptotest]-rankOfConnectedComponent[comptotest])))
    {
        throw "blob";
    }*/
    for(j=0;j<(1<<(nombremeccomp[comptotest]-rankOfConnectedComponent[comptotest]));j++)
    {
        std::fill(bombs,bombs+rankOfConnectedComponent[comptotest],0);
        for(k=rankOfConnectedComponent[comptotest];k<nombremeccomp[comptotest];k++)
        {
            bombs[k]=((j>>(k-rankOfConnectedComponent[comptotest]))%2);
            std::cout << bombs[k] << " ";
        }
        std:: cout << std::endl;
        test = true;
        for(unsigned int line=0;line<rang;++line)
        {
            if(connectedcomponentsrow[line]!=comptotest)
            {
                continue;
            }
            sum = mat[line][nbOfUnclickedCellsOnTheBoundary];
            for(k=rankOfConnectedComponent[comptotest];k<nombremeccomp[comptotest];k++)
            {
                sum-=mat[line][compconnexes[comptotest][k]]*bombs[k];
            }

            if(sum==mat[line][line])
            {
                k=0;
                while(compconnexes[comptotest][k]!=line && k<rankOfConnectedComponent[comptotest]) k++;
                if(k>=rankOfConnectedComponent[comptotest]) throw "error";
                bombs[k]=1;
            }
            else if(sum==0)
            {
                k=0;
                while(compconnexes[comptotest][k]!=line && k<rankOfConnectedComponent[comptotest]) k++;
                if(k>=rankOfConnectedComponent[comptotest]) throw "error";
                bombs[k]=0;
            }
            else
            {
                test=false;
                break;
            }

        }
        if(test)
        {
            for(k=0;k<nombremeccomp[comptotest];k++)
            {
                if(bombs[k]==1)
                {
                    bombespossible[compconnexes[comptotest][k]]=true;
                }
            }

            if(foundsafesolution == false && comptotest==compmec && bombs[indexmec]==0)
            {
                foundsafesolution = true;
                safesolution = new unsigned int[nombremeccomp[compmec]];
                std::copy(bombs,bombs+nombremeccomp[compmec],safesolution);
            }
        }
    }
    std:: cout << std::endl;
}

void Solver::coutmap()
{
    for(unsigned int i =0;i<nbOfClickedCellsOnTheBoundary;++i)
    {
        for(unsigned int j=0;j<nbOfUnclickedCellsOnTheBoundary+1;++j)
        {
            std::cout << mat[i][j] << " ";
        }
        std::cout << std::endl;
    }
    bool test = true;
    for(unsigned int j=0;j<nbOfUnclickedCellsOnTheBoundary+1;++j)
    {
        bool test2 = false;
        for(unsigned int i =0;i<nbOfClickedCellsOnTheBoundary;++i)
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

void Solver::luckRemover()
{
    unsigned int i,k,l;
    // On fait un gauss
    gauss();
    coutmap();

    //Computes the connected components. Sets compmec and indexmec.
    fillConnectedComponents();

    /*for(i=0;i<nbOfUnclickedCellsOnTheBoundary;i++)
    {
        if(bordext[i]->isBomb())
        {
            bombespossible[i]=true;
        }
        else
        {
            bombespossible[i]=false;
        }
    }*/

    possiblebombtester(compmec);

    if(bombespossible[clickedButtonIndex] && foundsafesolution)//The clicked place is identify as potentially containing a bomb
    {
        for(unsigned int comp=0;comp<nbcomp;++comp) //Filling up the other connected components to check if there is a safe cell.
        {
            if(comp!=compmec)
            {
                possiblebombtester(comp);
            }
        }
        bool forcedguess=true;
        unsigned int safecell;
        for(safecell=0;safecell<nbOfUnclickedCellsOnTheBoundary;safecell++)// Look for a safe cell.
        {
            if(bombespossible[safecell]==false)
            {
                forcedguess = false;
                break;
            }
        }
        if(forcedguess && foundsafesolution)//If there were no safecells;
        {
            w->nbOfForcedLuckyGuesses++;
            std::cout << "Forced luck!"<<std::endl;
            if(bordext[clickedButtonIndex]->isBomb())
            {
                std::cout<<"There was a mine!"<<std::endl;
                for(l=0;l<nombremeccomp[compmec];l++) //Should do something so the number of mines stay constant.
                {
                    if(safesolution[l]==1)
                    {
                        bordext[compconnexes[compmec][l]]->setBomb(true);
                    }else{
                        bordext[compconnexes[compmec][l]]->setBomb(false);
                    }
                }
            }

        }
        else
        {
            bordext[safecell]->setText("S");
            std::cout << "Trying your luck! "<<std::endl;
        }
        w->nbOfLuckyGuesses++;
    }
}
