#include "connection.h"


#define udpgetport 6666

connection::connection(QObject *parent) :
    QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(printError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(socket,SIGNAL(disconnected()), this, SLOT(on_disconnected()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(incommingData()));

}
void connection::printError(QAbstractSocket::SocketError Error)
{
    qDebug() << Error;
}

QString connection::Broadcast() {
    // Om ip adressen te ontdekken
    QNetworkAddressEntry inter;
    // sockets aanmaken en verbinden met enerzijds broadcast en anderzijds een luister poort
    QUdpSocket udpSocketSend;
    QUdpSocket udpSocketGet;
    udpSocketSend.connectToHost(inter.broadcast(), 40000);
    // udpSocketGet->bind(inter->ip(),667);
    // udpSocketGet->bind(QHostAddress::Any,667)
    if(udpSocketGet.bind(udpgetport,QUdpSocket::ShareAddress))
        qDebug("[INFO] Could properly bind udpSocketget to %d\n",udpgetport);
    else qDebug("[INFO] Couldn't properly bind udpSocketget to %d\n",udpgetport);
    // Pakket verzenden
    QByteArray send_datagram = "DISCOVER-STRATEGO-SERVER";
    // Optimalisatie voor in de loop
    QByteArray receive_datagram;
    QHostAddress server;
    quint16 serverPort;
    forever{
        udpSocketSend.writeDatagram(send_datagram, QHostAddress::Broadcast, 40000);
        if(udpSocketGet.waitForReadyRead(3000)){
            receive_datagram.resize(udpSocketGet.pendingDatagramSize());
            udpSocketGet.readDatagram(receive_datagram.data(),receive_datagram.size(),&server,&serverPort);
            qDebug("[INFO] udpSocketGet DATA: \'%s\'\n",receive_datagram.data());
            if(QString::fromUtf8(receive_datagram.data()) == "DISCOVERED-STRATEGO-SERVER")
            {
                receive_datagram.resize(udpSocketGet.pendingDatagramSize());
                udpSocketGet.readDatagram(receive_datagram.data(),receive_datagram.size(),&server,&serverPort);
                qDebug("[INFO] PLAYER DATA: \'%s\'\n",receive_datagram.data());
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
                qDebug("[INFO] GAME DATA: \'%s\'\n",receive_datagram.data());
                spel = receive_datagram.toInt();
                qDebug() << "SPEL:" << spel;
                qDebug("[INFO] Found STRATEGO-SERVER on %s\n",server.toString().toUtf8().constData());
                return server.toString();
            }
        }else qDebug("[INFO] UDP Discover TimeOut\n");
    }
    return "";
}
void connection::write_data(QByteArray *buffer){
    //buffer->rightJustified(56,' ',true);
    socket->write(*buffer);
    socket->flush();
}
void connection::connectToServer(){
    qDebug("[INFO] Initiating connection procedure\n");
    //socket->connectToHost("fenrig-N73SV", 666);
    //socket->connectToHost("192.168.10.124", 666);
    socket->connectToHost(Broadcast(), 41000);
    qDebug("Connected!");
}
void connection::on_connected(){
    //----------
    qDebug("[INFO] Authenticating to CAR-SERVER\n");
    // char auth_buffer[13] = "car_v1      ";
    /*char auth_buffer[13] = "car_v2      ";
    socket->write(auth_buffer);
    socket->flush(); */
    ableToWrite = true;
    //----------
    //hier een bool zetten op 1 endan checken of deze op 1 staat om de andere toegang te geven tot schrijven op socket
}
void connection::on_disconnected(){
    printf("[ERROR] Disconnected with TCP Server\n");
    ableToWrite = false;
}
void connection::incommingData(){
    QString input = socket->readAll();
    qDebug() << input;
    if(input == "START")
    {
        gameStatus=true;
        emit playfieldChanged();
    }
    else if(input == "STOP")
    {
        gameStatus=false;
        QDialog *dialog = new QDialog();
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *label = new QLabel();
        layout->addWidget(label);
        label->setText("Opponent stopped, you won!");
        dialog->show();
    }
    QStringList list = input.split("/");
    if(list.at(0)=="DRAW")
    {
        if(attacker==true)
        {
            qDebug("Attack==true");
            spelveld[x][y]=15;
            spelveld[x2][y2]=15;
            attacker=false;
        }
        else
        {
            qDebug("Attack==false");
            spelveld[defx2][defy2]=15;
            spelveld[defx][defy]=15;
            attacker=true;
        }
        emit playfieldChanged();
    }
    else if(list.at(0)=="WON")
    {
        if(attacker==true)
        {
            qDebug("Attack==true");
            spelveld[x][y]=15;
            spelveld[x2][y2]=list.at(1).toInt();
            attacker=false;
        }
        else //nog aanpassen
        {
            qDebug("Attack==false");
            spelveld[defx2][defy2]=15;
            attacker=true;
        }
        emit playfieldChanged();
    }
    else if(list.at(0)=="LOST")
    {
        if(attacker==true)
        {
            qDebug("Attack==true");
            spelveld[x][y]=15;
            attacker=false;
        }
        else
        {
            qDebug("Attack==false");
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
            qDebug("Attack==true");
            spelveld[x][y]=15;
            spelveld[x2][y2]=list.at(1).toInt();
            attacker=false;
        }
        else
        {
            qDebug("Attack==false");
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
        qDebug() << QByteArray("AT/" + QByteArray::number(spelveld[defx][defy]));
    }
    else if(list.at(0)=="FROM")
    {
        defx2=list.at(1).toInt();
        defy2=list.at(2).toInt();
    }
}
