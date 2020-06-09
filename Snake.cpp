#include <QPainter>
#include <QTime>
#include <QDebug>
#include <QtCore/qmath.h>
#include <QMediaPlayer>
#include "Snake.h"

Bullet::Bullet(int x,int y){
    this->x = x;
    this->y = y;
    bulletImg.load(":/images/images/bullet.png");
    /*载入图片，但是这个载入的图片为止再哪里是怎么确定的呢？
     * 我知道x，y可能是它的坐标，但是如何把x，y和这个图片联系起来的呢？*/
}
void Bullet::move(){
    //没使用方向，请自行修改。
    x = x - (int) (qCos(direction) * 20);
    y = y - (int) (qSin(direction) * 20);
    /*这个坐标系的数据怎么看？
     * 原点在哪？
     * 坐标轴在哪？
     * 单位1有多长？*/
}

Snake::Snake(QWidget *parent) : QWidget(parent) {
    setStyleSheet("background-color:white;");
    leftDirection = false;
    rightDirection = true;
    upDirection = false;
    downDirection = false;
    inGame = true;

    bcount =0;

    resize(B_WIDTH, B_HEIGHT);//设置窗口大小
    loadImages();
    initGame();
}

void Snake::loadImages() {

    dot.load(":/images/images/dot.png");
    head.load(":/images/images/head.png");
    apple.load(":/images/images/apple.png");
    button.load(":/images/images/button.png");
    buttonCover.load(":/images/images/buttonCover.png");
}

void Snake::initGame() {

    dots = 3;

    for (int z = 0; z < dots; z++) {
        x[z] = 50 - z * 10;
        y[z] = 50;
    }

    locateApple();
    locateButton();
    timerId = startTimer(DELAY);
    /*调用 startTimer启动一个定时器，并返回定时器id。（每次返回的id是不同的吗？如果是，每次返回值有什么规律呢）如果启动失败，返回0.
     * 定时器每隔interval 毫秒就会启动一次，直到调用killTimer(). 如果interval=0,当没有其他系统时间发生时，会调用一次。
     * 当定时器发生时，会调用timerEvent(QTimerEvent *event).如果多个定时器在运行，可用通过timeId()来区分。*/

    QMediaPlayer * player = new QMediaPlayer;//多媒体，用于设置进入游戏是的视频，音效
    player->setMedia(QUrl("qrc:/sounds/sounds/hdk.mp3"));
    player->setVolume(30);
    player->play();

}

void Snake::locateButton(){
    //这里应该是用来确定button位置，但是还没能做到显示
    mvButton_x = 390;
    mvButton_y = 390;

    contrlButton_x =350;
    contrlButton_y =350;

}


void Snake::paintEvent(QPaintEvent *e) {
    //这是个系统自动调用的函数

    Q_UNUSED(e);//说明程序中有特殊的变量名，避免重复警告

    doDrawing();
}

void Snake::doDrawing() {

    //用来画图，但这个函数应该只能画一次，所以需要定时器启动？

    QPainter qp(this);//QPainter只能在类对象的paintEvent()函数中绘制图形，所以这一步指名类对象

    if (inGame) {
        qp.drawImage(apple_x, apple_y, apple);//成功做到显示苹果
        qp.drawImage(contrlButton_x, contrlButton_y, button);

        for(int i=0; i<100;i++)
            qp.drawImage(bt[i].x, bt[i].y, bt[i].bulletImg);

        if(buttonCoverFlag==true)
            qp.drawImage(contrlButton_x, contrlButton_y, buttonCover);


        for (int z = 0; z < dots; z++) {
            if (z == 0) {
                qp.drawImage(x[z], y[z], head);
            } else {
                qp.drawImage(x[z], y[z], dot);
            }
        }

    } else {

        gameOver(qp);
    }
}

void Snake::gameOver(QPainter &qp) {
    //应该是用来画游戏结束窗口的

    QString message = "Game over";//画文字
    QFont font("Courier", 15, QFont::DemiBold);//设置字体
    QFontMetrics fm(font);//获取字体宽度大小
    int textWidth = fm.width(message);

    qp.setFont(font);
    int h = height();
    int w = width();

    qp.translate(QPoint(w/2, h/2));
    qp.drawText(-textWidth/2, 0, message);
}

void Snake::checkApple() {

    if ((x[0] == apple_x) && (y[0] == apple_y)) {

        dots++;
        locateApple();
    }
}

void Snake::move() {

    for (int z = dots; z > 0; z--) {
        x[z] = x[(z - 1)];
        y[z] = y[(z - 1)];
    }

    if (leftDirection) {
        x[0] -= DOT_SIZE;
    }

    if (rightDirection) {
        x[0] += DOT_SIZE;
    }

    if (upDirection) {
        y[0] -= DOT_SIZE;
    }

    if (downDirection) {
        y[0] += DOT_SIZE;
    }
}

