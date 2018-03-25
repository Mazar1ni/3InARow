#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include <QMouseEvent>
#include <QTimer>
#include <QSharedMemory>

class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    TableWidget(QWidget * parent = 0);
    void mousePressEvent(QMouseEvent* event);
    void Start();
    void Delete();
    bool Update();
    void CreateBalls();
    void DivagateDown();
    void Removal();
    void Memory();

signals:
    void ChangedScore(int sc);

private:
    int OldPosX = -1;
    int OldPosY = -1;
    QTimer* down;
    QTimer* removal;
    int g_i;
    int g_j;
    QList<int> col;
    QList<QString> Remove;

    // memory
    QSharedMemory* sm;
    QTimer* shTimer;
};

#endif // TABLEWIDGET_H
