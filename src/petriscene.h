#ifndef PETRISCENE_H
#define PETRISCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class PetriScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum Tool { SelectTool, PlaceTool, TransitionTool, ArcTool, DeleteTool };
    Tool currentTool;

    PetriScene(QObject* parent = 0);
    void setTool(Tool tool);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    int obj_id;

    QGraphicsItem* arcSource; // null when no source selected yet
};

#endif // PETRISCENE_H
