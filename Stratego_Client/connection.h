#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>

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
public slots:
private slots:
    void on_connected();
private:
    QString Broadcast();
    
};

#endif // CONNECT_H
