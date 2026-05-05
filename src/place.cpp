#include "place.h"

Place::Place(int id, int initial_tokens, QGraphicsItem* parent)
    : QGraphicsEllipseItem(-25, -25, 50, 50, parent),
      id(id),
      tokens(initial_tokens)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setPen(QPen(Qt::black));
    setBrush(QBrush(Qt::white));
}

void Place::paint(QPainter* painter,
                  const QStyleOptionGraphicsItem* option,
                  QWidget* widget)
{
    // First let the base class draw the circle
    QGraphicsEllipseItem::paint(painter, option, widget);

    // Then draw the token count in the center
    painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(tokens));
}
