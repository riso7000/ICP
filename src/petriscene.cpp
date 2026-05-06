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
        places.push_back(place);
        addItem(place);
    }
    else if (currentTool == TransitionTool) {
        Transition* transition = new Transition(obj_id++);
        transition->setPos(event->scenePos());
        transitions.push_back(transition);
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
                            Arc* arc = new Arc(obj_id, arcSource, clicked, obj_id++);

                            Transition* srcTrans  = dynamic_cast<Transition*>(arcSource);
                            Transition* destTrans = dynamic_cast<Transition*>(clicked);

                            if (srcTrans) {
                                srcTrans->output_arcs.push_back(arc);

                                qDebug() << "Transition" << srcTrans->getId()
                                          << "output arcs:";
                                 for (Arc* a : srcTrans->output_arcs)
                                     qDebug() << "  Arc" << a->getId();
                            }

                            if (destTrans) {
                                destTrans->input_arcs.push_back(arc);

                                qDebug() << "Transition" << destTrans->getId()
                                          << "input arcs:";
                                 for (Arc* a : destTrans->input_arcs)
                                     qDebug() << "  Arc" << a->getId();
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

            if (srcTrans)
                srcTrans->output_arcs.erase(
                    std::remove(srcTrans->output_arcs.begin(),
                                srcTrans->output_arcs.end(), clickedArc),
                    srcTrans->output_arcs.end());

            if (destTrans)
                destTrans->input_arcs.erase(
                    std::remove(destTrans->input_arcs.begin(),
                                destTrans->input_arcs.end(), clickedArc),
                    destTrans->input_arcs.end());

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

                if (srcTrans)
                    srcTrans->output_arcs.erase(
                        std::remove(srcTrans->output_arcs.begin(),
                                    srcTrans->output_arcs.end(), arc),
                        srcTrans->output_arcs.end());

                if (destTrans)
                    destTrans->input_arcs.erase(
                        std::remove(destTrans->input_arcs.begin(),
                                    destTrans->input_arcs.end(), arc),
                        destTrans->input_arcs.end());

                removeItem(arc);
                delete arc;
            }
            if (clickedPlace) places.erase(std::remove(places.begin(), places.end(), clickedPlace), places.end());
            if (clickedTrans) transitions.erase(std::remove(transitions.begin(), transitions.end(), clickedTrans), transitions.end());
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


bool PetriScene::isFireable(Transition* t) {
    for (Arc* arc : t->input_arcs) {
        Place* place = dynamic_cast<Place*>(arc->getSource());
        if (!place) continue;
        if (place->getTokens() < arc->getWeight())
            return false;
    }
    return true;
}


void PetriScene::fireTransition(Transition* t) {
    // consume tokens from input places
    for (Arc* arc : t->input_arcs) {
        Place* place = dynamic_cast<Place*>(arc->getSource());
        if (place)
            place->setTokens(place->getTokens() - arc->getWeight());
    }

    // produce tokens in output places
    for (Arc* arc : t->output_arcs) {
        Place* place = dynamic_cast<Place*>(arc->getDest());
        if (place)
            place->setTokens(place->getTokens() + arc->getWeight());
    }

    qDebug() << "Fired transition" << t->getId();
}

void PetriScene::stabilize() {
    bool fired = true;

    while (fired) {
        fired = false;

        // collect all currently fireable transitions
        std::vector<Transition*> fireable;
        for (Transition* t : transitions) {
            if (isFireable(t))
                fireable.push_back(t);
        }

        if (fireable.empty()) break;

        // fire all fireable transitions
        for (Transition* t : fireable) {
            fireTransition(t);
            fired = true;
        }

        // update visual availability state
        updateAvailability();
    }
}

void PetriScene::updateAvailability() {
    for (Transition* t : transitions) {
        if (isFireable(t))
            t->setAvailability(Transition::Available);
        else
            t->setAvailability(Transition::Disabled);
    }
}
