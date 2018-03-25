#include "tablewidget.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDebug>
#include "widget.h"

TableWidget::TableWidget(QWidget * parent) : QTableWidget(parent)
{
    srand(time(0));
    setColumnCount(10);
    setRowCount(10);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    verticalHeader()->setDefaultSectionSize(50);
    horizontalHeader()->setDefaultSectionSize(50);
    verticalHeader()->hide();
    horizontalHeader()->hide();

    move(0, 50);
    setFixedSize(500, 500);

    setIconSize(QSize(45, 45));

    down = new QTimer;
    connect(down, &QTimer::timeout, this, &TableWidget::DivagateDown);

    removal = new QTimer;
    connect(removal, &QTimer::timeout, this, &TableWidget::Removal);

    Start();

    sm = new QSharedMemory("3InRow");
    sm->create(1);
    shTimer = new QTimer;
    connect(shTimer, &QTimer::timeout, this, &TableWidget::Memory);
    shTimer->start(1000);

}

void TableWidget::mousePressEvent(QMouseEvent *event)
{
    QTableWidgetItem *targetItem=itemAt(event->pos());
    if(OldPosX == -1 && OldPosY == -1)
    {
        if (event->button() == Qt::LeftButton) {
            if (targetItem!=0) {
                OldPosX = targetItem->row();
                OldPosY = targetItem->column();
                //qDebug() << QString::number(OldPosX)  << QString::number(OldPosY);
            }
        }
    }
    else
    {
        if (event->button() == Qt::LeftButton) {
            if (targetItem!=0) {

                if(OldPosX - targetItem->row() <= 1 && OldPosX - targetItem->row() >= -1
                        && OldPosY - targetItem->column() <= 1 && OldPosY - targetItem->column() >= -1)
                {
                    QIcon icon1 = targetItem->icon();
                    QIcon icon2 = item(OldPosX, OldPosY)->icon();
                    QString name1 = targetItem->text();
                    QString name2 = item(OldPosX, OldPosY)->text();

                    targetItem->setIcon(icon2);
                    item(OldPosX, OldPosY)->setIcon(icon1);

                    targetItem->setText(name2);
                    item(OldPosX, OldPosY)->setText(name1);
                }

                Delete();

                OldPosX = -1;
                OldPosY = -1;
            }
        }
    }

    QTableWidget::mousePressEvent(event);
}

void TableWidget::Start()
{
    int random;
    int r, b, y = 0;
    bool yes;
    for(int i = 0; i != 10; i++)
    {
        for(int j = 0; j != 10; j++)
        {
            QTableWidgetItem* item = new QTableWidgetItem;
            yes = false;
            do
            {
                random = rand()%3+1;
                switch(random)
                {
                case 1:
                    if(b < 2)
                    {
                        if(i == 0 || (i != 0 && this->item(i - 1, j)->text() != "blue"))
                        {
                            item->setIcon(QPixmap(":/images/blue.png"));
                            item->setText("blue");
                            b++;
                            r = y = 0;
                            yes = true;
                        }
                    }
                    break;
                case 2:
                    if(r < 2)
                    {
                        if(i == 0 || (i != 0 && this->item(i - 1, j)->text() != "red"))
                        {
                            item->setIcon(QPixmap(":/images/red.png"));
                            item->setText("red");
                            r++;
                            b = y = 0;
                            yes = true;
                        }
                    }
                    break;
                case 3:
                    if(y < 2)
                    {
                        if(i == 0 || (i != 0 && this->item(i - 1, j)->text() != "yellow"))
                        {
                            item->setIcon(QPixmap(":/images/yellow.png"));
                            item->setText("yellow");
                            y++;
                            r = b = 0;
                            yes = true;
                        }
                    }
                    break;
                }
            }while(yes == false);
            setItem(i, j, item);
        }
    }
}

void TableWidget::Delete()
{
    int nextX, nextY;
    int NumberSame;
    Remove.clear();
    for(int i = 0; i != 10; i++)
    {
        for(int j = 0; j != 10; j++)
        {
            if(this->item(i, j)->text() != "")
            {
                // удаление по столбцу
                NumberSame = 0;
                nextX = i + 1;

                while(nextX < 10 && this->item(i, j)->text() == this->item(nextX, j)->text())
                {
                    NumberSame++;
                    nextX++;
                }

                if(NumberSame >= 2)
                {
                    QString str;
                    for(int z = i; z <= i + NumberSame; z++)
                    {
                        str.append(QString::number(z,10));
                        str.append(QString::number(j,10));
                    }
                    Remove.append(str);

                    ChangedScore(NumberSame + 1);
                }

                // удаление по строке
                NumberSame = 0;
                nextY = j + 1;

                while(nextY < 10 && this->item(i, j)->text() == this->item(i, nextY)->text())
                {
                    NumberSame++;
                    nextY++;
                }

                if(NumberSame >= 2)
                {
                    QString str;
                    for(int z = j; z <= j + NumberSame; z++)
                    {
                        str.append(QString::number(i,10));
                        str.append(QString::number(z,10));
                    }
                    Remove.append(str);

                    ChangedScore(NumberSame + 1);
                }
            }
        }
    }
    removal->start(200);
}

