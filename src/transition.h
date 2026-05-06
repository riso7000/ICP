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
    enum Availability_enum { Available, Waiting, Disabled };

    Transition(int id, QGraphicsItem* parent = 0);

    int getId() const { return id; }
    void setAvailability(Availability_enum a) { availability = a; update(); }

    std::vector<Arc*> input_arcs;
    std::vector<Arc*> output_arcs;


    Availability_enum availability;


protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int id;




};




#endif // TRANSITION_H
