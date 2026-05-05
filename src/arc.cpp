// arc.cpp
#include "arc.h"
#include <QPointF>
#include <cmath>

static const double ARROW_SIZE = 12.0; // size of the arrowhead

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
    QPointF p1 = source->scenePos();
    QPointF p2 = dest->scenePos();
    setLine(QLineF(p1, p2));
}

void Arc::paint(QPainter* painter,
                const QStyleOptionGraphicsItem* option,
                QWidget* widget)
{
    QLineF l = line();
    if (l.length() < 1.0) return;

    // find the point where the line hits the edge of the destination item
    QPointF tip = l.p2(); // fallback to centre
    QPainterPath destShape = dest->shape();

    for (int i = 0; i < (int)l.length(); ++i) {
        QPointF p = l.pointAt(i / l.length());
        // map from scene coordinates to dest item's local coordinates
        QPointF localP = dest->mapFromScene(p);
        if (destShape.contains(localP)) {
            tip = p;
            break;
        }
    }

    // shorten the line so it ends at the edge, not the centre
    setLine(QLineF(l.p1(), tip));
    QGraphicsLineItem::paint(painter, option, widget);
    setLine(l); // restore full line for correct hit detection

    double angle = std::atan2(l.dy(), l.dx());

    QPointF base1(
        tip.x() - ARROW_SIZE * std::cos(angle - M_PI / 6),
        tip.y() - ARROW_SIZE * std::sin(angle - M_PI / 6)
    );
    QPointF base2(
        tip.x() - ARROW_SIZE * std::cos(angle + M_PI / 6),
        tip.y() - ARROW_SIZE * std::sin(angle + M_PI / 6)
    );

    QPolygonF arrowHead;
    arrowHead << tip << base1 << base2;

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::black);
    painter->drawPolygon(arrowHead);
}
