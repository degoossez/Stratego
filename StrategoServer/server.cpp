#include "server.h"
#define udpgetport 6666

Server::Server(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);
    connect(server,SIGNAL(newConnection()),this,SLOT(CreateTcp()));
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

    signalMapper = new QSignalMapper(this);
    connect(signalMapper,SIGNAL(mapped(int)),this,SLOT(processGameData(int)));

    /*tcpSocket = new QTcpSocket(this);
    tcpSocket->bind(41000, QTcpSocket::ShareAddress);
    connect(tcpSocket, SIGNAL(readyRead()),this,SLOT(processGameData())); */
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
                //CreateTcp(spel);
            }
        }
        else
        {
            QByteArray send_datagram = "SERVER-IS-FULL";
            udpSocket->writeDatagram(send_datagram,SenderAdress,6666);
        }
        qDebug() << SenderAdress;

    }
}
void Server::CreateTcp()
{
    int spelerId;
    if(spelers==1)
    {
        spelerId = ((spelers + 1) * spel) - 2;
        qDebug() << "spelerId:" << spelerId;
    }
    else if(spelers==2)
    {
        spelerId = (spelers * spel) -1;
        qDebug() << "spelerId2:" << spelerId;
    }
    QTcpSocket *tcpsock;
    tcpsock = server->nextPendingConnection();
    //tcpsock->waitForReadyRead(5000);
    signalMapper->setMapping(tcpsock, spelerId);
    connect(tcpsock, SIGNAL(readyRead()),signalMapper,SLOT(map()));
    //tcpsock->waitForBytesWritten(3000);
    //qDebug() << tcpsock->readAll();
    //tcpsock->write("");
    socketList.append(tcpsock);
    spelList.append(spel);
    toestandList.append(false);
    if(spelers==1)
    {
        spelers++;
    }
    else if(spelers==2)
    {
        spelers=1;
        spel++;
    }
}
void Server::processGameData(int spelerId) {
    qDebug() << "Processing game data:";
    QString input = socketList.at(spelerId)->readAll();
    qDebug() << input;
    if(input=="START")
    {
        toestandList[spelerId] = true;
        if(spelerId%2==0)
        {
            if(toestandList.at(spelerId+1)==true)
            {
                write_data(new QByteArray("START"),spelerId);
                write_data(new QByteArray("START"),spelerId+1);
            }
        }
        else
        {
            if(toestandList.at(spelerId-1)==true)
            {
                write_data(new QByteArray("START"),spelerId);
                write_data(new QByteArray("START"),spelerId-1);
            }
        }
    }
    if(input=="STOP")
    {
        toestandList[spelerId] = false;
        socketList.at(spelerId)->close();
        if(spelerId%2==0)
        {
            toestandList[spelerId+1]=false;
            write_data(new QByteArray("STOP"),spelerId+1);
            socketList.at(spelerId+1)->close();
        }
        else
        {
            toestandList[spelerId-1]=false;
            write_data(new QByteArray("STOP"),spelerId-1);
            socketList.at(spelerId-1)->close();
        }

    }
    QStringList list = input.split("/");
    if(list.at(0)=="FROM")
    {
        //ask oponent for pown
    }
    else(list.at()=="TO")
    {

    }
}
void Server::write_data(QByteArray *buffer, int spelerId){
    qDebug() << "sending";
        buffer->rightJustified(56,' ',true);
        socketList.at(spelerId)->write(*buffer);
        socketList.at(spelerId)->flush();
}
