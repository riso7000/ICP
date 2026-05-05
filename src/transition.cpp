#include <QGraphicsScene>

#include "transition.h"
#include "arc.h"

Transition::Transition(int id, QGraphicsItem* parent)
    : QGraphicsRectItem(-15, -40, 30, 80, parent),
      id(id)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
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

QVariant Transition::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged && scene()) {
        // tell the scene to update all arcs connected to this item
        foreach (QGraphicsItem* item, scene()->items()) {
            Arc* arc = dynamic_cast<Arc*>(item);
            if (arc && (arc->getSource() == this || arc->getDest() == this)) {
                arc->updatePosition();
            }
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

