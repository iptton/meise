/**************************************************************************
**   Special keywords: pxz 2013-2-8 2013
**   Environment variables: 
**   To protect a percent sign, use '%'.
**************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void connected();
    void readyRead();
    void socketError(QAbstractSocket::SocketError err);
    void processPendingDatagrams();

    void connectReadyRead();

    void startConnectionSocket();
    void socketDisconnected();

    void connectToServer();
    void onConnectBtnClicked();

private:


    void startDataSocket();

    void parsePackage(QByteArray ba);
    void showFrame(const QByteArray& ba);
    long readLeft;
    long lastSize;

    QString dataAddress;
    int dataPort;

    QByteArray tmpHeader;
    QByteArray tmpImageData;
    QHostAddress groupAddress;
    QUdpSocket *dataSocket;
    QByteArray brocastData;

    QUdpSocket *connectSocket;



    enum PackageState{
        Parsing,
        GotPackage
    };
    enum PackageState state;
    QTcpSocket *socket;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
