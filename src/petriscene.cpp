#include "petriscene.h"
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>

#include "place.h"
#include "transition.h"

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
        Transition* transition = new Transition(obj_id++);
        transition->setPos(event->scenePos());
        addItem(transition);
    }
    else {
        QGraphicsScene::mousePressEvent(event);
    }
}
