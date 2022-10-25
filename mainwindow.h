#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QMainWindow>
#include <QToolBox>
#include <QToolButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QLabel>
#include <QMessageBox>
#include <QMap>
#include <QToolBar>

#include "diagramitem.h"
#include "diagramscene.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
/*Клас для створення вікна програми*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*
        Конструктор класу.
        parent - вказівник на батьківський клас.
    */
    MainWindow(QWidget *parent = nullptr);

private:
    Ui::MainWindow *ui;
    /*Метод для створення бокового меню.*/
    void createToolBox();
    /*Метод для створення кнопок редагування елементів.*/
    void createActions();
    /*Метод для створення меню взаємодії та редагуваня елементів.*/
    void createToolbars();
    /*
        Метод для перевірки чи можна видалити дугу.
        item - вказівник на об'єкт класу, від якого виходить вершина.
    */
    bool checkVertex(QGraphicsItem *item);
    /*
        Метод для видалення дуги без збереження зв'язків.
        item - вказівник на об'єкт класу, від якого виходить вершина.
    */
    void deleteVertex(QGraphicsItem *item);
    /*
     *  Метод для видалення дуги зі збереження зв'язків.
     *  item - вказівник на об'єкт класу, від якого виходить вершина.
     */
    void saveVertex(QGraphicsItem *item);
    /*
        Метод для створення кнопок бокової панелі.
        text - підпис кнопок.
    */
    QWidget *createCellWidget(const QString &text);
    /*
     *  Метод для створення меню вибору кольору.
     *  slot - який буде задієний для вибору кольору.
     *  defaultColor - кольор за замовчуванням.
     */
    QMenu *createColorMenu(const char *slot, QColor defaultColor);
    /*
     *  Метод для створення кнопки зі зображенням відповідного пункту.
     *  image - шлях до зображення.
     *  color - активний колір.
     */
    QIcon createColorToolButtonIcon(const QString &image, QColor color);
    /*
     *  Метод для сворення зображення активного кольору.
     *  color - активний колір.
     */
    QIcon createColorIcon(QColor color);
    /*
     *  Метод для додання імені вершинам.
     *  name - ім'я
     */
    void addItemName(QString name = "");
    /*
     *  Метод для перевірки видалення дуги.
     *  arrow - вказівник на об'єкт класу дуги.
     */
    bool checkArrow(Arrow *arrow);
    /*
     *  Метод для видалення дуги.
     *  arrow - вказівник на об'єкт класу дуги.
     */
    void deleteArrowConnect(Arrow *arrow);
    /*
     *  Метод для видалення зайвих елементів зі списку.
     *  item - силка на список із елементами.
     */
    void removeExcess(QList<QGraphicsItem *> &item);
    /*
     *  Метод для оновлення комірки в матриці істинності.
     *  startItem - вказівник на об'єкт класу початкової вершини.
     *  endItem - вказівник на об'єкт класу кінцевої вершини.
     *  value - нове значення.
     */
    void cellUpdate(DiagramItem *startItem, DiagramItem *endItem, int value);
    /*
     *  Метод для пошуку координат комірки в матриці істинності.
     *  startItem - вказівник на об'єкт класу початкової вершини.
     *  endItem - вказівник на об'єкт класу кінцевої вершини.
     */
    QPoint searchCell(DiagramItem *startItem, DiagramItem *endItem = nullptr);

    const int InsertTextButton = 10;
    /*Сцена для малювання.*/
    DiagramScene *scene;
    /*Об'єкт для відображення елементів.*/
    QGraphicsView *view;
    /*Кнопка для видалення вершин без збереження зв'язків.*/
    QAction *deleteActionNotSave;
    /*Кнопка для видалення вершин зі збереження зв'язків.*/
    QAction *deleteActionSave;
    /*Поле для взаємодії з текстом.*/
    QToolBar *textToolBar;
    /*Поле для різних варіантів видалення елементів.*/
    QToolBar *editToolBar;
    /*Поле для вибору кольору.*/
    QToolBar *colorToolBar;
    /*Поле для вибору режиму вказівника миші.*/
    QToolBar *pointerToolbar;
    /*Випадаючий список для вибору маштабу.*/
    QComboBox *sceneScaleCombo;
    /*Випадаючий список для розміру шрифту.*/
    QComboBox *fontSizeCombo;
    /*Випадаючий список для вибору шрифту.*/
    QFontComboBox *fontCombo;
    /*Об'єкт бокового поля.*/
    QToolBox *toolBox;
    /*Об'єкт групи кнопок бокового меню.*/
    QButtonGroup *buttonGroup;
    /*Об'єкт групи режиму курсору миші.*/
    QButtonGroup *pointerTypeGroup;
    /*Кнопка випадаючого списку вибору кольору шрифта.*/
    QToolButton *fontColorToolButton;
    /*Кнопка випадаючого списку вибору кольору заповнення вершин.*/
    QToolButton *fillColorToolButton;
    /*Кнопка випадаючого списку вибору кольору дуги.*/
    QToolButton *lineColorToolButton;
    /*Об'єкт жирних шрифта.*/
    QAction *boldAction;
    /*Об'єкт підкресленого шрифта.*/
    QAction *underlineAction;
    /*Об'єкт курсивного шрифта.*/
    QAction *italicAction;
    /*Кнопка вибору кольору шрифта.*/
    QAction *textAction;
    /*Кнопка вибору кольору заповнення вершини.*/
    QAction *fillAction;
    /*Кнопка вибору кольору дуги.*/
    QAction *lineAction;
    /*Матриця істинності.*/
    QTableWidget *tableItem;
    /*Список з'єднань між вершинами.*/
    QMultiMap<QGraphicsItem *, QGraphicsItem*> *connectsItems;
    /*Список вершин.*/
    QSet<QGraphicsItem *> *vertex;

