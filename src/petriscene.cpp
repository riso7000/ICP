#include "petriscene.h"
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>
#include <QDebug>


#include <algorithm>

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
                            Arc* arc = new Arc(arcSource, clicked, obj_id++);

                            // cast to access the place/transition specific members
                            Transition* srcTrans  = dynamic_cast<Transition*>(arcSource);
                            Transition* destTrans = dynamic_cast<Transition*>(clicked);
                            Place*      srcPlace  = dynamic_cast<Place*>(arcSource);
                            Place*      destPlace = dynamic_cast<Place*>(clicked);

                            if (srcTrans && destPlace) { // transition → place
                                srcTrans->output_places.push_back(destPlace);

                                /* qDebug() << "Transition" << srcTrans->getId()
                                                 << "output places:";
                                        for (Place* p : srcTrans->output_places)
                                            qDebug() << "  Place" << p->getId(); */
                            }
                            if (destTrans && srcPlace) { // place → transition
                                destTrans->input_places.push_back(srcPlace);

                                /* qDebug() << "Transition" << destTrans->getId()
                                                 << "input places:";
                                        for (Place* p : destTrans->input_places)
                                            qDebug() << "  Place" << p->getId();
                                            */
                            }

                            addItem(arc);
                        }
                }
            }
            arcSource = 0;
        }
    }
    else if (currentTool == DeleteTool) {
        QGraphicsItem* clicked = itemAt(event->scenePos(), QTransform());
        if (!clicked) return;

        Arc* clickedArc         = dynamic_cast<Arc*>(clicked);
        Place* clickedPlace     = dynamic_cast<Place*>(clicked);
        Transition* clickedTrans = dynamic_cast<Transition*>(clicked);

        if (clickedArc) {
            // deleting an arc directly - just update lists and remove it
            Transition* srcTrans = dynamic_cast<Transition*>(clickedArc->getSource());
            Transition* destTrans = dynamic_cast<Transition*>(clickedArc->getDest());
            Place* srcPlace      = dynamic_cast<Place*>(clickedArc->getSource());
            Place* destPlace     = dynamic_cast<Place*>(clickedArc->getDest());

            if (srcTrans && destPlace)
                srcTrans->output_places.erase(
                    std::remove(srcTrans->output_places.begin(),
                                srcTrans->output_places.end(), destPlace),
                    srcTrans->output_places.end());

            if (destTrans && srcPlace)
                destTrans->input_places.erase(
                    std::remove(destTrans->input_places.begin(),
                                destTrans->input_places.end(), srcPlace),
                    destTrans->input_places.end());

            removeItem(clickedArc);
            delete clickedArc;

        } else if (clickedPlace || clickedTrans) {
            // deleting a place or transition - collect arcs first, then delete
            QList<Arc*> arcsToDelete;
            foreach (QGraphicsItem* item, items()) {
                Arc* arc = dynamic_cast<Arc*>(item);
                if (arc && (arc->getSource() == clicked || arc->getDest() == clicked))
                    arcsToDelete.append(arc);
            }

            foreach (Arc* arc, arcsToDelete) {
                Transition* srcTrans  = dynamic_cast<Transition*>(arc->getSource());
                Transition* destTrans = dynamic_cast<Transition*>(arc->getDest());
                Place*      srcPlace  = dynamic_cast<Place*>(arc->getSource());
                Place*      destPlace = dynamic_cast<Place*>(arc->getDest());

                if (srcTrans && destPlace)
                    srcTrans->output_places.erase(
                        std::remove(srcTrans->output_places.begin(),
                                    srcTrans->output_places.end(), destPlace),
                        srcTrans->output_places.end());

                if (destTrans && srcPlace)
                    destTrans->input_places.erase(
                        std::remove(destTrans->input_places.begin(),
                                    destTrans->input_places.end(), srcPlace),
                        destTrans->input_places.end());

                removeItem(arc);
                delete arc;
            }

            removeItem(clicked);
            delete clicked;
        }
    }
    else {
        QGraphicsScene::mousePressEvent(event);
    }
}


void PetriScene::setTool(Tool tool) {
    currentTool = tool;

    bool selectable = (tool == SelectTool);

    foreach (QGraphicsItem* item, items()) {

        bool isArc = dynamic_cast<Arc*>(item);

        if (!isArc) {
            item->setFlag(QGraphicsItem::ItemIsMovable,   selectable);
        }
        item->setFlag(QGraphicsItem::ItemIsSelectable, selectable);
    }
}
