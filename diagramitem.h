#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QTableWidget>
#include <QGraphicsPolygonItem>
#include <QList>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <diagramtextitem.h>
#include <QPainter>

class Arrow;
/*Клас для створення вершин графу.*/
class DiagramItem : public QGraphicsPolygonItem
{
public:
    /*Тип об'єкту.*/
    enum { Type = UserType + 15 };
    /*
        Конструктор класу.
        parent - вказівник на батьківський клас.
    */
    explicit DiagramItem(QGraphicsItem *parent = nullptr);
    /*
        Метод для видалення дуги, яка взаємодіює з вершиною.
        arrow - вказівник на об'єкт класу вершини.
    */
    void removeArrow(Arrow *arrow);
    /*Метод для видалення дуг, яка взаємодіють з вершиною.*/
    void removeArrows();
    /*Метод, який повертає вершину відповідної форми.*/
    QPolygonF polygon() const { return polygonObject; }
    /*
        Метод для додання нової дуги.
        arrow - вказівник на об'єкт класа дуги.
    */
    void addArrow(Arrow *arrow);
    /*Метод для формування зображення на кнопку, яка відповідає за побудову вершини.*/
    QPixmap image() const;
    /*Метод для отримання типу об'єкта.*/
    int type() const override { return Type; }
    /*Метод для отримання імені об'єкта.*/
    QString name() { return nameObject; }

protected:
    /*
        Метод для повідомлення користувацьких елементів про те, що деяка частина стану елементів змінюється.
        change - об'єкт базового класу для всіх графічних елементів.
        value - силка на клас QVariant
    */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    /*Вершина.*/
    QPolygonF polygonObject;
    /*Список дуг*/
    QList<Arrow *> arrows;
    /*Назва вершини.*/
    QString nameObject;
    /*Порядковий номер вершини.*/
    static int number;
};

#endif // DIAGRAMITEM_H