signals:
    /*
        Посилання сигналу, для створення елементу.
        startItems - вказівник на елемент класу початкової вершини.
        endItems - вказівник на елемент класу кінцевої вершини.
    */
    void createArrow(QGraphicsItem *startItems, QGraphicsItem *endItems, DiagramScene::Mode mode);

private slots:
    /*
        Слот для визначення режиму створення елементу.
        button - вказівник об'єкт класу, яка відповідає за відповідний режим.
    */
    void buttonGroupClicked(QAbstractButton *button);
    /*Слот для видалення елементу.*/
    void deleteItem();
    /*Слот для вибору режиму вказівника миші.*/
    void pointerGroupClicked();
    /*
        Слот для обробки створеного елемента.
        item - вказівник на елемент.
    */
    void itemInserted(DiagramItem *item);
    /*
        Слот для обробки створеного текстового елемента.
        item - вказівник на елемент.
    */
    void textInserted(QGraphicsTextItem *item);
    /*
        Слот змінення поточного шрифту.
        font - силка на шрифт.
    */
    void currentFontChanged(const QFont &font);
    /*
        Слот змінення розміру шрифта.
        size - силка на новий розмір.
    */
    void fontSizeChanged(const QString &size);
    /*
        Слот змінення маштабу.
        scale - силка на новий маштаб.
    */
    void sceneScaleChanged(const QString &scale);
    /*Слот для вибору кольору тексту.*/
    void textColorChanged();
    /*Слот для вибору кольору вершини.*/
    void itemColorChanged();
    /*Слот для вибору кольору дуги.*/
    void lineColorChanged();
    /*Слот для зміни кольору тексту.*/
    void textButtonTriggered();
    /*Слот для зміни кольору вершини.*/
    void fillButtonTriggered();
    /*Слот для зміни кольору дуги.*/
    void lineButtonTriggered();
    /*Слот для обробки тексту.*/
    void handleFontChange();
    /*
        Слот обробки сигналу, що елемент вибрано.
        item - вказівник на елемент.
    */
    void itemSelected(QGraphicsItem *item);
    /*
        Слот для створення з'єднання двох вершин.
        startItem - вказівник на об'єкт класу початкової вершини.
        endItem - вказівник на об'єкт класу кінцевої вершини.
        arrowMode - режим побудови вказівників дуги.
    */
    void createConnect(DiagramItem *startItem, DiagramItem *endItem, DiagramScene::Mode mode);
    /*
        Слот для перевірки з'єднання між двома вершинами.
        startItems - вказівник на елемент класу початкової вершини.
        endItems - вказівник на елемент класу кінцевої вершини.
        check - посилання на результат перевірки.
    */
    void checkConnects(QGraphicsItem *startItem, QGraphicsItem *endItem, bool &check);
    /*
        Слот для виведення технічних повідомлень.
        message - текст повідомлення.
    */
    void message(QString message);

};
#endif // MAINWINDOW_H
