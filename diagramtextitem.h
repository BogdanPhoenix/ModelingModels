#ifndef DIAGRAMTEXTITEM_H
#define DIAGRAMTEXTITEM_H

#include <QGraphicsTextItem>

/*Клас для створення текстового поля.*/
class DiagramTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    /*Тип об'єкту.*/
    enum { Type = UserType +3 };
    /*
        Конструктор класу.
        parent - вказівник на батьківський клас.
    */
    explicit DiagramTextItem(QGraphicsItem *parent = nullptr);
    /*Повертає тип даного класу.*/
    int type() const override { return Type; }

signals:
    /*
        Посилання сигналу завершення редагування тексту, після відсутності фокусу на текстовому елементі.
        item - вказівник на елемент тексту.
    */
    void lostFocus(DiagramTextItem *item);
    /*
        Посилання сигналу після зміні елементу.
        item - вказівник на елемент тексту.
    */
    void selectedChange(QGraphicsItem *item);

protected:
    /*
        Метод для повідомлення користувацьких елементів про те, що деяка частина стану елементів змінюється.
        change - об'єкт базового класу для всіх графічних елементів.
        value - силка на клас QVariant
    */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    /*
        Подія фокусування на елементі.
        event - вказівник на об'єкт класу параметрів подій фокусування віджетів.
    */
    void focusOutEvent(QFocusEvent *event) override;
    /*
        Подія подвійного натиску на клавішу мишки.
        mouseEvent - вказівник на об'єкт класу подій миші в структурному графічному перегляді.
    */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // DIAGRAMTEXTITEM_H
