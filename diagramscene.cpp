#include "diagramscene.h"
#include <arrow.h>

DiagramScene::DiagramScene(QObject *parent) : QGraphicsScene(parent)
{
    modeItem = MoveItem;
    line = nullptr;
    textItem = nullptr;
    itemColorScene = Qt::white;
    textColorScene = Qt::black;
    lineColorScene = Qt::black;
    fontScene = QFont();
}

void DiagramScene::setLineColor(const QColor &color)
{
    lineColorScene = color;
    if (isItemChange(Arrow::Type))
    {
        Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().at(0));
        item->setColor(lineColorScene);
        update();
    }
}

void DiagramScene::setTextColor(const QColor &color)
{
    textColorScene = color;
    if (isItemChange(DiagramTextItem::Type))
    {
        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().at(0));
        item->setDefaultTextColor(textColorScene);
    }
}

void DiagramScene::setItemColor(const QColor &color)
{
    itemColorScene = color;
    if (isItemChange(DiagramItem::Type))
    {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().at(0));
        item->setBrush(itemColorScene);
    }
}

void DiagramScene::setFont(const QFont &font)
{
    fontScene = font;

    if (isItemChange(DiagramTextItem::Type))
    {
        QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().at(0));
        if (item)
        {
            item->setFont(fontScene);
        }
    }
}

void DiagramScene::setMode(Mode mode)
{
    modeItem = mode;
}

void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty())
    {
        removeItem(item);
        item->deleteLater();
    }
}

void DiagramScene::createArrow(QGraphicsItem *startItems, QGraphicsItem *endItems, Mode mode)
{
    DiagramItem *startItem = qgraphicsitem_cast<DiagramItem *>(startItems);
    DiagramItem *endItem = qgraphicsitem_cast<DiagramItem *>(endItems);
    Arrow *arrow = new Arrow(startItem, endItem);
    switch(mode)
    {
    case InsertLineOne: arrow->setDirectionMode(Arrow::One); break;
    case InsertLineAll: arrow->setDirectionMode(Arrow::All); break;
    default: break;
    }
    arrow->setColor(lineColorScene);
    startItem->addArrow(arrow);
    endItem->addArrow(arrow);
    arrow->setZValue(-1000.0);
    addItem(arrow);
    arrow->updatePosition();
    emit createConnect(startItem, endItem, mode);
}

void DiagramScene::addDiagramItem(Mode mode, QPointF position)
{
    DiagramItem *item;
    switch(mode)
    {
    case InsertItem:
        item = new DiagramItem;
        item->setBrush(itemColorScene);
        addItem(item);
        item->setPos(position);
        emit itemInserted(item);
        break;
    case InsertLineOne:
    case InsertLineAll:
        line = new QGraphicsLineItem(QLineF(position, position));
        line->setPen(QPen(lineColorScene, 2));
        addItem(line);
        break;
    case InsertText:
        textItem = new DiagramTextItem;
        textItem->setFont(fontScene);
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setZValue(1000.0);
        connect(textItem, &DiagramTextItem::lostFocus, this, &DiagramScene::editorLostFocus);
        connect(textItem, &DiagramTextItem::selectedChange, this, &DiagramScene::itemSelected);
        addItem(textItem);
        textItem->setDefaultTextColor(textColorScene);
        textItem->setPos(position);
        emit textInserted(textItem);
        break;
    default:;
    }
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
    {
        addDiagramItem(modeItem, mouseEvent->scenePos());
        QGraphicsScene::mousePressEvent(mouseEvent);
    }
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if ((modeItem == InsertLineOne || modeItem == InsertLineAll) && line != nullptr)
    {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    }
    else if (modeItem == MoveItem)
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (line != nullptr && (modeItem == InsertLineOne || modeItem == InsertLineAll))
    {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        deleteLine(startItems);

        if(startItems.count())
        {
            QList<QGraphicsItem *> endItems = items(line->line().p2());
            bool checkConnect = true;
            bool reverseConnect = true;
            deleteLine(endItems);

            if(!endItems.count())
            {
                Mode buffer = modeItem;
                addDiagramItem(InsertItem, mouseEvent->scenePos());
                endItems = items(line->line().p2());
                deleteLine(endItems);
                modeItem = buffer;
            }

            if (startItems.count() > 0 && endItems.count() > 0 && startItems.first()->type() == DiagramItem::Type && endItems.first()->type() == DiagramItem::Type && startItems.first() != endItems.first())
            {
                emit checkConnects(startItems.first(), endItems.first(), checkConnect);
                emit checkConnects(endItems.first(), startItems.first(), reverseConnect);
                if(reverseConnect)
                {
                    emit message("Між даними вершинами присутній зв'язок. Для побудови нового зв'яку видалить старий.");
                }
                else if(!checkConnect)
                {
                    createArrow(startItems.first(), endItems.first(), modeItem);
                }
                else
                {
                    emit message("Дуга в казаному напрямку вже існує. Додати нову в тому самому напрямку - не можливо.");
                }
            }
        }
        removeItem(line);
        delete line;
    }
    line = nullptr;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void DiagramScene::deleteLine(QList<QGraphicsItem *> &items)
{
    for(int i = 0; i < items.count(); ++i)
    {
        if(items.at(i)->type() == QGraphicsLineItem::Type)
        {
            items.removeAt(i--);
        }
    }
}

bool DiagramScene::isItemChange(int type) const
{
    const QList<QGraphicsItem *> items = selectedItems();
    const auto cb = [type](const QGraphicsItem *item)
    {
        return item->type() == type;
    };
    return std::find_if(items.begin(), items.end(), cb) != items.end();
}
