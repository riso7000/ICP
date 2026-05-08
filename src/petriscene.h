#ifndef PETRISCENE_H
#define PETRISCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <vector>
#include "transition.h"
#include "place.h"

#include "Cflat.h"


struct NetInput {
    QString name;

};

struct NetOutput {
    QString name;

};

struct NetVariable {
    QString name;
    QString type;
    QVariant initialValue;
};


class PetriScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum Tool { SelectTool, PlaceTool, TransitionTool, ArcTool, DeleteTool };
    Tool currentTool;

    PetriScene(QObject* parent = 0);
    void setTool(Tool tool);
    void setActive(bool value);
    void setSensorValue(int value);

    void stabilize();
    void scheduleTimers();
    void onTimerExpired(Transition* t);

    Cflat::Environment env;
    bool active;
    int sensorValue;
    QString name;
    QString comment;
    // Public for opening from and saving to file
    std::vector<Transition*> transitions;
    std::vector<Place*> places;



    std::vector<NetInput>    inputs;
    std::vector<NetOutput>   outputs;
    std::vector<NetVariable> variables;
    bool running = false;

    enum Mode { EditMode, RunMode };
    Mode currentMode = EditMode;

    void setInput(const QString& name);
    void startRun();
    void stopRun();


signals:
    void outputEmitted(const QString& name);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    bool isFireable(Transition* t);
    void fireTransition(Transition* t);
    void updateAvailability();
    void cancelTimer(Transition* t);
    bool evaluateGuard(const QString& guard);
    void postEvent(const QString& name);
    void rebuildCflatEnvironment();



private:
    int obj_id;

    QGraphicsItem* arcSource; // null when no source selected yet
};

#endif // PETRISCENE_H
