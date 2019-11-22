/**************************************************************************
**   Special keywords: pxz 2013-2-8 2013
**   Environment variables: 
**   To protect a percent sign, use '%'.
**************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    connectSocket(0),
    dataSocket(0){
    ui->setupUi(this);
    startConnectionSocket();
    connect(ui->pushButton,SIGNAL(clicked()),
            this,SLOT(startConnectionSocket()));
    connect(ui->connectBtn,SIGNAL(clicked()),this,SLOT(onConnectBtnClicked()));

}
QString getStringFromHexAddress(QString hex){
    QString ret = "";
    for(int i=0,l=4;i<l;++i){
        QString tmp2 = hex.mid(i*2,2);
        bool isSuccess = false;
        int tmp = tmp2.toInt(&isSuccess,16);
        qDebug()<<"tmp "<<tmp<< (isSuccess?"success":"false");
        ret += QString::number(tmp);
        if(i!=3){
            ret+=".";
        }
    }
    return ret;
}
void MainWindow::onConnectBtnClicked(){

    dataAddress = getStringFromHexAddress(ui->lineEdit->text());
    dataPort = 1234;
    MainWindow::connectToServer();
}



void MainWindow::startConnectionSocket(){
    if(connectSocket && connectSocket->isValid()){
        connectSocket->close();
        qDebug()<<"close old on";
        delete connectSocket;
    }
    connectSocket = new QUdpSocket(this);
    bool success = connectSocket->bind(7788);
    if(!success){
        qDebug()<<"error on binding";
    }
    connect(connectSocket, SIGNAL(readyRead()),
                this, SLOT(connectReadyRead()));
}

void MainWindow::connectReadyRead(){
    if(!connectSocket->hasPendingDatagrams()){
        return;
    }

    QByteArray datagram;
    datagram.resize(connectSocket->pendingDatagramSize());
    connectSocket->readDatagram(datagram.data(), datagram.size());
    qDebug()<<""+datagram<<datagram.size();

    //获取ip地址和端口
    QList<QByteArray> data = datagram.split(':');
    dataPort = QString(data[2]).toInt();
    qDebug()<<"port ="<<dataPort;
    QString dataAddress = "";
    QString debug = "c0a80065";
    for(int i=0,l=4;i<l;++i){
        QByteArray ba2 = data[4].mid(i*2,2);
        qDebug()<<ba2;
        bool isSuccess = false;
        int tmp = QString(ba2).toInt(&isSuccess,16);
        qDebug()<<"tmp "<<tmp<< (isSuccess?"success":"false");
        dataAddress += QString::number(tmp);
        if(i!=3){
            dataAddress+=".";
        }
    }
    qDebug()<<dataAddress;
    connectToServer();
    connectSocket->abort();
}

void MainWindow::connectToServer(){
    qDebug()<<dataAddress<<dataPort;
    if(socket && socket->isOpen()){
        socket->abort();
        delete socket;
    }
    socket = new QTcpSocket(this);
    state = GotPackage;
    connect(socket,SIGNAL(connected()),SLOT(connected()));
    connect(socket,SIGNAL(readyRead()),SLOT(readyRead()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(socketError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(disconnected()),SLOT(socketDisconnected()));
//    socket->connectToHost(QHostAddress(dataAddress),dataPort);

    socket->connectToHost(QHostAddress(dataAddress),dataPort);
}

void MainWindow::socketError(QAbstractSocket::SocketError err){
    qDebug()<<"server socket error"<<err;
    startConnectionSocket();
}
void MainWindow::socketDisconnected(){
    qDebug()<<"server socket disconnected";
}

void MainWindow::connected(){
    qDebug()<<"server socket connected";
}
void MainWindow::readyRead(){
    /*
        g_netpackge_head[0] = 'S';
        g_netpackge_head[1] = 'E';
        g_netpackge_head[2] = 'S';
        g_netpackge_head[3] = 'E';
        g_netpackge_head[4] = 'd';
        g_netpackge_head[5] = (byte) ((size >> 16) & 0xff);
        g_netpackge_head[6] = (byte) ((size >> 8) & 0xff);
        g_netpackge_head[7] = (byte) ((size >> 0) & 0xff);
    */


    QByteArray ba = socket->readAll();
    parsePackage(ba);
}


void MainWindow::parsePackage(QByteArray ba){

    int tmpeHeaderSize=tmpHeader.size();
    while(!ba.isEmpty()){
//        qDebug()<<"stream length = "<<ba.length();
        switch(state){
        case GotPackage:
            tmpHeader.append(ba);
            if(tmpHeader.size()<8){
                return;
            }
            lastSize = (tmpHeader[5] & 0xff) << 16;
            lastSize += (tmpHeader[6] & 0xff) << 8;
            lastSize += (tmpHeader[7] & 0xff);
            tmpHeader.clear();
            readLeft = lastSize;
            ba.remove(0,8-tmpeHeaderSize);
            tmpImageData.clear();
//            qDebug()<<"clear..";
//            qDebug()<<"size = "<<lastSize;
            state = Parsing;
            break;
        case Parsing:
            if(ba.length() <= readLeft){
                tmpImageData.append(ba);
                readLeft -= ba.size();
                ba.clear();
            }else{
                tmpImageData.append(ba.left(readLeft));
                ba.remove(0,readLeft);
                readLeft = 0;
            }
            if(readLeft == 0){
                state = GotPackage;
                showFrame(tmpImageData);
            }else{
                state = Parsing;
            }
            break;
        }
    }
}

void MainWindow::processPendingDatagrams(){
    while (dataSocket->hasPendingDatagrams()) {
         QByteArray datagram;
         datagram.resize(dataSocket->pendingDatagramSize());
         dataSocket->readDatagram(datagram.data(), datagram.size());
//         qDebug()<<tr("Received datagram: \"%1\"")
//                              .arg(datagram.data());
        brocastData.append(datagram);
    }
}


void MainWindow::showFrame(const QByteArray& ba){

    QPixmap pixmap1(480,320);

    pixmap1.loadFromData(ba);
    /*
    QMatrix rm;
    rm.rotate(40);
    pixmap1.transformed(rm);
    */
    ui->label->setPixmap(pixmap1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
