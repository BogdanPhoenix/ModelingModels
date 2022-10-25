#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>
#include "diagramitem.h"
#include <QtMath>

/*Клас для побудови дуг у графі.*/
class Arrow : public QGraphicsLineItem
{
public:
    /*Тип об'єкту.*/
    enum { Type = UserType + 4 };
    /*Режим побудови напрямків стрілок-вказівників.*/
    enum Direction
    {
        /*Стрілка в одному напрямку.*/
        One,
        /*Стрілки в обидва напрямки.*/
        All
    };
    /*
        Конструктор класу.
        startItem - вказівник на об'єкт від якого бере свій початок дуга.
        endItem - вказівник на об'єкт до якого прямує дуга.
        parent - вказівник на батьківський клас.
    */
    explicit Arrow(DiagramItem *startItem, DiagramItem *endItem, QGraphicsItem *parent = nullptr);
    /*Повертає тип даного класу.*/
    int type() const override { return Type; }
    /*
        Метод, який визначає межі елемента, як прямокутник.
        Повертає кінцевий прямокутник.
    */
    QRectF boundingRect() const override;
    /*Метод для визначення форми об'єкта.*/
    QPainterPath shape() const override;
    /*
        Метод для отримання кольору дуги.
        color - змінна, яка отримує необхідний колір.
    */
    void setColor(const QColor &color);
    /*
        Метод для отримання режиму побудови напрямку дуги.
    */
    void setDirectionMode(Direction mode);
    /*Метод оновлення позиції стрілки.*/
    void updatePosition();
    /*Метод для отримання доступу до об'єкта, від якого починає свій початок дуга, без можливості його зміни.*/
    DiagramItem *getStartItem() const;
    /*Метод для отримання доступу до об'єкта, до якого прямує дуга, без можливості його зміни.*/
    DiagramItem *getEndItem() const;

protected:
    /*
        Метод для побудови дуги.
        painter - вказівник на об'єкт класу, який малює віджети.
        option - вказівник на об'єкт класу, який описує параметри, які необхідні для малювання.
        widget - вказівник на об'єкт батьківського класу.
    */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    /*
        Метод для визначення крайньої точки стрілки, для її правильної побудови.
        item - вказівник на об'єкт, до якого прилягає стрілка.
    */
    QPointF updateArrowPosition(DiagramItem *item);
    /*Вказівник на об'єкт класу, з якого бере початок дуга.*/
    DiagramItem *startItem;
    /*Вказівник на об'єкт класу, до якого прямує дуга.*/
    DiagramItem *endItem;
    /*Полігон для побудови стрілки дуги, яка прямує до об'єкту endItem.*/
    QPolygonF arrowHead;
    /*Полігон для побудови стрілки дуги, яка прямує до об'єкту startItem.*/
    QPolygonF arrowTail;
    /*Колір дуги.*/
    QColor color;
    /*Режим побудови дуги.*/
    Direction directionMode;
};

#endif // ARROW_H
