// arc.cpp
#define _USE_MATH_DEFINES
#include <cmath>
#include "arc.h"
#include "petriscene.h"
#include <QPointF>

static const double ARROW_SIZE = 12.0; // size of the arrowhead

Arc::Arc(QGraphicsItem* source, QGraphicsItem* dest, int weight, QGraphicsItem* parent)
    : QGraphicsLineItem(parent),

      source(source),
      dest(dest),
      weight(weight)
{
    setPen(QPen(Qt::black, 2));
    setZValue(-1);
    updatePosition();
}

void Arc::updatePosition() {
    QPointF p1 = source->scenePos();
    QPointF p2 = dest->scenePos();
    setLine(QLineF(p1, p2));
}

QRectF Arc::boundingRect() const {
    // get the base bounding rect of the line
    QRectF base = QGraphicsLineItem::boundingRect();

    // expand it by enough to cover the arrowhead and text label
    return base.adjusted(-ARROW_SIZE - 20, -ARROW_SIZE - 20,
                          ARROW_SIZE + 20,  ARROW_SIZE + 20);
}

void Arc::paint(QPainter* painter,
                const QStyleOptionGraphicsItem* option,
                QWidget* widget)
{
    QLineF l = line();
    if (l.length() < 1.0) return;

    // find the edge of the destination item
    QPointF tip = l.p2();
    QPainterPath destShape = dest->shape();

    for (int i = 0; i < (int)l.length(); ++i) {
        QPointF p = l.pointAt(i / l.length());
        QPointF localP = dest->mapFromScene(p);
        if (destShape.contains(localP)) {
            tip = p;
            break;
        }
    }

    // draw the line manually from source to tip (no setLine!)
    painter->setPen(QPen(Qt::black, 2));
    painter->drawLine(l.p1(), tip);

    // arrowhead
    double angle = std::atan2(l.dy(), l.dx());

    QPointF base1(
        tip.x() - ARROW_SIZE * std::cos(angle - M_PI / 6),
        tip.y() - ARROW_SIZE * std::sin(angle - M_PI / 6)
    );
    QPointF base2(
        tip.x() - ARROW_SIZE * std::cos(angle + M_PI / 6),
        tip.y() - ARROW_SIZE * std::sin(angle + M_PI / 6)
    );

    QPolygonF arrowHead;
    arrowHead << tip << base1 << base2;

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::black);
    painter->drawPolygon(arrowHead);

    // weight label
    QPointF mid(
        (l.p1().x() + l.p2().x()) / 2.0,
        (l.p1().y() + l.p2().y()) / 2.0
    );
    painter->setPen(Qt::black);
    painter->drawText(mid + QPointF(-10, -5), QString::number(weight));
}



void Arc::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {

    PetriScene* petriScene = dynamic_cast<PetriScene*>(scene());
    if (!petriScene || petriScene->currentMode == PetriScene::RunMode) return;
    if (petriScene->currentTool != PetriScene::SelectTool) return;

    bool ok;
    int value = QInputDialog::getInt(
        NULL,            // parent widget
        "Edit Arc",   // dialog title
        "Weight:",       // label
        weight,          // current value
        1,               // minimum
        999,             // maximum
        1,               // step
        &ok
    );

    if (ok) {
        setWeight(value);
    }

    QGraphicsLineItem::mouseDoubleClickEvent(event);
}


QPainterPath Arc::shape() const {
    QPainterPath path;

    // create a stroked version of the line with a wider width
    QPainterPathStroker stroker;
    stroker.setWidth(20); // hitbox width in pixels, adjust to taste

    path.moveTo(line().p1());
    path.lineTo(line().p2());

    return stroker.createStroke(path);
}
