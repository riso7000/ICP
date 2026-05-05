#include <QGraphicsScene>


#include "place.h"
#include "arc.h"



Place::Place(int id, int initial_tokens, QGraphicsItem* parent)
    : QGraphicsEllipseItem(-25, -25, 50, 50, parent),
      id(id),
      tokens(initial_tokens)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
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

QVariant Place::itemChange(GraphicsItemChange change, const QVariant& value) {
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
