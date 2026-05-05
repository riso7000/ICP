#ifndef TRANSITION_H
#define TRANSITION_H


#include <QGraphicsRectItem>
#include <QPainter>
#include <QString>

class Transition : public QGraphicsRectItem
{
public:
    Transition(int id, QGraphicsItem* parent = 0);

    int getId() const { return id; }


protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

private:
    int id;



};




#endif // TRANSITION_H
