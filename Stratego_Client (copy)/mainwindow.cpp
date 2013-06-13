#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QVBoxLayout"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    play=0;
    from=true;
    ui->setupUi(this);
    Hlayout = new QHBoxLayout(this->centralWidget());
    layout = new QVBoxLayout();
    Hlayout->addLayout(layout);
    view = new QGraphicsView(this);
    scene = new MyGraphicsScene(view);
    connect(scene,SIGNAL(mouseclicked(double,double)),this,SLOT(pownClicked(double,double)));
    QPixmap pixmap = QPixmap::fromImage(QImage(":/Classicboard.jpg"));
    QGraphicsPixmapItem *item =new QGraphicsPixmapItem(pixmap);
    item->setPixmap(pixmap);
    item->setScale(2);
    item->setPos(0,0);
    scene->addItem(item);
    QMenuBar *bar = new QMenuBar(view);
    connect(bar->addAction("Connect"),SIGNAL(triggered()),this,SLOT(Play()));
    connect(bar->addAction("Start"),SIGNAL(triggered()),this,SLOT(Start()));
    connect(bar->addAction("Stop"),SIGNAL(triggered()),this,SLOT(Stop()));
    this->setWindowTitle("Stratego");
    scene->setSceneRect(0,0,570,570);
    //view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    view->setScene(scene);
    this->setMinimumSize(700,700);
    //this->setMaximumSize(700,700);
    layout->addWidget(view);
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onMsgChanged(QString info)
{
    ChatMessages->setText(ChatMessages->text() + info);
    qDebug() << "onmsgchanged" << info;
}

