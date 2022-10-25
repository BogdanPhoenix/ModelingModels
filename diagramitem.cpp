#include "diagramitem.h"
#include <arrow.h>

int DiagramItem::number = 0;

DiagramItem::DiagramItem(QGraphicsItem *parent) : QGraphicsPolygonItem(parent)
{
    QPainterPath path;
    path.addEllipse(QPointF(0, 0), 50, 50);
    polygonObject = path.toFillPolygon();
    this->nameObject = QString::number(number++);
    setPolygon(polygonObject);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void DiagramItem::removeArrow(Arrow *arrow)
{
    try
    {
        arrows.removeAll(arrow);
    }
    catch (...) { }
}

void DiagramItem::removeArrows()
{
    const auto arrowsCopy = arrows;
    for (Arrow *arrow : arrowsCopy)
    {
        arrow->getStartItem()->removeArrow(arrow);
        arrow->getEndItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}

void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}

QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(polygonObject);

    return pixmap;
}

QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange)
    {
        for (Arrow *arrow : qAsConst(arrows))
        {
            arrow->updatePosition();
        }
    }

    return value;
}
