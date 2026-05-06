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
    Transition(int id, QGraphicsItem* parent = 0);

    int getId() const { return id; }

    std::vector<Arc*> input_arcs;
    std::vector<Arc*> output_arcs;


protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int id;




};




#endif // TRANSITION_H