void MainWindow::Play() {
    qDebugdfs("test new connection");
    c = new connection(this);

    qDebug("testadden");
    SA = new QScrollArea();
    label = new QLabel(SA);
    Chat = new QScrollArea;
    ChatMessages = new QLabel(Chat);
    Commit = new QPushButton;
    ChatInput = new QTextEdit;
    connect(Commit,SIGNAL(clicked()),c,SLOT(sendChatData());

    qDebug("TestLayout");
    layout->addWidget(SA,Qt::AlignBottom);
    layout2 = new QVBoxLayout();
    Hlayout->addLayout(layout2,Qt::AlignRight);
    layout2->addWidget(Chat,Qt::AlignRight);
    layout2->addWidget(ChatInput,Qt::AlignRight);
    c->Commit->setText("Send");
    layout2->addWidget(Commit,Qt::AlignRight);
    connect(c,SIGNAL(msgChanged(QString)),this,SLOT(onMsgChanged(QString)));
    c->connectToServer();
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
        {
            if(j<4)
            {
                c->spelveld[i][j]=20;
            }
            else
            {
                c->spelveld[i][j]=15;
            }
        }
    }
    connect(c,SIGNAL(playfieldChanged()),this,SLOT(redrawField()));
    connect(this,SIGNAL(send(QByteArray*)),c,SLOT(write_data(QByteArray*)));
    Bom=6;
    Maarschalk=1;
    Generaal=1;
    Kolonel=2;
    Majoor=3;
    Kapitein=4;
    Luitenant=4;
    Sergeant=4;
    Mineur=5;
    Verkenner=8;
    Spion=1;
    Vaandel=1;
}
void MainWindow::pownClicked(double x , double y)
{
    posx = int(x/58);
    posy = int(y/58);
    if(x>=0 && x<=575 && y>=0 && y<= 570)
    {
        if(play==0)
        {
            if(c->spelveld[int(posx)][int(posy)] == 15 && posy>5)
            {
                addPown(x,y);
            }
        }
        if(play==1 && c->attacker==true)
        {
            if(from==true){
                c->x=int(posx);
                c->y=int(posy);
                if(c->spelveld[int(posx)][int(posy)] !=20 && c->spelveld[int(posx)][int(posy)]!=15 && c->spelveld[int(posx)][int(posy)] != 11 && c->spelveld[int(posx)][int(posy)] != 0){
                    emit(send(new QByteArray("FROM/" + QByteArray::number(int(posx)) + "/" + QByteArray::number(int(posy)) + "/" + QByteArray::number(c->spelveld[int(posx)][int(posy)]))));
                    from = false;
                }
            }
            else
            {
                c->x2=int(posx);
                c->y2=int(posy);
                bool move = isMoveValid();
                if(move && (c->spelveld[int(posx)][int(posy)]==20 || c->spelveld[int(posx)][int(posy)]==15))
                {
                    from=true;
                    emit(send(new QByteArray("TO/" + QByteArray::number(int(posx)) + "/" + QByteArray::number(int(posy)))));
                }
                else
                {
                    from=true;
                    c->Label->setText(c->Label->text() + "Invalid move!\n");
                }
            }
        }
    }
}
bool MainWindow::isMoveValid()
{
    if(c->spelveld[int(c->x)][int(c->y)] !=2)
    {
        if((c->x-c->x2)==1 || (c->x-c->x2)==-1)
        {
            if((c->y-c->y2)==1)
            {
                return false;
            }
            if((c->y-c->y2)==-1)
            {
                return false;
            }
            if((c->y-c->y2)==0)
            {
                return true;
            }
        }
        if((c->x-c->x2)==0)
        {
            if((c->y-c->y2)==1)
            {
                return true;
            }
            if((c->y-c->y2)==-1)
            {
                return true;
            }
            if((c->y-c->y2)==0)
            {
                return false;
            }
        }
    }
    else
    {
        if((c->x-c->x2)==0)
        {
            int dif = c->y-c->y2;
            if(dif>0)
            {
                for(int i=1;i<dif;i++)
                {
                    if(c->spelveld[c->x][c->y+i]!=15)
                    {
                        return false;
                    }
                }
                return true;
            }
            if(dif<0)
            {
                for(int i=1;i<=dif;i++)
                {
                    if(c->spelveld[c->x][c->y-i]!=15)
                    {
                        return false;
                    }
                }
                return true;
            }
        }
        if((c->y-c->y2)==0)
        {
            int dif = c->x-c->x2;
            if(dif>0)
            {
                for(int i=1;i<dif;i++)
                {
                    if(c->spelveld[c->x+i][c->y]!=15)
                    {
                        return false;
                    }
                }
                return true;
             }
            if(dif<0)
            {
                for(int i=1;i<dif;i++)
                {
                    if(c->spelveld[c->x-i][c->y]!=15)
                    {
                        return false;
                    }
                }
                return true;
            }
        }
    }
    return false;
}
void MainWindow::Start()
{
    c->Label->setText(c->Label->text() + "Game Started/n");
    if(play == 0 && Bom==0 && Maarschalk==0 && Generaal==0 && Kolonel==0 && Majoor==0 && Kapitein==0 && Luitenant==0 && Sergeant==0 && Mineur==0 && Verkenner==0 && Spion==0 && Vaandel==0)
    {
        play = 1;
        emit send(new QByteArray("START"));
    }
    play = 1;
    emit send(new QByteArray("START"));
}
void MainWindow::Stop()
{
    c->Label->setText(c->Label->text() +"Game Stopped\n");
    if(play==1)
    {
        play = 0;
        emit send(new QByteArray("STOP"));
    }
}

