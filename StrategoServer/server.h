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
#include <QSignalMapper>
class QTcpSocket;
//! [0]
class Server : public QObject
{
    Q_OBJECT

public:
    Server(QObject *parent = 0);
    int spelers,spel;
    QTcpServer *server;
    QList<QTcpSocket*> socketList;
    QList<int> spelList;
    QList<bool> toestandList;
    QSignalMapper *signalMapper;
public slots:
    void processGameData(int spelerId);
    void processPendingDatagrams();
    void CreateTcp();
    void write_data(QByteArray *buffer,int spelerId);
private:
    QUdpSocket *udpSocket;
    QTcpSocket *tcpSocket;

};
//! [0]

#endif // HEADER_H
