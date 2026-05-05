#ifndef PLACE_H
#define PLACE_H

#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QString>

class Place : public QGraphicsEllipseItem {
public:
    Place(int id, int initial_tokens, QGraphicsItem* parent = 0);

    int getId() const { return id; }
    int getTokens() const { return tokens; }
    void setTokens(int t) { tokens = t; update(); } // update() triggers repaint

private:
    int id;
    int tokens;

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
};

#endif // PLACE_H
