#ifndef HEADER_H
#define HEADER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QHostInfo>
#include <cstdio>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QList>
class QTcpSocket;
//! [0]
class Server : public QObject
{
    Q_OBJECT

public:
    Server(QObject *parent = 0);
    void CreateTcp(int);
    int spelers,spel;
    QTcpServer *server;
    QList<QTcpSocket*> socketList;
    QList<int> spelList;

public slots:
    void processGameData();
    void processPendingDatagrams();
private:
    QUdpSocket *udpSocket;
    QTcpSocket *tcpSocket;

};
//! [0]

#endif // HEADER_H
