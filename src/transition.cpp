#include <QGraphicsScene>
#include <QTimer>

#include "transition.h"
#include "arc.h"

Transition::Transition(int id, int delay, QGraphicsItem* parent)
    : QGraphicsRectItem(-15, -40, 30, 80, parent),
      id(id),
      availability(Disabled),
      delay_ms(delay),
      timer(0)
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setPen(QPen(Qt::black));
}

void Transition::paint(QPainter* painter,
                  const QStyleOptionGraphicsItem* option,
                  QWidget* widget)
{
    if (availability == Waiting) setBrush(QBrush(Qt::blue));
    else setBrush(QBrush(Qt::darkGray));
    QGraphicsRectItem::paint(painter, option, widget);

    // Draw delay timer in the middle
    painter->setPen(QPen(Qt::white));
    painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(delay_ms));

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


void Transition::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    if (!(flags() & QGraphicsItem::ItemIsSelectable)) return;

    bool ok;
    // -1 means no timer, so show 0 as "enter 0 or more for timed, -1 for immediate"
    int value = QInputDialog::getInt(
        NULL,
        "Edit Delay",
        "Delay (ms), 0 for immediate:",
        delay_ms,
        0,     // minimum
        999999, // maximum
        1,
        &ok
    );

    if (ok) setDelay(value);

    QGraphicsRectItem::mouseDoubleClickEvent(event);
}
