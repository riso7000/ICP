#ifndef PETRISCENE_H
#define PETRISCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <vector>
#include "transition.h"
#include "place.h"


class PetriScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum Tool { SelectTool, PlaceTool, TransitionTool, ArcTool, DeleteTool };
    Tool currentTool;

    PetriScene(QObject* parent = 0);
    void setTool(Tool tool);

    bool isFireable(Transition* t);
    void fireTransition(Transition* t);
    void stabilize();
    void updateAvailability();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    int obj_id;
    std::vector<Transition*> transitions;
    std::vector<Place*> places;

    QGraphicsItem* arcSource; // null when no source selected yet
};

#endif // PETRISCENE_H
