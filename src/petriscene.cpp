#include "petriscene.h"
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QTimer>


#include <algorithm>

#include "place.h"
#include "transition.h"
#include "arc.h"

PetriScene::PetriScene(QObject* parent)
    : QGraphicsScene(parent),
      currentTool(SelectTool),
      obj_id(0),
      arcSource(0),
      active(true),
      sensorValue(42)
{
    env.load("vars", "bool active = true; int sensorValue = 42;");
}

void PetriScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (currentTool == PlaceTool) {
        Place* place = new Place(obj_id++, 0);
        place->setPos(event->scenePos());
        places.push_back(place);
        addItem(place);
    }
    else if (currentTool == TransitionTool) {
        Transition* transition = new Transition(obj_id++, 0);
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
                            Arc* arc = new Arc(obj_id++, arcSource, clicked, 1);

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

    if (t->input_arcs.empty()) return false;

    // check token counts
    for (Arc* arc : t->input_arcs) {
        Place* place = dynamic_cast<Place*>(arc->getSource());
        if (!place) continue;
        if (place->getTokens() < arc->getWeight())
            return false;
    }

    // check guard
    if (!evaluateGuard(t->guard))
        return false;

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
    // Step 1: fire all immediate transitions until stable
    bool fired = true;
    while (fired) {
        fired = false;

        for (Transition* t : transitions) {
            if (!t->eventName.isEmpty()) continue;
            if (t->isImmediate() && isFireable(t)) {
                fireTransition(t);
                fired = true;
                break; // restart the loop after each firing
                       // so we recheck all transitions with updated tokens
            }
        }
    }

    updateAvailability();

    // Step 2: schedule timers for timed transitions that are fireable
    scheduleTimers();
}

void PetriScene::updateAvailability() {
    for (Transition* t : transitions) {
        if (!isFireable(t))
            cancelTimer(t);
        t->setAvailability(t->timer ? Transition::Waiting : Transition::Disabled);
    }
}



void PetriScene::scheduleTimers() {
    for (Transition* t : transitions) {
        if (!t->eventName.isEmpty()) continue;
        if (t->isImmediate()) continue;     // skip immediate transitions
        if (!isFireable(t)) continue;        // skip non-fireable transitions
        if (t->timer) continue;              // timer already running

        qDebug() << "Scheduling timer for transition" << t->getId()
                 << "delay:" << t->delay_ms << "ms";

        t->timer = new QTimer();
        t->timer->setSingleShot(true);
        t->setAvailability(Transition::Waiting);

        // capture t by value so the lambda knows which transition fired
        connect(t->timer, &QTimer::timeout, [this, t]() {
            onTimerExpired(t);
        });

        t->timer->start(t->delay_ms);
    }
}



void PetriScene::onTimerExpired(Transition* t) {
    qDebug() << "Timer expired for transition" << t->getId();

    // clean up the timer
    delete t->timer;
    t->timer = 0;

    // check if still fireable at the moment of expiry
    if (!isFireable(t)) {
        qDebug() << "Timeout ignored - transition" << t->getId() << "no longer fireable";
        return;
    }

    fireTransition(t);
    updateAvailability();

    // after firing, run immediate stabilization and reschedule timers
    stabilize();
}


void PetriScene::cancelTimer(Transition* t) {
    if (t->timer) {
        t->timer->stop();
        delete t->timer;
        t->timer = 0;
        qDebug() << "Cancelled timer for transition" << t->getId();
    }
}


bool PetriScene::evaluateGuard(const QString& guard) {
    if (guard.isEmpty()) return true;

    static int guardCounter = 0;
    std::string scriptName = "guard_" + std::to_string(guardCounter++);

    std::string code = "bool __result = (" + guard.toStdString() + ");";

    bool loaded = env.load(scriptName.c_str(), code.c_str());
    if (!loaded) {
        qDebug() << "CFlat failed to load guard:" << guard;
        qDebug() << "Error:" << env.getErrorMessage();
        return false;
    }

    Cflat::Value* result = env.getVariable("__result");
    if (!result) {
        qDebug() << "__result variable not found after loading guard";
        return false;
    }
    bool val = CflatValueAs(result, bool);
    qDebug() << "Checked guard:" << guard << "got result:" << (val ? "true" : "false");
    return CflatValueAs(result, bool);
}


void PetriScene::setActive(bool value) {
    Cflat::Value* v = env.getVariable("active");
    if (v) CflatValueAs(v, bool) = value;
    postEvent("active"); // notify any transitions waiting on this
}

void PetriScene::setSensorValue(int value) {
    Cflat::Value* v = env.getVariable("sensorValue");
    if (v) CflatValueAs(v, int) = value;
    postEvent("sensorValue");
}


void PetriScene::postEvent(const QString& name) {
    // Step 1: fire immediate event-gated transitions for this event
    bool fired = true;
    while (fired) {
        fired = false;
        for (Transition* t : transitions) {
            if (t->eventName != name) continue;
            if (t->isImmediate() && isFireable(t)) {
                fireTransition(t);
                fired = true;
                break;
            }
        }
    }

    // Step 2: schedule timed event-gated transitions for this event
    for (Transition* t : transitions) {
        if (t->eventName != name) continue;
        if (t->isImmediate()) continue;
        if (!isFireable(t)) continue;
        if (t->timer) continue; // already scheduled

        t->timer = new QTimer();
        t->timer->setSingleShot(true);
        t->setAvailability(Transition::Waiting);
        connect(t->timer, &QTimer::timeout, [this, t]() { onTimerExpired(t); });
        t->timer->start(t->delay_ms);
    }

    // Step 3: after event-gated firings, re-stabilize the free transitions
    // because tokens may have moved into places that enable free transitions
    stabilize();
}
