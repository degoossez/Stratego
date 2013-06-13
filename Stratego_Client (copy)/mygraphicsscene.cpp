#include "mygraphicsscene.h"
#include "QDebug"
#include "mainwindow.h"

MyGraphicsScene::MyGraphicsScene(QObject *parent): QGraphicsScene(parent)
{

}

void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit mouseclicked(event->scenePos().x(),event->scenePos().y());
}
