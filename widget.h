#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include "tablewidget.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    void Time();

public slots:
    void ChangedScore(int sc);

private:
    TableWidget* Table;
    QLabel* l_time;
    QLabel* l_score;
    QWidget end;

    int score = 0;
    int time = 60;
    QTimer* timer;
};

#endif // WIDGET_H
