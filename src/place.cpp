#include <QGraphicsScene>
#include <QSpinBox>
#include <QStyleOptionGraphicsItem>
#include <QMessageBox>

#include "place.h"
#include "arc.h"
#include "petriscene.h"
#include "properties_dialog.h"



Place::Place(QString name, int initial_tokens, QGraphicsItem* parent)
    : QGraphicsEllipseItem(-25, -25, 50, 50, parent),
      name(name),
      tokens(initial_tokens)
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setPen(QPen(Qt::black));
    setBrush(QBrush(Qt::white));
    init_tokens = initial_tokens;
    lastTokenChange = QDateTime::currentDateTime();
}

void Place::paint(QPainter* painter,
                  const QStyleOptionGraphicsItem* option,
                  QWidget* widget)
{

    // strip the selection flag before passing to base class
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    // First let the base class draw the circle
    QGraphicsEllipseItem::paint(painter, &myOption, widget);

    // draw selection manually around just the rectangle
    if (option->state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect());
    }

    // Then draw the token count in the center
    painter->drawText(rect(), Qt::AlignCenter, QString::number(tokens));


    // Draw name above the rectangle
    painter->setPen(QPen(Qt::black));
    QRectF nameRect = rect().adjusted(-60, -25, 60, 0);
    nameRect.setHeight(20);
    painter->drawText(nameRect, Qt::AlignCenter, name);
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


void Place::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    PetriScene* petriScene = dynamic_cast<PetriScene*>(scene());
    if (!petriScene || petriScene->currentMode == PetriScene::RunMode) return;
    if (petriScene->currentTool != PetriScene::SelectTool) return;

    PlacePropertiesDialog dialog(name, tokens);
    while(true) {
        if (dialog.exec() == QDialog::Accepted) {
            QString newName = dialog.nameLineEdit->text();
            if (newName != name && petriScene->isPlaceNameTaken(newName, this)) {
                QMessageBox::warning(nullptr, "Name taken",
                    "A place named '" + newName + "' already exists.");
                continue; // don't save
            }
            name = newName;
            setTokens(dialog.tokensSpinBox->value());
            update();
            break;
        }
        else break;
    }


    QGraphicsEllipseItem::mouseDoubleClickEvent(event);
}

void Place::setTokens(int t) {
    tokens = t;
    lastTokenChange = QDateTime::currentDateTime();
    update(); // update() triggers repaint

    // notify scene to recheck fireability
    PetriScene* petriScene = dynamic_cast<PetriScene*>(scene());
    if (petriScene) petriScene->updateFireability();
}
