#include "server.h"
#define udpgetport 6666

Server::Server(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);
    //connect(server,SIGNAL(newConnection()),this,SLOT(ConnectClients()));
    if(!server->listen(QHostAddress::Any,41000))
    {
        qDebug() <<"SERVER COULD NOT BE STARTED\n";
    }
    else
    {
        qDebug() <<"SERVER STARTED\n";
    }
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(40000, QUdpSocket::ShareAddress);
    spelers = 1;
    spel=1;
    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(processPendingDatagrams()));

    tcpSocket = new QTcpSocket(this);
    tcpSocket->bind(41000, QTcpSocket::ShareAddress);
    connect(tcpSocket, SIGNAL(readyRead()),this,SLOT(processGameData()));
}
void Server::processGameData() {
    qDebug() << "HALLO";
}
void Server::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress SenderAdress;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(),&SenderAdress);
        //statusLabel->setText(tr("Received datagram: \"%1\"").arg(datagram.data()));
        if(spelers<=2)
        {
            if(QString::fromUtf8(datagram.data()) == "DISCOVER-STRATEGO-SERVER")
            {
                QByteArray send_datagram = "DISCOVERED-STRATEGO-SERVER";
                udpSocket->writeDatagram(send_datagram,SenderAdress,6666);
                //speler nummer
                send_datagram = (QString::number(spelers)).toUtf8();
                udpSocket->writeDatagram(send_datagram,SenderAdress,6666);
                //game nummer
                send_datagram = (QString::number(spel)).toUtf8();
                udpSocket->writeDatagram(send_datagram,SenderAdress,6666);
                spelers++;
                CreateTcp(spel);
            }
        }
        else
        {
            QByteArray send_datagram = "SERVER-IS-FULL";
            udpSocket->writeDatagram(send_datagram,SenderAdress,6666);
            spelers=1;
            spel++;
        }
        qDebug() << SenderAdress;

    }
}
void Server::CreateTcp(int spel)
{
    QTcpSocket *tcpsock = new QTcpSocket();
    tcpsock = server->nextPendingConnection();
    socketList.append(tcpsock);
    spelList.append(spel);
}
