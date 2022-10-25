#include "mainwindow.h"
#include "diagramscene.h"
#include "qmenu.h"
#include <arrow.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    tableItem = new QTableWidget;
    tableItem->setMinimumWidth(300);
    createActions();
    createToolBox();

    scene = new DiagramScene();
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
    connect(scene, &DiagramScene::itemInserted, this, &MainWindow::itemInserted);
    connect(scene, &DiagramScene::textInserted, this, &MainWindow::textInserted);
    connect(scene, &DiagramScene::itemSelected, this, &MainWindow::itemSelected);
    connect(scene, &DiagramScene::createConnect, this, &MainWindow::createConnect);
    connect(this, &MainWindow::createArrow, scene, &DiagramScene::createArrow);
    connect(scene, &DiagramScene::checkConnects, this, &MainWindow::checkConnects);
    connect(scene, &DiagramScene::message, this, &MainWindow::message);
    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);
    view = new QGraphicsView(scene);
    layout->addWidget(view);
    layout->addWidget(tableItem);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle("Програма для побудови графів");
    connectsItems = new QMultiMap<QGraphicsItem *, QGraphicsItem *>();
    vertex = new QSet<QGraphicsItem *>();
}

void MainWindow::buttonGroupClicked(QAbstractButton *button)
{
    const int id = buttonGroup->id(button);
    scene->setMode(id ==InsertTextButton ? DiagramScene::InsertText : DiagramScene::InsertItem);
}

void MainWindow::deleteItem()
{
    for(QGraphicsItem *item : scene->selectedItems())
    {
        if(item->type() == Arrow::Type){
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            if(!checkArrow(arrow))
            {
                QMessageBox::about(this, tr("Повідомлення"), tr("Видалити даний вузол не можливо. Утворюється два різні графи з різними стартовими вершинами."));
                return;
            }
            deleteArrowConnect(arrow);
            scene->removeItem(item);
            arrow->getStartItem()->removeArrow(arrow);
            arrow->getEndItem()->removeArrow(arrow);
            delete item;
        }
    }

    for(QGraphicsItem *item : scene->selectedItems())
    {
        if(item->type() == DiagramItem::Type)
        {
            DiagramItem *buffer = qgraphicsitem_cast<DiagramItem *>(item);
            if(!checkVertex(buffer))
            {
                QMessageBox::about(this, tr("Повідомлення"), tr("Видалити вершину не можливо."));
                return;
            }
            buffer->removeArrows();
            deleteVertex(item);
        }
        scene->removeItem(item);

        delete item;
    }
}

bool MainWindow::checkArrow(Arrow *arrow)
{
    QList<QGraphicsItem *> endItems = scene->items(arrow->line().p1());
    removeExcess(endItems);
    if(!endItems.count())
    {
        return false;
    }
    return connectsItems->keys(endItems.first()).count() > 1;
}

void MainWindow::deleteArrowConnect(Arrow *arrow)
{
    QList<QGraphicsItem *> startItems = scene->items(arrow->line().p2());
    QList<QGraphicsItem *> endItems = scene->items(arrow->line().p1());
    removeExcess(startItems);
    removeExcess(endItems);
    cellUpdate(dynamic_cast<DiagramItem*>(startItems.first()), dynamic_cast<DiagramItem*>(endItems.first()), false);
    cellUpdate(dynamic_cast<DiagramItem*>(endItems.first()), dynamic_cast<DiagramItem*>(startItems.first()), false);

    connectsItems->remove(startItems.first(), endItems.first());
    connectsItems->remove(endItems.first(), startItems.first());
}

void MainWindow::removeExcess(QList<QGraphicsItem *> &item)
{
    item.removeIf([](QGraphicsItem *item){ return item->type() != DiagramItem::Type; });
}

