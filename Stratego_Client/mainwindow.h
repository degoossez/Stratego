#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostInfo>
#include <cstdio>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QLabel>
#include <mygraphicsscene.h>
#include "QDialog"
#include "QPushButton"
#include "connection.h"

QT_BEGIN_NAMESPACE
class QUdpSocket;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void drawForeground(QPainter* painter, const QRectF& rect);
    QVBoxLayout *layout;
    QGraphicsView *view;
    MyGraphicsScene *scene;
    double posx,posy;
    int x,y;
    int Bom,Maarschalk,Generaal,Kolonel,Majoor,Kapitein,Luitenant,Sergeant,Mineur,Verkenner,Spion,Vaandel;
    connection *c;
private:
    Ui::MainWindow *ui;
private slots:
    void Play();
    void addPown(double,double);
    void drawBom();
    void drawMaarschalk();
    void drawGeneraal();
    void drawKolonel();
    void drawMajoor();
    void drawKapitein();
    void drawLuitenant();
    void drawSergeant();
    void drawMineur();
    void drawVerkenner();
    void drawSpion();
    void drawVaandel();
    void redrawField(int[9][9]);
    void draw(int type);

};

#endif // MAINWINDOW_H
