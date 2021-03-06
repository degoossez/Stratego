#include "connection.h"


#define udpgetport 6666

connection::connection(QObject *parent) :
    QObject(parent)
{
    ableToWrite=false;
    socket = new QTcpSocket(this);
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(printError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(socket,SIGNAL(disconnected()), this, SLOT(on_disconnected()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(incommingData()));

    UDPsocket = new QUdpSocket(this);
    UDPsocket->bind(6666, QUdpSocket::ShareAddress);
    connect(UDPsocket, SIGNAL(readyRead()),this, SLOT(chatData()));
}
void connection::sendChatData()
{
    QUdpSocket udpSocketSend;
    QByteArray send_datagram;
    qDebug("send");
    QByteArray datagram = Data.toUtf8();
    send_datagram = datagram + "@@" + QByteArray::number(speler);
    qDebug() << send_datagram;
    udpSocketSend.writeDatagram(send_datagram, opponent, 6666);
}

void connection::chatData()
{
    qDebug("beginchatdata");
    QByteArray receive_datagram;
    quint16 serverPort;
    qDebug() << UDPsocket->pendingDatagramSize();
    receive_datagram.resize(UDPsocket->pendingDatagramSize());
    UDPsocket->readDatagram(receive_datagram.data(),receive_datagram.size(),&opponent,&serverPort);
    qDebug() << receive_datagram.data();
    QString input = QString::fromUtf8(receive_datagram.data());
    QStringList list = input.split("@@");
    qDebug("-----------------------");
    if(list.at(1)!=QString::number(speler))
    {
        QString info = "Openent: " + list.at(0) + "\n";
        emit msgChanged(info);
    }
    qDebug("eindechatdata");
}
void connection::printError(QAbstractSocket::SocketError Error)
{
    emit lblChanged(Error + "\n");
}

QString connection::Broadcast() {
    // Om ip adressen te ontdekken
    QNetworkAddressEntry inter;
    // sockets aanmaken en verbinden met enerzijds broadcast en anderzijds een luister poort
    QUdpSocket udpSocketSend;
    QUdpSocket udpSocketGet;
    udpSocketSend.connectToHost(inter.broadcast(), 40000);
    if(udpSocketGet.bind(udpgetport,QUdpSocket::ShareAddress))
        emit lblChanged("[INFO] Could properly bind udpSocketget to " + QString::number(udpgetport) + "\n");
    else emit lblChanged("[INFO] Couldn't properly bind udpSocketget to " + QString::number(udpgetport) + "\n");
    // Pakket verzenden
    QByteArray send_datagram = "DISCOVER-STRATEGO-SERVER";
    // Optimalisatie voor in de loop
    QByteArray receive_datagram;

    quint16 serverPort;
    forever{
        udpSocketSend.writeDatagram(send_datagram, QHostAddress::Broadcast, 40000);
        if(udpSocketGet.waitForReadyRead(3000)){
            receive_datagram.resize(udpSocketGet.pendingDatagramSize());
            udpSocketGet.readDatagram(receive_datagram.data(),receive_datagram.size(),&server,&serverPort);
            if(QString::fromUtf8(receive_datagram.data()) == "DISCOVERED-STRATEGO-SERVER")
            {
                receive_datagram.resize(udpSocketGet.pendingDatagramSize());
                udpSocketGet.readDatagram(receive_datagram.data(),receive_datagram.size(),&server,&serverPort);
                emit lblChanged("[INFO] PLAYER DATA: "+ QString::fromUtf8(receive_datagram.data()) +"\n");
                speler = receive_datagram.toInt();
                if(speler==1)
                {
                    attacker=true;
                }
                else
                {
                    attacker=false;
                }
                receive_datagram.resize(udpSocketGet.pendingDatagramSize());
                udpSocketGet.readDatagram(receive_datagram.data(),receive_datagram.size(),&server,&serverPort);
                emit lblChanged( "[INFO] GAME DATA: "+ QString::fromUtf8(receive_datagram.data())+"\n");
                spel = receive_datagram.toInt();
                emit lblChanged(" SPEL:" + QString::number(spel) + "\n");
                emit lblChanged("[INFO] Found STRATEGO-SERVER on " + server.toString() + "\n");
                return server.toString();
            }
        }
        else
        {
            emit lblChanged("[INFO] UDP Discover TimeOut!\n");
            static int timeout=0;
            timeout++;
            if(timeout==5)
            {
                emit lblChanged("[ERROR] Server is not online. Please try again later!");
                return "";
            }
        }
    }
    return "";
}
void connection::write_data(QByteArray *buffer){
    socket->write(*buffer);
    socket->flush();
}
void connection::connectToServer(){
    emit lblChanged("[INFO] Initiating connection procedure.\n");
    socket->connectToHost(Broadcast(), 41000);
    emit lblChanged("Connected!");
}
void connection::on_connected(){
    //----------
    emit lblChanged("[INFO] Authenticating to STRATEGO-SERVER!\n");
    ableToWrite = true;
}
void connection::on_disconnected(){
    emit lblChanged("[ERROR] Disconnected with TCP Server!\n");
    ableToWrite = false;
}
void connection::incommingData(){
    QString input = socket->readAll();
    QStringList list = input.split("/");
    if(list.at(0) == "START")
    {
        opponent = list.at(1);
        gameStatus=true;
        emit playfieldChanged();
    }
    else if(list.at(0) == "STOP")
    {
        gameStatus=false;
        QDialog *dialog = new QDialog();
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *label = new QLabel();
        layout->addWidget(label);
        label->setText("Opponent stopped, you won!");
        dialog->show();
        socket->close();
    }
    if(list.at(0)=="WINNER")
    {
        emit lblChanged("You won!\n");
        spelveld[x][y]=15;
        spelveld[x2][y2]=list.at(1).toInt();
        socket->close();
    }
    else if(list.at(0)=="LOSER")
    {
        spelveld[defx2][defy2]=15;
        spelveld[defx][defy]=20;
        emit lblChanged("You lost!\n");
        socket->close();
    }
    else if(list.at(0)=="DRAW")
    {
        emit lblChanged("It was a draw against " + list.at(1)+"\n");

        if(attacker==true)
        {
            spelveld[x][y]=15;
            spelveld[x2][y2]=15;
            attacker=false;
        }
        else
        {
            spelveld[defx2][defy2]=15;
            spelveld[defx][defy]=15;
            attacker=true;
        }
        emit playfieldChanged();
    }
    else if(list.at(0)=="WON")
    {
        emit lblChanged("It was a win against " + list.at(1)+"\n");

        if(attacker==true)
        {
            spelveld[x][y]=15;
            spelveld[x2][y2]=list.at(1).toInt();
            attacker=false;
        }
        else
        {
            spelveld[defx2][defy2]=15;
            attacker=true;
        }
        emit playfieldChanged();
    }
    else if(list.at(0)=="LOST")
    {
        emit lblChanged("It was a defeat against " + list.at(1)+"\n");

        if(attacker==true)
        {
            spelveld[x][y]=15;
            attacker=false;
        }
        else
        {
            spelveld[defx2][defy2]=15;
            spelveld[defx][defy]=20;
            attacker=true;
        }
        emit playfieldChanged();
    }
    else if(list.at(0)=="MOVE")
    {
        if(attacker==true)
        {
            spelveld[x][y]=15;
            spelveld[x2][y2]=list.at(1).toInt();
            attacker=false;
        }
        else
        {
            spelveld[defx2][defy2]=15;
            spelveld[defx][defy]=20;
            attacker=true;
        }
        emit playfieldChanged();
    }
    else if(list.at(0)=="POWN")
    {
        defx=list.at(1).toInt();
        defy=list.at(2).toInt();
        write_data(new QByteArray("AT/" + QByteArray::number(spelveld[defx][defy])));
    }
    else if(list.at(0)=="FROM")
    {
        defx2=list.at(1).toInt();
        defy2=list.at(2).toInt();
    }
}
