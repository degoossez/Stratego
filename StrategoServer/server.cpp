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

}
void Server::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress SenderAdress;
        QString Data;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(),&SenderAdress);
        Data = QString::fromUtf8(datagram.data());
        if(Data == "DISCOVER-STRATEGO-SERVER")
        {
            if(spelers<=2)
            {
                    QByteArray send_datagram = "DISCOVERED-STRATEGO-SERVER";
                    udpSocket->writeDatagram(send_datagram,SenderAdress,6666);
                    //speler nummer
                    send_datagram = (QString::number(spelers)).toUtf8();
                    udpSocket->writeDatagram(send_datagram,SenderAdress,6666);
                    //game nummer
                    send_datagram = (QString::number(spel)).toUtf8();
                    udpSocket->writeDatagram(send_datagram,SenderAdress,6666);
            }
            else
            {
                QByteArray send_datagram = "SERVER-IS-FULL";
                udpSocket->writeDatagram(send_datagram,SenderAdress,6666);
            }
        }
        qDebug("SenderAdress");
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
        qDebug() << "spelerIdAanval:" << spelerId;
    }
    QTcpSocket *tcpsock;
    tcpsock = server->nextPendingConnection();
    signalMapper->setMapping(tcpsock, spelerId);
    connect(tcpsock, SIGNAL(readyRead()),signalMapper,SLOT(map()));
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
        toestandList[spelerId]=true;
        if(spelerId%2==0)
        {
            if(toestandList.size() > spelerId+1)
            {
                if(toestandList.at(spelerId+1)==true)
                {
                    write_data(new QByteArray("START/" + socketList.at(spelerId+1)->peerAddress().toString().toUtf8()),spelerId);
                    write_data(new QByteArray("START/" + socketList.at(spelerId)->peerAddress().toString().toUtf8()),spelerId+1);
                }
            }
        }
        else
        {
            if(toestandList.at(spelerId-1)==true)
            {
                write_data(new QByteArray("START/" + socketList.at(spelerId-1)->peerAddress().toString().toUtf8()),spelerId);
                write_data(new QByteArray("START/" + socketList.at(spelerId)->peerAddress().toString().toUtf8()),spelerId-1);
            }
        }
    }
    if(input=="STOP")
    {
        toestandList[spelerId]=false;
        socketList.at(spelerId)->close();
        if(spelerId%2==0)
        {
            if(toestandList.size() > spelerId+1)
            {
                toestandList.insert(spelerId+1,false);
                write_data(new QByteArray("STOP"),spelerId+1);
                socketList.at(spelerId+1)->close();
            }
        }
        else
        {
            toestandList.insert(spelerId-1,false);
            write_data(new QByteArray("STOP"),spelerId-1);
            socketList.at(spelerId-1)->close();
        }

    }
    QStringList list = input.split("/");
    if(list.at(0)=="AT")
    {
        int attack,spelerIdAanval;
        if(spelerId%2==0)
        {
             spelerIdAanval = spelerId+1;
        }
        else
        {
             spelerIdAanval = spelerId-1;
        }
        attack=pownList.at(spelerIdAanval);
        pown = list.at(1).toInt();
        bool win;
        bool draw=false;
        bool loose=false;
        bool winner=false;
        if(pown==15)
        {
            win =false;
            draw=false;
        }
        else if(pown==0)
        {
            qDebug("winner=true");
            winner=true;
        }
        else if(attack==2 && pown==10)
        {
            win = true;
        }
        else if(attack==3 && pown==11)
        {
            win = true;
        }
        else if(attack > pown)
        {
            win = true;
        }
        else if(attack < pown)
        {
            loose = true;
            win = false;
        }
        else if(attack==pown)
        {
            draw = true;
        }
        if(winner==true)
        {
            qDebug("WIN");
            //attacker
            write_data(new QByteArray("WINNER/"+QByteArray::number(attack)),spelerIdAanval);
            //defender
            write_data(new QByteArray("LOSER/"+QByteArray::number(pown)),spelerId);
            toestandList[spelerId]=false;
            socketList.at(spelerId)->close();
            if(spelerId%2==0)
            {
                toestandList.insert(spelerId+1,false);
                socketList.at(spelerId+1)->close();
            }
            else
            {
                toestandList.insert(spelerId-1,false);
                socketList.at(spelerId-1)->close();
            }
        }
        else if(draw==true)
        {
            qDebug("draw==true");
            //defender
            write_data(new QByteArray("DRAW/"+QByteArray::number(attack)),spelerId);
            //attacker
            write_data(new QByteArray("DRAW/"+QByteArray::number(attack)),spelerIdAanval);
        }
        else if(win==true)
        {
            qDebug("win==true");
            //attacker
            write_data(new QByteArray("WON/"+QByteArray::number(pown)),spelerIdAanval);
            //defender
            write_data(new QByteArray("LOST/"+QByteArray::number(attack)),spelerId);
        }
        else if(win==false && draw==false && loose==false) //nog aan te passen
        {
            qDebug("win==false draw==false");
            //attacker
            write_data(new QByteArray("MOVE/"+QByteArray::number(attack)),spelerIdAanval);
            //defender
            write_data(new QByteArray("MOVE/"+QByteArray::number(pown)),spelerId);
        }
        else if(loose==true)
        {
            qDebug("win == false");
            //attacker
            write_data(new QByteArray("LOST/"+QByteArray::number(pown)),spelerIdAanval);
            //defender
            write_data(new QByteArray("WON/"+QByteArray::number(attack)),spelerId);
        }
    }
    if(list.at(0)=="FROM")
    {
        if(spelerId < pownList.size())
        {
            pownList.removeAt(spelerId);
        }
        int x = 9-list.at(1).toInt();
        int y = 9-list.at(2).toInt();
        pownList.insert(spelerId,list.at(3).toInt()); //welke pion word verzet (aanvaller)
        if(spelerId%2==0)
        {
            write_data(new QByteArray("FROM/"+ QByteArray::number(x) + "/" + QByteArray::number(y)),spelerId+1);
        }
        else
        {
            write_data(new QByteArray("FROM/"+ QByteArray::number(x) + "/" + QByteArray::number(y)),spelerId-1);
        }
    }
    else if(list.at(0)=="TO")
    {
        //ask oponent for pown
        QByteArray *from = new QByteArray("POWN/" + QByteArray::number(9-list.at(1).toInt()) + "/" + QByteArray::number(9-list.at(2).toInt()));
        if(spelerId%2==0)
        {
            write_data(from,spelerId+1);
        }
        else
        {
            write_data(from,spelerId-1);
        }
    }
}
void Server::write_data(QByteArray *buffer, int spelerId){
        socketList.at(spelerId)->write(*buffer);
        socketList.at(spelerId)->flush();
}
