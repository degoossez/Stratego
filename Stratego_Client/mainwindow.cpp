#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connection.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QVBoxLayout *layout = new QVBoxLayout(this->centralWidget());
    QGraphicsView *view = new QGraphicsView(this);
    QGraphicsScene *scene = new QGraphicsScene(this);

    QPixmap pixmap = QPixmap::fromImage(QImage("/home/dries/School/computernetwerken/Stratego/Classic_board.jpg"));
    QGraphicsPixmapItem *item =new QGraphicsPixmapItem(pixmap);
    item->setPixmap(pixmap);
    item->setScale(2);
    item->setPos(0,0);
    QGraphicsEllipseItem *itemElipse;
    itemElipse = new QGraphicsEllipseItem(0,0,60,60,item);
    itemElipse->setBrush(Qt::yellow);
    scene->addItem(item);
    QMenuBar *bar = new QMenuBar(view);
    QObject::connect(bar->addMenu("Start")->addAction("Connection"),SIGNAL(triggered()),this,SLOT(Play()));
    view->setWindowTitle("Stratego");
    layout->addWidget(view);
    view->setScene(scene);

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::Play() {
    connection c;
    c.connectToServer();
    if(c.socket->state() == QAbstractSocket::ConnectedState){
        QByteArray BA = "TEST WRITEN";
        c.socket->write(BA);
        c.socket->flush();
    }

}


