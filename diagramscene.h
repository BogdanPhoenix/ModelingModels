#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include "diagramitem.h"
#include "diagramtextitem.h"
#include "qaction.h"

/*Клас для взаємодії об'єктів на сцені.*/
class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    /*Режими взаємодії з об'єктами на сцені.*/
    enum Mode
    {
        /*Додати елемент.*/
        InsertItem,
        /*Додати дугу в одному напрямку.*/
        InsertLineOne,
        /*Додати дугу в двох напрямках.*/
        InsertLineAll,
        /*Додати текст.*/
        InsertText,
        /*Переміщення елементу.*/
        MoveItem
    };
    /*
        Конструктор класу.
        parent - вказівник на об'єкт батьківсько класу.
    */
    explicit DiagramScene(QObject *parent = nullptr);
    /*Метод, який повертає шрифт сцени.*/
    QFont font() const { return fontScene; }
    /*Метод, який повертає колір тексту сцени.*/
    QColor textColor() const { return textColorScene; }
    /*Метод, який повертає колір елементу сцени.*/
    QColor itemColor() const { return itemColorScene; }
    /*Метод, який повертає колір дуги сцени.*/
    QColor lineColor() const { return lineColorScene; }
    /*
        Метод для отримання кольору дуги.
        color - силка на колір дуги.
    */
    void setLineColor(const QColor &color);
    /*
        Метод для отримання кольору тексту.
        color - силка на колір тексту.
    */
    void setTextColor(const QColor &color);
    /*
        Метод для отримання кольору вершини.
        color - силка на колір вершини.
    */
    void setItemColor(const QColor &color);
    /*
        Метод для отримання шрифту сцени.
        color - силка на шифр.
    */
    void setFont(const QFont &font);

public slots:
    /*
        Слот для отримання режиму побудови елементу.
        mode - режим.
    */
    void setMode(DiagramScene::Mode mode);
    /*
        Слот для завершення редагування тексту, після відсутності фокусу на текстовому елементі.
        item - вказівник на елемент тексту.
    */
    void editorLostFocus(DiagramTextItem *item);
    /*
        Слот для додання дуг.
        startItems - вказівник на початкову вершину.
        endItems - вказівник на кінцеву вершину.
        mode - режим дуги.
    */
    void createArrow(QGraphicsItem *startItems, QGraphicsItem *endItems, DiagramScene::Mode mode);

signals:
    /*
        Посилання сигналу, що елемент створено.
        item - вказівник на елемент.
    */
    void itemInserted(DiagramItem *item);
    /*
        Посилання сигналу, що текстовий елемент створено.
        item - вказівник на елемент.
    */
    void textInserted(QGraphicsTextItem *item);
    /*
        Посилання сигналу, що елемент вибрано.
        item - вказівник на елемент.
    */
    void itemSelected(QGraphicsItem *item);
    /*
        Посилання сигналу, для створення з'єднання двох вершин.
        startItem - вказівник на об'єкт класу початкової вершини.
        endItem - вказівник на об'єкт класу кінцевої вершини.
        arrowMode - режим побудови вказівників дуги.
    */
    void createConnect(DiagramItem *startItem, DiagramItem *endItem, DiagramScene::Mode arrowMode);
    /*
        Посилання сигналу, для перевірки чи існує вже з'єднання між вказаними вершинами.
        startItem - вказівник на об'єкт класу початкової вершини.
        endItem - вказівник на об'єкт класу кінцевої вершини.
        check - силка на результат перевірки.
    */
    void checkConnects(QGraphicsItem *startItem, QGraphicsItem *endItem, bool &check);
    /*
        Сигнал для надсилання технічних повідомлень.
        message - поле для передачи повідомлення.
    */
    void message(QString message);

protected:
    /*
        Подія натиску на клавішу мишки.
        mouseEvent - вказівник на об'єкт класу подій миші в структурному графічному перегляді.
    */
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    /*
        Подія переміщення мишки.
        mouseEvent - вказівник на об'єкт класу подій миші в структурному графічному перегляді.
    */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    /*
        Подія відпускання клавіші мишки.
        mouseEvent - вказівник на об'єкт класу подій миші в структурному графічному перегляді.
    */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private:
    /*
        Метод для перевірки зміни елементів.
        type - тип елемента.
    */
    bool isItemChange(int type) const;
    /*
        Метод для видалення лінії, яка створюється підчас перетягування миші для візуального відображення дуги.
        items - вказівник на список елементів.
    */
    void deleteLine(QList<QGraphicsItem *> &items);
    /*
        Метод для додання елементу на сцену.
        mode - режим елемента.
        position - розташування елемента.
    */
    void addDiagramItem(Mode mode, QPointF position);

    /*Режим елемента.*/
    Mode modeItem;
    /*Елемент лінії для візуального відображення дуги.*/
    QGraphicsLineItem *line;
    /*Шрифт тексту.*/
    QFont fontScene;
    /*Елемент текстового поля.*/
    DiagramTextItem *textItem;
    /*Колір тексту.*/
    QColor textColorScene;
    /*Колір елементу.*/
    QColor itemColorScene;
    /*Колір дуги.*/
    QColor lineColorScene;
};

#endif // DIAGRAMSCENE_H
