#include "arrow.h"

Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem, QGraphicsItem *parent) : QGraphicsLineItem(parent)
{
    this->startItem = startItem;
    this->endItem = endItem;
    color = Qt::black;
    directionMode = One;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;
    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(), line().p2().y() - line().p1().y())).normalized().adjusted(-extra, -extra, extra, extra);
}

QPainterPath Arrow::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    if(directionMode == All)
    {
        path.addPolygon(arrowTail);
    }
    return path;
}

void Arrow::setColor(const QColor &color)
{
    this->color = color;
}

DiagramItem *Arrow::getStartItem() const
{
    return startItem;
}

DiagramItem *Arrow::getEndItem() const
{
    return endItem;
}

void Arrow::setDirectionMode(Direction mode)
{
    directionMode = mode;
}

void Arrow::updatePosition()
{
    QLineF line(mapFromItem(startItem, 0, 0), mapFromItem(endItem, 0, 0));
    setLine(line);
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (startItem->collidesWithItem(endItem))
    {
        return;
    }

    QPen myPen = pen();
    QPointF intersectStartPoint = updateArrowPosition(endItem);
    QPointF intersectEndPoint;
    double angle = qAcos(line().dx() / line().length());
    qreal arrowSize = 20;

    myPen.setColor(color);
    painter->setPen(myPen);
    painter->setBrush(color);

    switch (directionMode) {
    case One:
        intersectEndPoint = startItem->pos();
        break;
    case All:
        intersectEndPoint = updateArrowPosition(startItem);
        break;
    }

    setLine(QLineF(intersectStartPoint, intersectEndPoint));
    //Визначення кута нахилу стрілок
    if(line().dy() >= 0)
    {
        angle = (M_PI * 2.0) - angle;
    }

    QPointF arrowP1 = line().p1() + QPointF(qSin(angle + M_PI / 3.0) * arrowSize, qCos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = line().p1() + QPointF(qSin(angle + M_PI - M_PI / 3) * arrowSize, qCos(angle + M_PI - M_PI / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p1() << arrowP1 << arrowP2;

    painter->drawLine(line());
    painter->drawPolygon(arrowHead);

    if(directionMode == All)
    {
        QPointF arrowP3 = line().p2() - QPointF(qSin(angle + M_PI / 3.0) * arrowSize, qCos(angle + M_PI / 3) * arrowSize);
        QPointF arrowP4 = line().p2() - QPointF(qSin(angle + M_PI - M_PI / 3) * arrowSize, qCos(angle + M_PI - M_PI / 3) * arrowSize);
        arrowTail.clear();
        arrowTail << line().p2() << arrowP3 << arrowP4;
        painter->drawPolygon(arrowTail);
    }
    if (isSelected())
    {
        painter->setPen(QPen(color, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }
}

QPointF Arrow::updateArrowPosition(DiagramItem *item)
{
    QLineF centerLine(startItem->pos(), endItem->pos());
    QPolygonF endPolygon = item->polygon();
    QPointF p1 = endPolygon.first() + item->pos();
    QPointF intersectPoint;

    for (int i = 1; i < endPolygon.count(); ++i)
    {
        QPointF p2 = endPolygon.at(i) + item->pos();
        QLineF polyLine = QLineF(p1, p2);
        QLineF::IntersectionType intersectionType = polyLine.intersects(centerLine, &intersectPoint);
        if (intersectionType == QLineF::BoundedIntersection)
        {
            break;
        }
        p1 = p2;
    }

    return intersectPoint;
}
