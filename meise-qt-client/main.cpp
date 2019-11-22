/**************************************************************************
**   Special keywords: pxz 2013-2-8 2013
**   Environment variables: 
**   To protect a percent sign, use '%'.
**************************************************************************/

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