void Snake::checkCollision() {

    for (int z = dots; z > 0; z--) {

        if ((z > 4) && (x[0] == x[z]) && (y[0] == y[z])) {
            inGame = false;
        }
    }

    if (y[0] >= B_HEIGHT) {
        inGame = false;
    }

    if (y[0] < 0) {
        inGame = false;
    }

    if (x[0] >= B_WIDTH) {
        inGame = false;
    }

    if (x[0] < 0) {
        inGame = false;
    }

    if(!inGame) {
        killTimer(timerId);
        killTimer(timerId2);
    }
}

void Snake::locateApple() {

    QTime time = QTime::currentTime();
    qsrand((uint) time.msec());

    int r = qrand() % RAND_POS;
    apple_x = (r * DOT_SIZE);

    r = qrand() % RAND_POS;
    apple_y = (r * DOT_SIZE);

    printf("%d %d", apple_x, apple_y);//输出了个寂寞？？
}

void Snake::timerEvent(QTimerEvent *e) {

    Q_UNUSED(e);
    int id = e->timerId();
    if (inGame) {
        if(id == timerId){
        checkApple();
        checkCollision();
        move();

        }
        else if(id==timerId2){
            if(buttonMove==false){
               //对子弹的初始和设定，并更新移动
               bt[bcount].x = mvButton_x;
               bt[bcount].y = mvButton_y;
               bt[bcount].direction=qAtan((double)(mvButton_y-y[0])/(mvButton_x-x[0]));
               bcount++;
               if(bcount>=20) bcount = 0;
            }
           for(int i=0; i<10; i++)
                bt[i].move();

           repaint();
        }

    }

    repaint();
}

void Snake::keyPressEvent(QKeyEvent *e) {

    int key = e->key();

    if ((key == Qt::Key_Left) && (!rightDirection)) {
        leftDirection = true;
        upDirection = false;
        downDirection = false;
    }

    if ((key == Qt::Key_Right) && (!leftDirection)) {
        rightDirection = true;
        upDirection = false;
        downDirection = false;
    }

    if ((key == Qt::Key_Up) && (!downDirection)) {
        upDirection = true;
        rightDirection = false;
        leftDirection = false;
    }

    if ((key == Qt::Key_Down) && (!upDirection)) {
        downDirection = true;
        rightDirection = false;
        leftDirection = false;
    }

}

void Snake::mousePressEvent(QMouseEvent *event)
{
    int mx = event->x();
    int my = event->y();


    //设置按钮变化，开启第二个计时器，
    if ((mx<400) && (mx >=350) && (my<400) &&(my>=350)){
        buttonMove = true;
        mvButton_x = mx;
        mvButton_y = my;
        timerId2 = startTimer(10); //启动第二个时间控制器
        buttonCoverFlag = true; //按钮覆盖的标示
    }
    else{
        buttonMove = false;
    }

    if ((my < y[0]) && (rightDirection)) {
        leftDirection = false;
        upDirection = true;
        downDirection = false;
        rightDirection = false;
    }

    else if ((my > y[0]) && (rightDirection)) {
        leftDirection = false;
        upDirection = false;
        downDirection = true;
        rightDirection = false;
    }

    else if ((my > y[0]) && (leftDirection)) {
        leftDirection = false;
        upDirection = false;
        downDirection = true;
        rightDirection = false;
    }

    else if ((my < y[0]) && (leftDirection)) {
        leftDirection = false;
        upDirection = true;
        downDirection = false;
        rightDirection = false;
    }

    else if ((mx < x[0]) && (downDirection)) {
        leftDirection = true;
        upDirection = false;
        downDirection = false;
        rightDirection = false;
    }

    else if ((mx > x[0]) && (downDirection)) {
        leftDirection = false;
        upDirection = false;
        downDirection = false;
        rightDirection = true;
    }

    else if ((mx < x[0]) && (upDirection)) {
        leftDirection = true;
        upDirection = false;
        downDirection = false;
        rightDirection = false;
    }

    else  {
        leftDirection = false;
        upDirection = false;
        downDirection = false;
        rightDirection = true;
    }


}

void Snake::mouseMoveEvent(QMouseEvent *event){
    int mx = event->x();
    int my = event->y();

    if(buttonMove==true){
        mvButton_x = mx-5;
        mvButton_y = my-5;
        qDebug()<<mx<<my;
    }
}

void Snake::mouseReleaseEvent(QMouseEvent *event){
    if(buttonMove==true){
        buttonMove =false;
    }
    int mx = event->x();
    int my = event->y();

//    //此处可以更改优化成类的封装方法
//    bt[bcount].x = mx;
//    bt[bcount].y = my;
//    bt[bcount].direction=qAtan((double)(my-y[0])/(mx-x[0]));
//    qDebug()<<bt[bcount].direction;
//    bcount++;
//    if(bcount>=10) bcount = 0;

    buttonCoverFlag = false;

}

