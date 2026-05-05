#include "arc.h"

Arc::Arc(QGraphicsItem* source, QGraphicsItem* dest, QGraphicsItem* parent)
    : QGraphicsLineItem(parent),
      source(source),
      dest(dest)
{
    setPen(QPen(Qt::black, 2));
    setZValue(-1);
    updatePosition();
}

void Arc::updatePosition() {
    // draw a line between the centre points of the two items
    QPointF p1 = source->scenePos();
    QPointF p2 = dest->scenePos();
    setLine(QLineF(p1, p2));
}
