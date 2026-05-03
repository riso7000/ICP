#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QToolBar>
#include <QAction>
#include <QGraphicsSceneMouseEvent>


class PetriScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum Tool { SelectTool, PlaceTool, TransitionTool, ArcTool };
    Tool currentTool = SelectTool;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        if (currentTool == PlaceTool) {
            // Add a circle (Place)
            auto* item = addEllipse(-25, -25, 50, 50,
                                    QPen(Qt::black), QBrush(Qt::white));
            item->setPos(event->scenePos());
            item->setFlag(QGraphicsItem::ItemIsMovable);
            item->setFlag(QGraphicsItem::ItemIsSelectable);
        }
        else if (currentTool == TransitionTool) {
            // Add a rectangle (Transition)
            auto* item = addRect(-15, -40, 30, 80,
                                 QPen(Qt::black), QBrush(Qt::darkGray));
            item->setPos(event->scenePos());
            item->setFlag(QGraphicsItem::ItemIsMovable);
            item->setFlag(QGraphicsItem::ItemIsSelectable);
        }
        else {
            QGraphicsScene::mousePressEvent(event); // default (selection/drag)
        }
    }
};

#endif // MAIN_H