void MainWindow::addPown(double x, double y) {
    posx = x;
    posy = y;
    QDialog *dialog = new QDialog();
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    posx = int(posx/58);
    posy = int(posy/58);
    if(Bom>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Bom");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawBom()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Maarschalk>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Maarschalk");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawMaarschalk()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Generaal>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Generaal");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawGeneraal()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Kolonel>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Kolonel");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawKolonel()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Majoor>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Majoor");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawMajoor()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Kapitein>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Kapitein");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawKapitein()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Luitenant>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Luitenant");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawLuitenant()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Sergeant>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Sergeant");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawSergeant()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Mineur>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Mineur");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawMineur()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Verkenner>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Verkenner");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawVerkenner()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Spion>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Spion");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawSpion()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    if(Vaandel>=1)
    {
        QPushButton *pushb = new QPushButton(this);
        pushb->setText("Vaandel");
        layout->addWidget(pushb);
        connect(pushb,SIGNAL(clicked()),this,SLOT(drawVaandel()));
        connect(pushb,SIGNAL(clicked()),dialog,SLOT(close()));
        layout->update();
    }
    dialog->show();

}
void MainWindow::drawBom()
{
    Bom--;
    c->spelveld[int(posx)][int(posy)] = 11;
    x=posx;
    y=posy;
    type=11;
    draw();
}
void MainWindow::drawMaarschalk()
{
    Maarschalk--;
    c->spelveld[int(posx)][int(posy)] = 10;
    x=posx;
    y=posy;
    type=10;
    draw();
}
void MainWindow::drawGeneraal()
{
    Generaal--;
    c->spelveld[int(posx)][int(posy)] = 9;
    x=posx;
    y=posy;
    type=9;
    draw();
}
void MainWindow::drawKolonel()
{
    Kolonel--;
    c->spelveld[int(posx)][int(posy)] = 8;
    x=posx;
    y=posy;
    type=8;
    draw();
}
void MainWindow::drawMajoor()
{
    Majoor--;
    c->spelveld[int(posx)][int(posy)] = 7;
    x=posx;
    y=posy;
    type=7;
    draw();
}
void MainWindow::drawKapitein()
{
    Kapitein--;
    c->spelveld[int(posx)][int(posy)] = 6;
    x=posx;
    y=posy;
    type=6;
    draw();
}
void MainWindow::drawLuitenant()
{
    Luitenant--;
    c->spelveld[int(posx)][int(posy)] = 5;
    x=posx;
    y=posy;
    type=5;
    draw();
}
void MainWindow::drawSergeant()
{
    Sergeant--;
    c->spelveld[int(posx)][int(posy)] = 4;
    x=posx;
    y=posy;
    type=4;
    draw();
}
void MainWindow::drawMineur()
{
    Mineur--;
    c->spelveld[int(posx)][int(posy)] = 3;
    x=posx;
    y=posy;
    type=3;
    draw();
}
void MainWindow::drawVerkenner()
{
    Verkenner--;
    c->spelveld[int(posx)][int(posy)] = 2;
    x=posx;
    y=posy;
    type=2;
    draw();
}
void MainWindow::drawSpion()
{
    Spion--;
    c->spelveld[int(posx)][int(posy)] = 1;
    x=posx;
    y=posy;
    type=1;
    draw();
}
void MainWindow::drawVaandel()
{
    Vaandel--;
    c->spelveld[int(posx)][int(posy)] = 0;
    x=posx;
    y=posy;
    type=0;
    draw();
}
void MainWindow::redrawField()
{
    scene->clear();
    QPixmap pixmap = QPixmap::fromImage(QImage(":/Classicboard.jpg"));
    QGraphicsPixmapItem *item =new QGraphicsPixmapItem(pixmap);
    item->setPixmap(pixmap);
    item->setScale(2);
    item->setPos(0,0);
    scene->addItem(item);
    for(x=0;x<10;x++)
    {
        for(y=0;y<10;y++)
        {
            type = c->spelveld[x][y];
            draw();
        }
    }
}
void MainWindow::draw()
{
    if(type!=15)
    {
        int xpos = x*58;
        int ypos = y*58;
        QPixmap pixmap = QPixmap::fromImage(QImage(":/" + QString::number(type) + ".jpg"));
        QGraphicsPixmapItem *item =new QGraphicsPixmapItem(pixmap);
        item->setScale(2.5);
        item->setPixmap(pixmap);
        item->setPos(xpos,ypos);
        scene->addItem(item);
        view->setScene(scene);
        view->update();
    }
}

