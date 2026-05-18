// ------------------------------
// transition.cpp
//
// Authors:
// Richard Henček (xhencer00)
// Šimon Varga (xvargas00)
//
// Implements transition objects of Petri net.
// ------------------------------

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

// Module headers
#include "transition.hpp"
#include "arc.hpp"
#include "properties_dialog.h"
#include "petriscene.hpp"



Transition::Transition(QString name, int delay, QGraphicsItem* parent) :
    QGraphicsRectItem(-15, -40, 30, 80, parent), name(name), availability(Disabled), delayMs(delay), timer(0)
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setPen(QPen(Qt::black));
}

void Transition::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    // Strip the selection flag before passing to base class
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    if (availability == Waiting) {
        setBrush(QBrush(Qt::blue));
    }
    else if (availability == Fireable) {
        setBrush(QBrush(QColor(0, 220, 0)));
    }
    else {
        setBrush(QBrush(Qt::darkGray));
    }

    QGraphicsRectItem::paint(painter, &myOption, widget);

    // Draw selection manually around just the rectangle
    if (option->state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect().adjusted(-1, -1, 1, 1));
    }


    // Draw delay timer in the middle
    painter->setPen(QPen(Qt::white));
    painter->drawText(rect(), Qt::AlignCenter, QString::number(delayMs));

    // Draw name above the rectangle
    painter->setPen(QPen(Qt::black));
    QRectF nameRect = rect().adjusted(-60, -25, 60, 0);
    nameRect.setHeight(20);
    painter->drawText(nameRect, Qt::AlignCenter, name);
}

QVariant Transition::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged && scene()) {
        // Tell the scene to update all arcs connected to this item
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

    if (petriScene->currentTool != PetriScene::SelectTool) {
        return;
    }

    bool readOnly = (petriScene->currentMode == PetriScene::RunMode);
    TransitionPropertiesDialog dialog(name, delayMs, eventName, guard, action, readOnly);

    while(true) {
        if (dialog.exec() == QDialog::Accepted) {
            if (readOnly) {
                break;
            }

            QString newName = dialog.nameLineEdit->text();

            if (newName != name && petriScene->isTransitionNameTaken(newName, this)) {
                QMessageBox::warning(nullptr, "Name taken", "A transition named '" + newName + "' already exists.");

                continue;
            }

            name = newName;
            setDelay(dialog.delaySpinBox->value());
            eventName = dialog.eventLineEdit->text();
            guard = dialog.guardTextEdit->toPlainText();
            action = dialog.actionTextEdit->toPlainText();

            if (petriScene) {
                petriScene->updateFireability();
            }

            break;
        }
        else {
            break;
        }
    }

    event->accept();
}
