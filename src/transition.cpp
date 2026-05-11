#include <QGraphicsScene>
#include <QTimer>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QStyleOptionGraphicsItem>
#include <QMessageBox>

#include "transition.h"
#include "arc.h"
#include "properties_dialog.h"
#include "petriscene.h"

Transition::Transition(QString name, int delay, QGraphicsItem* parent)
    : QGraphicsRectItem(-15, -40, 30, 80, parent),
      name(name),
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

    // strip the selection flag before passing to base class
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    if (availability == Waiting) setBrush(QBrush(Qt::blue));
    else setBrush(QBrush(Qt::darkGray));
    QGraphicsRectItem::paint(painter, &myOption, widget);

    // draw selection manually around just the rectangle
    if (option->state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect().adjusted(-1, -1, 1, 1));
    }


    // Draw delay timer in the middle
    painter->setPen(QPen(Qt::white));
    painter->drawText(rect(), Qt::AlignCenter, QString::number(delay_ms));

    // Draw name above the rectangle
    painter->setPen(QPen(Qt::black));
    QRectF nameRect = rect().adjusted(-60, -25, 60, 0);
    nameRect.setHeight(20);
    painter->drawText(nameRect, Qt::AlignCenter, name);
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

    PetriScene* petriScene = dynamic_cast<PetriScene*>(scene());
    if (!petriScene || petriScene->currentMode == PetriScene::RunMode) return;
    if (petriScene->currentTool != PetriScene::SelectTool) return;

    // 'widget' can usually be found via the view, or just pass NULL
    TransitionPropertiesDialog dialog(name, delay_ms, eventName, guard, action);

    if (dialog.exec() == QDialog::Accepted) {
        QString newName = dialog.nameLineEdit->text();
            if (newName != name && petriScene->isTransitionNameTaken(newName, this)) {
                QMessageBox::warning(nullptr, "Name taken",
                    "A transition named '" + newName + "' already exists.");
                return;
            }
        name = newName;
        setDelay(dialog.delaySpinBox->value());
        eventName = dialog.eventLineEdit->text();
        guard = dialog.guardTextEdit->toPlainText();
        action = dialog.actionTextEdit->toPlainText();

    }

    QGraphicsRectItem::mouseDoubleClickEvent(event);
}
