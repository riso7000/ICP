#include <QGraphicsScene>
#include <QTimer>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QDialogButtonBox>

#include "transition.h"
#include "arc.h"
#include "properties_dialog.h"

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

    // 'widget' can usually be found via the view, or just pass NULL
    TransitionPropertiesDialog dialog(delay_ms, eventName, guard);

    if (dialog.exec() == QDialog::Accepted) {
        // Only update if the user clicked OK
        setDelay(dialog.delaySpinBox->value());
        guard = dialog.guardTextEdit->toPlainText();
        eventName = dialog.eventLineEdit->text();
    }

    QGraphicsRectItem::mouseDoubleClickEvent(event);
}
