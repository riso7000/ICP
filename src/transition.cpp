#include "transition.h"

Transition::Transition(int id, QGraphicsItem* parent)
    : QGraphicsRectItem(-15, -40, 30, 80, parent),
      id(id)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setPen(QPen(Qt::black));
    setBrush(QBrush(Qt::darkGray));
}

void Transition::paint(QPainter* painter,
                  const QStyleOptionGraphicsItem* option,
                  QWidget* widget)
{
    // First let the base class draw the circle
    QGraphicsRectItem::paint(painter, option, widget);

    // Implement the rest later

}

