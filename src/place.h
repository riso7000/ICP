// ------------------------------
// place.h
//
// Author:
// Richard Henček (xhencer00)
// ------------------------------

#ifndef PLACE_H
#define PLACE_H

#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QString>
#include <QInputDialog>
#include <QDateTime>



class Place : public QGraphicsEllipseItem {
public:
    Place(QString name, int initial_tokens, QGraphicsItem* parent = 0);
    QString getName() const { return name; }
    int getTokens() const { return tokens; }
    int getInitTokens() const { return init_tokens; }
    void setTokens(int t);

    QString name;
    QDateTime lastTokenChange;
    int tokens;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    QRectF boundingRect() const override {
        QRectF base = QGraphicsEllipseItem::boundingRect();
        // Expand upward to include the name label
        return base.adjusted(-60, -25, 60, 0);
    }

    QPainterPath shape() const override {
        QPainterPath path;
        path.addEllipse(QGraphicsEllipseItem::rect());
        return path;
    }

private:
    int init_tokens;
};

#endif // PLACE_H
