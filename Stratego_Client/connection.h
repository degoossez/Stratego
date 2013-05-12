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
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QStringList>

class QTcpSocket;

class connection : public QObject
{
    Q_OBJECT
public:
    explicit connection(QObject *parent = 0);
    void connectToServer();
    int speler,spel;
    QTcpSocket* socket;
    bool ableToWrite,gameStatus,attacker;
    int spelveld[10][10];
    int x,y,x2,y2;
    int defx,defy,defx2,defy2;
public slots:
private slots:
    void on_connected();
    void on_disconnected();
    void printError(QAbstractSocket::SocketError);
    void incommingData();
    void write_data(QByteArray *buffer);
signals:
    void playfieldChanged();
private:
    QString Broadcast();

};

#endif // CONNECT_H
