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
    enum Availability_enum { Waiting, Disabled };
    Availability_enum availability;

    Transition(int id, int timer, QGraphicsItem* parent = 0);

    int getId() const { return id; }
    void setAvailability(Availability_enum a) { availability = a; update(); }

    std::vector<Arc*> input_arcs;
    std::vector<Arc*> output_arcs;


    int delay_ms; // 0 means immediate (no timer)
    QTimer* timer; // null if not scheduled

    bool isImmediate() const { return delay_ms == 0; }
    void setDelay(int ms) { delay_ms = ms; }

    QString guard;
    QString eventName; // empty = fires freely; non-empty = only on this event


protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    int id;




};




#endif // TRANSITION_H
