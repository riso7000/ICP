// ------------------------------
// transition.h
//
// Author:
// Richard Henček (xhencer00)
// ------------------------------

#ifndef TRANSITION_H
#define TRANSITION_H

#include <QGraphicsRectItem>
#include <QPainter>
#include <QString>

// Module headers
#include "place.h"



class Arc;

class Transition : public QGraphicsRectItem {
public:
    enum Availability_enum {
        Waiting,
        Disabled,
        Fireable
    };

    Availability_enum availability;

    Transition(QString name, int timer, QGraphicsItem* parent = 0);

    QString getName() const { return name; }
    void setAvailability(Availability_enum a) { availability = a; update(); }

    std::vector<Arc*> inputArcs;
    std::vector<Arc*> outputArcs;

    int delayMs; // 0 means immediate (no timer)
    QTimer* timer; // Null if not scheduled

    bool isImmediate() const { return delayMs == 0; }
    void setDelay(int ms) { delayMs = ms; }

    QString guard;
    QString eventName; // Empty = fires freely; non-empty = only on this event
    QString outputName;

    QString action;
    QString name;
    QDateTime becameFireable;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    QRectF boundingRect() const override {
        QRectF base = QGraphicsRectItem::boundingRect();
        // Expand upward to include the name label
        return base.adjusted(-60, -25, 60, 0);
    }

    QPainterPath shape() const override {
        QPainterPath path;
        path.addRect(rect());
        return path;
    }
};

#endif // TRANSITION_H
