#include "petriscene.h"
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>

PetriScene::PetriScene(QObject* parent)
    : QGraphicsScene(parent),
      currentTool(SelectTool),
      obj_id(0)
{}

void PetriScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (currentTool == PlaceTool) {
        Place* place = new Place(obj_id++, obj_id);
        place->setPos(event->scenePos());
        addItem(place);
    }
    else if (currentTool == TransitionTool) {
        auto* item = addRect(-15, -40, 30, 80,
                             QPen(Qt::black), QBrush(Qt::darkGray));
        item->setPos(event->scenePos());
        item->setFlag(QGraphicsItem::ItemIsMovable);
        item->setFlag(QGraphicsItem::ItemIsSelectable);
    }
    else {
        QGraphicsScene::mousePressEvent(event);
    }
}
