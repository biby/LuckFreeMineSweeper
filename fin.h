#ifndef FIN_H
#define FIN_H

#include <QDialog>
#include "mainwindow.h"

class MainWindow;

namespace Ui {
class EndGameDisplay;
}

class EndGame : public QDialog
{
    Q_OBJECT
    
public:
    explicit EndGame(MainWindow *parent = 0,int score = 100000);
    ~EndGame();
    MainWindow *w;
    Ui::EndGameDisplay *ui;
private:

};

#endif // FIN_H
