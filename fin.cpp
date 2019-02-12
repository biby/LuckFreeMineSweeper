#include "fin.h"
#include "ui_fin.h"

EndGame::EndGame(MainWindow *parent,int score) :
    QDialog(parent),
    ui(new Ui::EndGameDisplay)
{
    w =parent;
    ui->setupUi(this);
    ui->Tempsecoule->setText(w->clock->time->text());
    char q[60];
    sprintf(q,"Nombre de clicks au hasard : %i",w->nbOfLuckyGuesses);
    ui->Totalchance->setText(tr(q));
    sprintf(q,"Nombre fois sans possibilite : %i",w->nbOfForcedLuckyGuesses);
    ui->Totalchanceobl->setText(tr(q));
    setObjectName(tr("Felicitation"));
    sprintf(q,"Votre meilleur score est : %d secondes",score);
    ui->bestscore->setText(tr(q));
}

EndGame::~EndGame()
{
    delete ui;
}
