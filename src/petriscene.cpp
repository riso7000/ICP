#include "petriscene.h"
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QTimer>
#include <QRegularExpression>


#include <algorithm>

#include "place.h"
#include "transition.h"
#include "arc.h"

PetriScene::PetriScene(QObject* parent)
    : QGraphicsScene(parent),
      currentTool(SelectTool),
      placeId(0),
      transitionId(0),
      arcId(0),
      arcSource(0)
{
    startTime = QDateTime::currentDateTime();
}

void PetriScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        deleteItemAt(event->scenePos());
        updateFireability();
        return;
    }
    if (event->button() != Qt::LeftButton) return;


    if (currentTool == PlaceTool) {
        // 1. Keep looping until we find a name that isn't taken
        bool collision = true;
        while (collision) {
            collision = false;
            QString potentialName = QString("Place%1").arg(placeId);

            for (Place* p : places) {
                if (p->getName() == potentialName) {
                    placeId++; // ID is taken, increment and try again
                    collision = true;
                    break; // Exit the for-loop to re-check the whole list with the new ID
                }
            }
        }

        Place* place = new Place("Place" + QString::number(placeId++), 0);
        place->setPos(event->scenePos());
        places.push_back(place);
        addItem(place);
    }
    else if (currentTool == TransitionTool) {
        // 1. Keep looping until we find a name that isn't taken
        bool collision = true;
        while (collision) {
            collision = false;
            QString potentialName = QString("Transition%1").arg(transitionId);

            for (Transition* t : transitions) {
                if (t->getName() == potentialName) {
                    transitionId++; // ID is taken, increment and try again
                    collision = true;
                    break; // Exit the for-loop to re-check the whole list with the new ID
                }
            }
        }

        Transition* transition = new Transition("Transition" + QString::number(transitionId++), 0);
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
                            Arc* arc = new Arc(arcSource, clicked, 1);

                            Transition* srcTrans  = dynamic_cast<Transition*>(arcSource);
                            Transition* destTrans = dynamic_cast<Transition*>(clicked);

                            if (srcTrans) {
                                srcTrans->output_arcs.push_back(arc);

                            }

                            if (destTrans) {
                                destTrans->input_arcs.push_back(arc);
                            }


                            addItem(arc);
                            updateFireability();
                        }
                }
            }
            arcSource = 0;
        }
    }
    else if (currentTool == DeleteTool) {
        deleteItemAt(event->scenePos());
        updateFireability();
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

void PetriScene::clearNet() {
    variables.clear();
    inputs.clear();
    outputs.clear();

    for (Transition* tr : transitions) {
        for (Arc* arc : tr->input_arcs) {
            delete arc;
        }

        for (Arc* arc : tr->output_arcs) {
            delete arc;
        }

        delete tr;
    }

    transitions.clear();

    for (Place* pl : places) {
        delete pl;
    }

    places.clear();
}


void PetriScene::deleteItemAt(const QPointF& scenePos) {
    QGraphicsItem* clicked = itemAt(scenePos, QTransform());
    if (!clicked) return;

    Arc* clickedArc         = dynamic_cast<Arc*>(clicked);
    Place* clickedPlace     = dynamic_cast<Place*>(clicked);
    Transition* clickedTrans = dynamic_cast<Transition*>(clicked);

    if (clickedArc) {
        Transition* srcTrans  = dynamic_cast<Transition*>(clickedArc->getSource());
        Transition* destTrans = dynamic_cast<Transition*>(clickedArc->getDest());

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


void PetriScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (currentTool == DeleteTool &&
        event->buttons() & (Qt::LeftButton | Qt::RightButton)) {
        deleteItemAt(event->scenePos());
        updateFireability();
        return;
    }
    QGraphicsScene::mouseMoveEvent(event);
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
    log("FIRED: " + t->name, 1);

    // consume tokens from input places
    for (Arc* arc : t->input_arcs) {
        Place* place = dynamic_cast<Place*>(arc->getSource());
        if (place) {
            int oldTokens = place->tokens;
            place->setTokens(place->getTokens() - arc->getWeight());
            log("TOKENS: " + place->name + " " +
                QString::number(oldTokens) + "->" + QString::number(place->tokens), 2);
        }
    }

    // produce tokens in output places
    for (Arc* arc : t->output_arcs) {
        Place* place = dynamic_cast<Place*>(arc->getDest());
        if (place) {
            int oldTokens = place->tokens;
            place->setTokens(place->getTokens() + arc->getWeight());
            log("TOKENS: " + place->name + " " +
                QString::number(oldTokens) + "->" + QString::number(place->tokens), 2);
        }
    }

    executeAction(t->action);

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
        bool fireable = isFireable(t);
        if (fireable && t->becameFireable.isNull())
            t->becameFireable = QDateTime::currentDateTime();
        else if (!fireable)
            t->becameFireable = QDateTime(); // reset

        if (!isFireable(t)) cancelTimer(t);
        t->setAvailability(t->timer ? Transition::Waiting : Transition::Disabled);
    }
}



void PetriScene::scheduleTimers() {
    for (Transition* t : transitions) {
        if (!t->eventName.isEmpty()) continue;
        if (t->isImmediate()) continue;     // skip immediate transitions
        if (!isFireable(t)) continue;        // skip non-fireable transitions
        if (t->timer) continue;              // timer already running

        log("TIMER SCHEDULED: " + t->name + " : " + QString::number(t->delay_ms) + "ms");

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
    log("TIMEOUT EXPIRED: " + t->name);

    // clean up the timer
    delete t->timer;
    t->timer = 0;

    // check if still fireable at the moment of expiry
    if (!isFireable(t)) {
        log("TIMEOUT IGNORED: " + t->name);
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
        log("TIMEOUT CANCELLED: " + t->name);
    }
}


bool PetriScene::evaluateGuard(const QString& guard) {
    if (guard.isEmpty()) return true;
    QString processed = preprocessCode(guard);
    qDebug() << "Guard after preprocessing:" << processed;

    static int guardCounter = 0;
    std::string scriptName = "guard_" + std::to_string(guardCounter++);

    std::string code = "bool __result = false;\nvoid __guard() {\n__result = ("
                     + processed.toStdString() + ");\n}\n__guard();";

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



void PetriScene::postEvent(const QString& name) {
    // Step 1: fire immediate event-gated transitions for this event
    for (Transition* t : transitions) {
        if (t->eventName != name) continue;
        if (t->isImmediate() && isFireable(t)) {
            fireTransition(t);
        }
    }

    // Step 2: schedule timed event-gated transitions for this event
    for (Transition* t : transitions) {
        if (t->eventName != name) continue;
        if (t->isImmediate()) continue;
        if (!isFireable(t)) continue;
        if (t->timer) continue;

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



void PetriScene::startRun() {
    currentMode = RunMode;
    for (QGraphicsItem* item : items())
        item->setFlag(QGraphicsItem::ItemIsMovable, false);
    rebuildCflatEnvironment();
}


void PetriScene::stopRun() {
    currentMode = EditMode;

    for (auto& i : inputs) {
        i.value   = "";
        i.defined = false;
    }

    //cancel all runnig timers
    for (Transition* t : transitions) {
        cancelTimer(t);
        t->becameFireable = QDateTime(); // reset to null
        t->setAvailability(Transition::Disabled);
    }

    for (Place* p : places)
        p->lastTokenChange = QDateTime();

    setTool(SelectTool);
    updateFireability();
}


void PetriScene::rebuildCflatEnvironment() {
    std::string code;
    for (auto& v : variables)
        code += v.type.toStdString() + " " + v.name.toStdString()
              + " = " + v.initialValue.toString().toStdString() + ";\n";
    env.load("vars", code.c_str());
}


void PetriScene::setInput(const QString& name, const QString& value) {
    if (currentMode != RunMode) return;
    for (auto& i : inputs) {
        if (i.name == name) {
            i.value   = value;
            i.defined = true;
            log("INPUT: " + name + " = \"" + value + "\"");
            break;
        }
    }
    postEvent(name);
}



QString PetriScene::preprocessCode(const QString& code) {
    QString result = code;

    // atoi(valueof("name")) -> integer value directly
    QRegularExpression ratoiv("atoi\\(valueof\\(\"([^\"]+)\"\\)\\)");
    QRegularExpressionMatch m;
    while ((m = ratoiv.match(result)).hasMatch()) {
        QString inputName = m.captured(1);
        int val = 0;
        for (auto& i : inputs)
            if (i.name == inputName) { val = (int)i.value.toDouble(); break; }
        result.replace(m.captured(0), QString::number(val));
    }

    // valueof("name") -> string value (as integer for Cflat)
    QRegularExpression rvo("valueof\\(\"([^\"]+)\"\\)");
    while ((m = rvo.match(result)).hasMatch()) {
        QString inputName = m.captured(1);
        QString val = "0";
        for (auto& i : inputs)
            if (i.name == inputName) { val = i.value; break; }
        result.replace(m.captured(0), val);
    }

    // defined("name") -> true/false
    QRegularExpression rdef("defined\\(\"([^\"]+)\"\\)");
    while ((m = rdef.match(result)).hasMatch()) {
        QString inputName = m.captured(1);
        bool def = false;
        for (auto& i : inputs)
            if (i.name == inputName) { def = i.defined; break; }
        result.replace(m.captured(0), def ? "true" : "false");
    }

    // tokens("place_name") -> int
    QRegularExpression rtok("tokens\\(\"([^\"]+)\"\\)");
    while ((m = rtok.match(result)).hasMatch()) {
        QString placeName = m.captured(1);
        int val = 0;
        for (auto* p : places)
            if (p->name == placeName) { val = p->getTokens(); break; }
        result.replace(m.captured(0), QString::number(val));
    }

    // elapsed("place_name") -> ms since last token change
    // elapsed("transition_name") -> ms since transition became fireable
    QRegularExpression relap("elapsed\\(\"([^\"]+)\"\\)");
    while ((m = relap.match(result)).hasMatch()) {
        QString arg = m.captured(1);
        int val = 0;

        bool found = false;
        for (auto* p : places) {
            if (p->name == arg) {
                val = p->lastTokenChange.msecsTo(QDateTime::currentDateTime());
                found = true;
                break;
            }
        }
        if (!found) {
            for (auto* t : transitions)
                if (t->name == arg && !t->becameFireable.isNull()) {
                    val = t->becameFireable.msecsTo(QDateTime::currentDateTime());
                    break;
                }
            }
        result.replace(m.captured(0), QString::number(val));
    }

    // now() -> ms since start
    QRegularExpression rnow("now\\(\\)");
    while ((m = rnow.match(result)).hasMatch()) {
        qint64 ms = startTime.msecsTo(QDateTime::currentDateTime());
        result.replace(m.captured(0), QString::number(ms));
    }

    return result;
}



void PetriScene::executeAction(const QString& action) {
    if (action.isEmpty()) return;

    QString processed = preprocessCode(action);

    // snapshot variables before execution to detect changes
    QMap<QString, QString> before;
    for (auto& v : variables) {
        Cflat::Value* val = env.getVariable(v.name.toStdString().c_str());
        if (!val) continue;
        if (v.type == "bool")        before[v.name] = CflatValueAs(val, bool) ? "true" : "false";
        else if (v.type == "int")    before[v.name] = QString::number(CflatValueAs(val, int));
        else if (v.type == "float")  before[v.name] = QString::number(CflatValueAs(val, float));
        else if (v.type == "double") before[v.name] = QString::number(CflatValueAs(val, double));
    }

    // find all output() calls and collect output names
    QStringList outputNames;
    QRegularExpression rout("output\\(\"([^\"]+)\"\\s*,\\s*");
    QRegularExpressionMatchIterator it = rout.globalMatch(processed);
    while (it.hasNext()) {
        auto m = it.next();
        QString name = m.captured(1);
        if (!outputNames.contains(name))
            outputNames.append(name);
    }

    // replace output("name", expr) with __out_name = (expr)
    QString replaced = processed;
    QRegularExpression routFull("output\\(\"([^\"]+)\"\\s*,\\s*(.+?)\\)\\s*;?");
    QRegularExpressionMatch m;
    while ((m = routFull.match(replaced)).hasMatch()) {
        QString name = m.captured(1);
        QString expr = m.captured(2);
        replaced.replace(m.captured(0),
            "__out_" + name + " = (" + expr + "); __out_" + name + "_set = 1;");
    }

    // build code - output declarations globally, action inside function
    QString globalDecls;
    for (const QString& name : outputNames) {
        globalDecls += "int __out_" + name + " = 0;\n";
        globalDecls += "int __out_" + name + "_set = 0;\n"; // flag as int
    }

    QString innerCode = replaced; // just the action, no declarations

    std::string scriptName = "action_" + std::to_string(actionCounter++);
    std::string wrapped = globalDecls.toStdString() +
                          "void __action() {\n" +
                          innerCode.toStdString() +
                          "\n}\n__action();";

    //qDebug() << "Final action code:" << QString::fromStdString(wrapped);

    if (!env.load(scriptName.c_str(), wrapped.c_str())) {
        qDebug() << "Action failed:" << env.getErrorMessage();
        return;
    }


    // detect variable changes
    for (auto& v : variables) {
        Cflat::Value* val = env.getVariable(v.name.toStdString().c_str());
        if (!val) continue;

        QString after;
        if (v.type == "bool")        after = CflatValueAs(val, bool) ? "true" : "false";
        else if (v.type == "int")    after = QString::number(CflatValueAs(val, int));
        else if (v.type == "float")  after = QString::number(CflatValueAs(val, float));
        else if (v.type == "double") after = QString::number(CflatValueAs(val, double));

        if (before.contains(v.name) && before[v.name] != after)
            log("VARIABLE: " + v.name + " " + before[v.name] + "→" + after, 2);
    }

    // read back output values
    for (const QString& name : outputNames) {
        Cflat::Value* flag = env.getVariable(("__out_" + name + "_set").toStdString().c_str());
        if (!flag || CflatValueAs(flag, int) == 0) continue; // branch was not taken

        Cflat::Value* val = env.getVariable(("__out_" + name).toStdString().c_str());
        if (!val) continue;
        QString strVal = QString::number(CflatValueAs(val, int));
        log("OUTPUT: " + name + " = " + strVal, 2);
        emit outputEmitted(name, strVal);
    }


}


bool PetriScene::isPlaceNameTaken(const QString& name, Place* exclude) {
    for (auto* p : places)
        if (p != exclude && p->name == name) return true;
    return false;
}

bool PetriScene::isTransitionNameTaken(const QString& name, Transition* exclude) {
    return false;
}


void PetriScene::updateFireability() {
    rebuildCflatEnvironment();
    if (currentMode == RunMode) return;

    for (Transition* t : transitions) {

        if (!(t->eventName.isEmpty())) {
            t->setAvailability(Transition::Disabled);
            continue;
        }

        bool fireable = isFireable(t);
        t->setAvailability(fireable ? Transition::Fireable : Transition::Disabled);
    }
}


void PetriScene::log(const QString& msg, int indent) {
    QString prefix = QString("  ").repeated(indent);
    qDebug().noquote() << prefix + msg;
}
