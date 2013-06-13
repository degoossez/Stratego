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
    qDebug("send1");
    QByteArray datagram = ChatInput->toPlainText().toUtf8();
    ChatInput->clear();
    qDebug("send1.1");
    send_datagram = datagram + "@@" + QByteArray::number(speler);
    qDebug("send1.2");
    qDebug() << send_datagram;
    udpSocketSend.writeDatagram(send_datagram, opponent, 6666);
    qDebug("send1.3");
}

void connection::chatData()
{
    QByteArray receive_datagram;
    quint16 serverPort;
    qDebug() << UDPsocket->pendingDatagramSize();
    receive_datagram.resize(UDPsocket->pendingDatagramSize());
    UDPsocket->readDatagram(receive_datagram.data(),receive_datagram.size(),&opponent,&serverPort);
    qDebug() << receive_datagram.data();
    QString input = QString::fromUtf8(receive_datagram.data());
    qDebug() << "input: " + input;
    QStringList list = input.split("@@");
    if(list.at(1)!=QString::number(speler))
    {
        qDebug("recieve6");
        QString info = "Openent: " + list.at(0) + "\n";
        emit msgChanged(info);
    }
}
void connection::printError(QAbstractSocket::SocketError Error)
{
    Label->setText(Label->text() + Error + "\n");
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
        Label->setText(Label->text() + "[INFO] Could properly bind udpSocketget to " + QString::number(udpgetport) + "\n");
    else Label->setText(Label->text() + "[INFO] Couldn't properly bind udpSocketget to " + QString::number(udpgetport) + "\n");
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
                Label->setText(Label->text() +"[INFO] PLAYER DATA: "+ receive_datagram.data() +"\n");
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
                Label->setText(Label->text() + "[INFO] GAME DATA: "+receive_datagram.data()+"\n");
                spel = receive_datagram.toInt();
                Label->setText(Label->text() + " SPEL:" + QString::number(spel) + "\n");
                Label->setText(Label->text() + "[INFO] Found STRATEGO-SERVER on " + server.toString().toUtf8().constData() + "\n");
                return server.toString();
            }
        }
        else
        {
            Label->setText(Label->text() + "[INFO] UDP Discover TimeOut!\n");
            static int timeout=0;
            timeout++;
            if(timeout==5)
            {
                Label->setText(Label->text() + "[ERROR] Server is not online. Please try again later!");
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
    Label->setText(Label->text() + "[INFO] Initiating connection procedure.\n");

    socket->connectToHost(Broadcast(), 41000);
    Label->setText(Label->text() +"Connected!");
}
void connection::on_connected(){
    //----------
    Label->setText(Label->text() + "[INFO] Authenticating to STRATEGO-SERVER!\n");
    ableToWrite = true;
}
void connection::on_disconnected(){
    Label->setText(Label->text() + "[ERROR] Disconnected with TCP Server!\n");
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
    }
    if(list.at(0)=="WINNER")
    {
        Label->setText(Label->text() + "You won!\n");
        spelveld[x][y]=15;
        spelveld[x2][y2]=list.at(1).toInt();
        socket->close();
    }
    else if(list.at(0)=="LOSER")
    {
        spelveld[defx2][defy2]=15;
        spelveld[defx][defy]=20;
        Label->setText(Label->text() + "You lost!\n");
        socket->close();
    }
    else if(list.at(0)=="DRAW")
    {
        Label->setText(Label->text() + "It was a draw against " + list.at(1)+"\n");

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
        Label->setText(Label->text() + "It was a win against " + list.at(1)+"\n");

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
         Label->setText(Label->text() + "It was a defeat against " + list.at(1)+"\n");

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
