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
#include <QStringList>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
class QTcpSocket;

class connection : public QObject
{
    Q_OBJECT
public:
    explicit connection(QObject *parent = 0);
    void connectToServer();
    int speler,spel;
    QTcpSocket* socket;
    QUdpSocket* UDPsocket;
    QHostAddress server;
    QHostAddress opponent;
    bool ableToWrite;
    bool gameStatus,attacker;
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
    void chatData();
    void sendChatData();
signals:
    void playfieldChanged();
    void msgChanged(QString info);
private:
    QString Broadcast();

};

#endif // CONNECT_H
