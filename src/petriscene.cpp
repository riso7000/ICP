#include "petriscene.h"
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>

#include "place.h"
#include "transition.h"
#include "arc.h"

PetriScene::PetriScene(QObject* parent)
    : QGraphicsScene(parent),
      currentTool(SelectTool),
      obj_id(0),
      arcSource(0)
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
    else if (currentTool == ArcTool) {
        QGraphicsItem* clicked = itemAt(event->scenePos(), QTransform());

        if (!clicked) return; // clicked empty space, ignore

        if (!arcSource) {
            // first click - store source
            arcSource = clicked;
        } else {
            if (clicked != arcSource) {
                bool sourceIsPlace      = dynamic_cast<Place*>(arcSource);
                bool sourceIsTransition = dynamic_cast<Transition*>(arcSource);
                bool destIsPlace        = dynamic_cast<Place*>(clicked);
                bool destIsTransition   = dynamic_cast<Transition*>(clicked);

                // only create arc if source and dest are different types
                if ((sourceIsPlace && destIsTransition) ||
                    (sourceIsTransition && destIsPlace)) {
                    // check if an arc already exists between these two items
                        bool alreadyConnected = false;
                        foreach (QGraphicsItem* item, items()) {
                            Arc* arc = dynamic_cast<Arc*>(item);
                            if (arc) {
                                if ((arc->getSource() == arcSource && arc->getDest() == clicked) ||
                                    (arc->getSource() == clicked   && arc->getDest() == arcSource)) {
                                    alreadyConnected = true;
                                    break;
                                }
                            }
                        }

                        if (!alreadyConnected) {
                            Arc* arc = new Arc(arcSource, clicked);
                            addItem(arc);
                        }
                }
            }
            arcSource = 0;
        }
    }
    else {
        QGraphicsScene::mousePressEvent(event);
    }
}