bool MainWindow::checkVertex(QGraphicsItem *item)
{
    QList<QGraphicsItem *> in = connectsItems->keys(item);
    QList<QGraphicsItem *> out = connectsItems->values(item);
    if(!(!in.count() && out.count() == 1) || !(in.count() && !out.count()))
    {
        QMap<QGraphicsItem*, bool> *result = new QMap<QGraphicsItem*, bool>();

        int index = 0;
        foreach (QGraphicsItem *it, *vertex)
        {
            if(it != item && out.contains(it))
            {
                result->insert(it, connectsItems->keys(it).count() - 1 >= 1);
            }
        }
        foreach (QGraphicsItem *key, result->keys())
        {
            if(!result->value(key))
            {
                ++index;
            }
        }
        if(index > 1)
        {
            return false;
        }
    }
    return true;
}

void MainWindow::deleteVertex(QGraphicsItem *item)
{
    DiagramItem *vertex = dynamic_cast<DiagramItem*>(item);
    QPoint deleteRowColumn = searchCell(vertex);

    if(QObject::sender() == deleteActionSave)
    {
        saveVertex(item);
    }

    tableItem->removeColumn(deleteRowColumn.x());
    tableItem->removeRow(deleteRowColumn.x());

    connectsItems->remove(item);
    QList<QGraphicsItem *> connection = connectsItems->values();
    for(int i = 0; i < connection.count(); ++i)
    {
        if(connection.at(i) == item)
        {
            connectsItems->remove(connectsItems->key(connection.at(i)), connection.at(i));
            connection.remove(i);
            --i;
        }
    }
}

void MainWindow::saveVertex(QGraphicsItem *item)
{
    QList<QGraphicsItem *> in = connectsItems->keys(item);
    QList<QGraphicsItem *> out = connectsItems->values(item);
    foreach (QGraphicsItem *itIn, in) {
        foreach (QGraphicsItem *itOut, out) {
            if(!connectsItems->contains(itIn, itOut))
            {
                emit createArrow(itIn, itOut, DiagramScene::InsertLineOne);
            }
        }
    }
}

void MainWindow::pointerGroupClicked()
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::itemInserted(DiagramItem *item)
{
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));

    if(tableItem)
    {
        QTableWidgetItem *rowHeader = new QTableWidgetItem(item->name());
        QTableWidgetItem *columnHeader = new QTableWidgetItem(item->name());

        vertex->insert(item);

        tableItem->setColumnCount(tableItem->columnCount() + 1);
        tableItem->setRowCount(tableItem->rowCount() + 1);

        tableItem->setVerticalHeaderItem(tableItem->rowCount() - 1, rowHeader);
        tableItem->setHorizontalHeaderItem(tableItem->columnCount() - 1, columnHeader);

        tableItem->setColumnWidth(tableItem->columnCount() - 1, 20);

        for(int i = 0; i < tableItem->rowCount(); ++i)
        {
            QVariant rVariant(int(false));
            QVariant cVariant(int(false));

            QTableWidgetItem *rowItem = new QTableWidgetItem();
            QTableWidgetItem *columnItem = new QTableWidgetItem();

            rowItem->setData( Qt::DisplayRole, rVariant );
            columnItem->setData(Qt::DisplayRole, cVariant);

            tableItem->setItem(tableItem->rowCount() - 1, i, rowItem);
            tableItem->setItem(i, tableItem->rowCount() - 1, columnItem);
        }
    }
}

void MainWindow::textInserted(QGraphicsTextItem *)
{
    buttonGroup->button(InsertTextButton)->setChecked(false);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::currentFontChanged(const QFont &)
{
    handleFontChange();
}

void MainWindow::fontSizeChanged(const QString &)
{
    handleFontChange();
}

void MainWindow::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QTransform oldMatrix = view->transform();
    view->resetTransform();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(newScale, newScale);
}

void MainWindow::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon( ":/img/img/font-small.svg", qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}

