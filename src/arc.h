#ifndef ARC_H
#define ARC_H

#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include <QPainter>

class Arc : public QGraphicsLineItem {
public:
    Arc(QGraphicsItem* source, QGraphicsItem* dest, QGraphicsItem* parent = 0);

    QGraphicsItem* getSource() const { return source; }
    QGraphicsItem* getDest()   const { return dest; }
    void updatePosition(); // recalculates the line endpoints

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    QGraphicsItem *source;
    QGraphicsItem *dest;
};

#endif // ARC_H
