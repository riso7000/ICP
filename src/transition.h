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

#include "place.h"

class Arc;

class Transition : public QGraphicsRectItem
{
public:
    enum Availability_enum { Waiting, Disabled, Fireable };
    Availability_enum availability;

    Transition(QString name, int timer, QGraphicsItem* parent = 0);

    QString getName() const { return name; }
    void setAvailability(Availability_enum a) { availability = a; update(); }

    std::vector<Arc*> input_arcs;
    std::vector<Arc*> output_arcs;


    int delay_ms; // 0 means immediate (no timer)
    QTimer* timer; // null if not scheduled

    bool isImmediate() const { return delay_ms == 0; }
    void setDelay(int ms) { delay_ms = ms; }

    QString guard;
    QString eventName; // empty = fires freely; non-empty = only on this event
    QString outputName;

    QString action;
    QString name;
    QDateTime becameFireable;


protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    QRectF boundingRect() const override {
        QRectF base = QGraphicsRectItem::boundingRect();
        // expand upward to include the name label
        return base.adjusted(-60, -25, 60, 0);
    }

    QPainterPath shape() const override {
        QPainterPath path;
        path.addRect(rect());
        return path;
    }

private:


};




#endif // TRANSITION_H