void MainWindow::itemColorChanged()
{
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(":/img/img/fill-drip-small.svg", qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}

void MainWindow::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon( ":/img/img/diagonal-line.svg", qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}

void MainWindow::textButtonTriggered()
{
    scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}

void MainWindow::fillButtonTriggered()
{
    scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}

void MainWindow::lineButtonTriggered()
{
    scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}

void MainWindow::handleFontChange()
{
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    scene->setFont(font);
}

void MainWindow::itemSelected(QGraphicsItem *item)
{
    DiagramTextItem *textItem = qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    boldAction->setChecked(font.weight() == QFont::Bold);
    italicAction->setChecked(font.italic());
    underlineAction->setChecked(font.underline());
}

void MainWindow::createToolBox()
{
    buttonGroup = new QButtonGroup();
    buttonGroup->setExclusive(false);
    connect(buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, &MainWindow::buttonGroupClicked);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(createCellWidget(tr("Вершина")), 0, 0);

    QToolButton *textButton = new QToolButton;
    textButton->setCheckable(true);

    buttonGroup->addButton(textButton, InsertTextButton);
    textButton->setIcon(QIcon(QPixmap(":/img/img/font.svg").scaled(30, 30)));
    textButton->setIconSize(QSize(50, 50));

    QGridLayout *textLayout = new QGridLayout;
    textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
    textLayout->addWidget(new QLabel("Текст"), 1, 0, Qt::AlignHCenter);

    QWidget *textWidget = new QWidget;
    textWidget->setLayout(textLayout);
    layout->addWidget(textWidget, 1, 0);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidget, "");
}

void MainWindow::createActions()
{
    deleteActionNotSave = new QAction(QIcon(":/img/img/trash-not-save.svg"), tr("&Видалити без збереження зв'язків"), this);
    deleteActionNotSave->setShortcut(tr("Delete"));
    deleteActionNotSave->setStatusTip("Видалити елемент із діаграми (без збереження зв'язків)");
    connect(deleteActionNotSave, &QAction::triggered, this, &MainWindow::deleteItem);

    deleteActionSave = new QAction(QIcon(":/img/img/trash-save.svg"), tr("&Видалити зі збереження зв'язків"), this);
    deleteActionSave->setShortcut(tr("Delete+S"));
    deleteActionSave->setStatusTip("Видалити елемент із діаграми (зі збереження зв'язків)");
    connect(deleteActionSave, &QAction::triggered, this, &MainWindow::deleteItem);

    boldAction = new QAction(QIcon(":/img/img/bold.svg"), "Жирний", this);
    boldAction->setCheckable(true);
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, &QAction::triggered, this, &MainWindow::handleFontChange);

    italicAction = new QAction(QIcon(":/img/img/italic.svg"), "Курсив", this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, &QAction::triggered, this, &MainWindow::handleFontChange);

    underlineAction = new QAction(QIcon(":/img/img/underline.svg"), "Підчеркнутий", this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, &QAction::triggered, this, &MainWindow::handleFontChange);
}

void MainWindow::createToolbars()
{
    editToolBar = this->addToolBar("Редагувати");
    editToolBar->addAction(deleteActionNotSave);
    editToolBar->addAction(deleteActionSave);

    fontCombo = new QFontComboBox;
    connect(fontCombo, &QFontComboBox::currentFontChanged, this, &MainWindow::currentFontChanged);

    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);
    for(int i = 8; i <= 30; i+=2)
    {
        fontSizeCombo->addItem(QString::number(i));
    }
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
    connect(fontSizeCombo, &QComboBox::currentTextChanged, this, &MainWindow::fontSizeChanged);

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));

    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(":/img/img/font-small.svg", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, &QToolButton::clicked, this, &MainWindow::textButtonTriggered);

    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon( ":/img/img/fill-drip-small.svg", Qt::white));
    connect(fillColorToolButton, &QAbstractButton::clicked, this, &MainWindow::fillButtonTriggered);

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(":/img/img/diagonal-line.svg", Qt::black));
    connect(lineColorToolButton, &QToolButton::clicked, this, &MainWindow::lineButtonTriggered);

    textToolBar = this->addToolBar("Шрифт");
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);

    colorToolBar = this->addToolBar("Колір");
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/img/img/arrow-pointer.svg"));

    QToolButton *linePointerOneButton = new QToolButton;
    linePointerOneButton->setCheckable(true);
    linePointerOneButton->setIcon(QIcon(":/img/img/connect-one.svg"));

    QToolButton *linePointerAllButton = new QToolButton;
    linePointerAllButton->setCheckable(true);
    linePointerAllButton->setIcon(QIcon(":/img/img/connect-all.svg"));

    pointerTypeGroup = new QButtonGroup;
    pointerTypeGroup->addButton(pointerButton, DiagramScene::MoveItem);
    pointerTypeGroup->addButton(linePointerOneButton, DiagramScene::InsertLineOne);
    pointerTypeGroup->addButton(linePointerAllButton, DiagramScene::InsertLineAll);
    connect(pointerTypeGroup, &QButtonGroup::buttonClicked, this, &MainWindow::pointerGroupClicked);

    sceneScaleCombo = new QComboBox;
    QStringList scale;
    scale << "50%" << "75%" << "100%" << "125%" << "150%";
    sceneScaleCombo->addItems(scale);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, &QComboBox::currentTextChanged, this, &MainWindow::sceneScaleChanged);

    pointerToolbar = this->addToolBar("Тип покажчика");
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerOneButton);
    pointerToolbar->addWidget(linePointerAllButton);
    pointerToolbar->addWidget(sceneScaleCombo);
}

