// ------------------------------
// petriscene.h
//
// Authors:
// Richard Henček (xhencer00)
// Šimon Varga (xvargas00)
// ------------------------------

#ifndef PETRISCENE_H
#define PETRISCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <vector>
#include "Cflat.h"

// Module headers
#include "transition.h"
#include "place.h"



class PetriScene : public QGraphicsScene {
    Q_OBJECT
public:
    struct NetInput {
        QString name;
        QString value;   // Current string value
        bool defined;    // Whether a value has ever been sent

        NetInput(const QString& name) : name(name), value(""), defined(false) {}
    };

    struct NetOutput {
        QString name;
        QString value;
    };

    struct NetVariable {
        QString name;
        QString type;
        QVariant initialValue;
    };

    enum Tool {
        SelectTool,
        PlaceTool,
        TransitionTool,
        ArcTool,
        DeleteTool
    };

    Tool currentTool;

    PetriScene(QObject* parent = 0);
    void setTool(Tool tool);
    void clearNet();

    void stabilize();
    void scheduleTimers();
    void onTimerExpired(Transition* t);

    Cflat::Environment* env;
    QString name;
    QString comment;

    // Public for opening from and saving to file
    std::vector<Transition*> transitions;
    std::vector<Place*> places;
    std::vector<NetInput> inputs;
    std::vector<NetOutput> outputs;
    std::vector<NetVariable> variables;

    bool running = false;

    enum Mode {
        EditMode,
        RunMode
    };

    Mode currentMode = EditMode;

    void setInput(const QString& name, const QString& value);
    void startRun();
    void stopRun();

    void rebuildCflatEnvironment();

    bool isPlaceNameTaken(const QString& name, Place* exclude);
    bool isTransitionNameTaken(const QString& name, Transition* exclude);

    void updateFireability();

signals:
    void outputEmitted(const QString& name, const QString& value);
    void variableChanged(const QString& name, const QString& value);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    bool isFireable(Transition* t);
    void fireTransition(Transition* t);
    void updateAvailability();
    void cancelTimer(Transition* t);
    bool evaluateGuard(const QString& guard);
    void postEvent(const QString& name);

    void executeAction(const QString& action);
    QString preprocessCode(const QString& code);
    void deleteItemAt(const QPointF& scenePos);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

private:
    int placeId;
    int transitionId;
    int arcId;
    int actionCounter = 0;
    int guardCounter = 0;

    QGraphicsItem* arcSource; // Null when no source selected yet
    QDateTime startTime;

    void log(const QString& msg, int indent = 0);
};

#endif // PETRISCENE_H
