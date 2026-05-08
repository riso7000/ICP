#ifndef PETRISCENE_H
#define PETRISCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <vector>
#include "transition.h"
#include "place.h"

#include "Cflat.h"


class PetriScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum Tool { SelectTool, PlaceTool, TransitionTool, ArcTool, DeleteTool };
    Tool currentTool;

    PetriScene(QObject* parent = 0);
    void setTool(Tool tool);
    void setActive(bool value);
    void setSensorValue(int value);

    void stabilize();
    void scheduleTimers();
    void onTimerExpired(Transition* t);

    Cflat::Environment env;
    bool active;
    int sensorValue;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    bool isFireable(Transition* t);
    void fireTransition(Transition* t);
    void updateAvailability();
    void cancelTimer(Transition* t);
    bool evaluateGuard(const QString& guard);

private:
    int obj_id;
    std::vector<Transition*> transitions;
    std::vector<Place*> places;

    QGraphicsItem* arcSource; // null when no source selected yet
};

#endif // PETRISCENE_H
