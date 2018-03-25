#include "widget.h"
#include <QHeaderView>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    Table = new TableWidget(this);

    l_time = new QLabel(this);
    l_time->resize(75, 100);
    l_time->move(500, 150);
    l_time->setText("Time \n" + QString::number(time));
    l_time->setAlignment(Qt::AlignCenter);

    // נאחלונ רנטפעא
    QFont font = l_time->font();
    font.setPixelSize(20);
    l_time->setFont(font);

    l_score = new QLabel(this);
    l_score->resize(75, 100);
    l_score->move(500, 300);
    l_score->setText("Score \n" + QString::number(score));
    l_score->setAlignment(Qt::AlignCenter);

    // נאחלונ רנטפעא
    font = l_score->font();
    font.setPixelSize(20);
    l_score->setFont(font);

    setFixedSize(600, 600);

    connect(Table, &TableWidget::ChangedScore, this, &Widget::ChangedScore);

    timer = new QTimer;
    connect(timer, QTimer::timeout, this, &Widget::Time);
    timer->start(1000);
}

Widget::~Widget()
{

}

void Widget::Time()
{
    l_time->setText("Time \n" + QString::number(time));
    time--;
    if(time == 0)
    {
        timer->stop();
        end.show();
        this->setEnabled(false);
    }
}

void Widget::ChangedScore(int sc)
{
    score += sc;
    l_score->setText("Score \n" + QString::number(score));
}
