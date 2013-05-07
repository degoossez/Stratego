#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostInfo>
#include <cstdio>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
class QTcpSocket;

class connection : public QObject
{
    Q_OBJECT
public:
    explicit connection(QObject *parent = 0);
    void connectToServer();
    void write_data(QByteArray *buffer);
    int speler,spel;
    QTcpSocket* socket;
    bool ableToWrite,gameStatus;
    int spelveld[9][9];
public slots:
private slots:
    void on_connected();
    void on_disconnected();
    void printError(QAbstractSocket::SocketError);
    void incommingData();
signals:
    void playfieldChanged(int[][9]);
private:
    QString Broadcast();
    
};

#endif // CONNECT_H
