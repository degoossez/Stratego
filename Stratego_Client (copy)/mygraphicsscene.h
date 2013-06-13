#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit MyGraphicsScene(QObject *parent =0);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
    void mouseclicked(double,double);

};

#endif // MYGRAPHICSSCENE_H
