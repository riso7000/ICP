#ifndef PLACE_H
#define PLACE_H

#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QString>
#include <QInputDialog>

class Place : public QGraphicsEllipseItem {
public:
    Place(int id, int initial_tokens, QGraphicsItem* parent = 0);

    int getId() const { return id; }
    int getTokens() const { return tokens; }
    int getInitTokens() const { return init_tokens; }
    void setTokens(int t) { tokens = t; update(); } // update() triggers repaint


protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    int id;
    int tokens;
    int init_tokens;

};

#endif // PLACE_H
