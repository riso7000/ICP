#ifndef PETRISCENE_H
#define PETRISCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "place.h"

class PetriScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum Tool { SelectTool, PlaceTool, TransitionTool, ArcTool };
    Tool currentTool;

    PetriScene(QObject* parent = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    int obj_id;
};

#endif // PETRISCENE_H
