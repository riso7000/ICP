// ------------------------------
// arc.h
//
// Author:
// Richard Henček (xhencer00)
// ------------------------------

#ifndef ARC_H
#define ARC_H

#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include <QPainter>
#include <QInputDialog>
#include <QPainterPath>



static const double ARROW_SIZE = 12.0; // Size of the arrowhead

class Arc : public QGraphicsLineItem {
public:
    Arc(QGraphicsItem* source, QGraphicsItem* dest, int weight, QGraphicsItem* parent = 0);

    QGraphicsItem* getSource() const { return source; }
    QGraphicsItem* getDest()   const { return dest; }
    int getWeight() const { return weight; }
    void updatePosition(); // Recalculates the line endpoints

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void setWeight(int w) { weight = w; update(); } // update() triggers repaint

private:
    QGraphicsItem *source;
    QGraphicsItem *dest;
    int weight;
};

#endif // ARC_H