QWidget *MainWindow::createCellWidget(const QString &text)
{
    DiagramItem item;
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    buttonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

QMenu *MainWindow::createColorMenu(const char *slot, QColor defaultColor)
{
    QMenu *colorMenu = new QMenu(this);
    const QStringList colorNames = QColor::colorNames();
    foreach (const QString &colorName, colorNames) {
        const QColor color(colorName);

        QAction *action = new QAction(colorName, this);
        action->setData(colorName);
        action->setIcon(createColorIcon(color));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (color == defaultColor){
            colorMenu->setDefaultAction(action);
        }
    }
    return colorMenu;
}

QIcon MainWindow::createColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);

    QRect target(0, 0, 50, 60);
    QRect source(0, 0, 50, 50);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);
    painter.end();

    return QIcon(pixmap);
}

QIcon MainWindow::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);
    painter.end();

    return QIcon(pixmap);
}

void MainWindow::createConnect(DiagramItem *startItem, DiagramItem *endItem, DiagramScene::Mode mode)
{
    connectsItems->insert(startItem, endItem);
    cellUpdate(startItem, endItem, true);
    if(mode == DiagramScene::InsertLineAll)
    {
        connectsItems->insert(endItem, startItem);
        cellUpdate(endItem, startItem, true);
    }
}

void MainWindow::checkConnects(QGraphicsItem *startItem, QGraphicsItem *endItem, bool &check)
{
    check = connectsItems->contains(startItem, endItem);
}

void MainWindow::message(QString message)
{
    QMessageBox::about(this, tr("Увага"), message);
}

void MainWindow::cellUpdate(DiagramItem *startItem, DiagramItem *endItem, int value)
{
    QPoint cell = searchCell(startItem, endItem);

    if(cell.x() < 0 || cell.y() < 0)
    {
        return;
    }

    QVariant oVariant(value);
    QTableWidgetItem *cellItem = new QTableWidgetItem();

    cellItem->setData( Qt::DisplayRole, oVariant );
    tableItem->removeCellWidget(cell.x(), cell.y());
    tableItem->setItem(cell.x(), cell.y(), cellItem);
}

QPoint MainWindow::searchCell(DiagramItem *startItem, DiagramItem *endItem)
{
    QPoint cell(-1, -1);
    for(int i = 0; i < tableItem->rowCount(); ++i)
    {
        if(cell.x() >= 0 && cell.y() >= 0)
        {
            break;
        }
        else if(tableItem->verticalHeaderItem(i)->text() == startItem->name())
        {
            cell.setX(i);
            if(!endItem)
            {
                cell.setY(i);
            }
        }
        else if(endItem && tableItem->verticalHeaderItem(i)->text() == endItem->name())
        {
            cell.setY(i);
        }
    }
    return cell;
}