bool TableWidget::Update()
{
    bool UpdateDown = true;
    bool Down = false;
    bool column = false;
    for(int i = 9; i > -1; i--)
    {
        for(int j = 9; j > -1; j--)
        {
            column = false;
            foreach( int value, col)
            {
                if(value == j)
                {
                    column = true;
                    break;
                }
            }
            // смещ€ем все элементы в низ, вместо пустых клеток
            if(this->item(i, j)->text() == "" && column == false)
            {
                for(int z = i; z != -1;)
                {
                    // проверка есть ли элемент(шар) выше пустого
                    if(this->item(z, j)->text() == "")
                    {
                        z--;
                    }
                    else
                    {
                        Down = true;
                        g_i = z;
                        z = -1;
                    }
                }
                if(Down == true)
                {
                    UpdateDown = false;

                    g_j = j;

                    down->start(20);

                    i = -1;
                    j = -1;
                }
                else
                {
                    col << j;
                }
            }
        }
    }
    if(UpdateDown == true)
    {
        col.clear();
    }
    return UpdateDown;
}

void TableWidget::CreateBalls()
{
    bool deleted = false;
    int random;
    bool spawn = false;
    for(int i = 9; i != -1; i--)
    {
        for(int j = 9; j != -1; j--)
        {
            // замен€ем пустые клетки на шарики
            if(this->item(i, j)->text() == "")
            {
                QTableWidgetItem* item = new QTableWidgetItem;
                while(spawn == false)
                {
                    random = rand()%3+1;
                    switch(random)
                    {
                    case 1:
                        if(((j == 0) || (this->item(i, j - 1)->text() != "blue"))
                            && ((j == 9) || (this->item(i, j + 1)->text() != "blue")))
                        {
                            item->setIcon(QPixmap(":/images/blue.png"));
                            item->setText("blue");
                            spawn = true;
                        }
                        break;
                    case 2:
                        if(((j == 0) || (this->item(i, j - 1)->text() != "red"))
                            && ((j == 9) || (this->item(i, j + 1)->text() != "red")))
                        {
                            item->setIcon(QPixmap(":/images/red.png"));
                            item->setText("red");
                            spawn = true;
                        }
                        break;
                    case 3:
                        if(((j == 0) || (this->item(i, j - 1)->text() != "yellow"))
                            && ((j == 9) || (this->item(i, j + 1)->text() != "yellow")))
                        {
                            item->setIcon(QPixmap(":/images/yellow.png"));
                            item->setText("yellow");
                            spawn = true;
                        }
                        break;
                    }
                }
                setItem(i, j, item);
                deleted = true;
                spawn = false;
            }
        }
    }
    if(deleted == true)
    {
        Delete();
    }
}

void TableWidget::DivagateDown()
{
    QIcon icon1 = item(g_i, g_j)->icon();
    QIcon icon2 = item(g_i + 1, g_j)->icon();
    QString name1 = item(g_i, g_j)->text();
    QString name2 = item(g_i + 1, g_j)->text();

    item(g_i, g_j)->setIcon(icon2);
    item(g_i + 1, g_j)->setIcon(icon1);

    item(g_i, g_j)->setText(name2);
    item(g_i + 1, g_j)->setText(name1);

    // проверка на то что ниже нет пустого элемента
    if(g_i + 1 == 10 || this->item(g_i + 1, g_j)->text() != "")
    {
        down->stop();

        if(Update() == true)
        {
            Delete();
        }
    }
    else
    {
        g_i++;
    }
}

void TableWidget::Removal()
{
    foreach (QString it, Remove)
    {
        for(int i = 0; i < it.size(); i+=2)
        {
            this->item(it.at(i).digitValue(), it.at(i+1).digitValue())->setText("");
            this->item(it.at(i).digitValue(), it.at(i+1).digitValue())->setIcon(QPixmap(""));
        }
        Remove.removeOne(it);
        break;
    }

    if(Remove.isEmpty())
    {
        if(Update())
        {
            CreateBalls();
        }
    }
}

void TableWidget::Memory()
{
    sm->lock();
    qDebug() << QString::number(sm->size());
    sm->unlock();
}
