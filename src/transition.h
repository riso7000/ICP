#ifndef TRANSITION_H
#define TRANSITION_H


#include <QGraphicsRectItem>
#include <QPainter>
#include <QString>

#include "place.h"

class Transition : public QGraphicsRectItem
{
public:
    Transition(int id, QGraphicsItem* parent = 0);

    int getId() const { return id; }

    std::vector<Place*> input_places {};
    std::vector<Place*> output_places {};


protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int id;




};




#endif // TRANSITION_H
