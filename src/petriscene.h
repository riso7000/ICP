#ifndef PETRISCENE_H
#define PETRISCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <vector>
#include "transition.h"
#include "place.h"

#include "Cflat.h"



class PetriScene : public QGraphicsScene {
    Q_OBJECT
public:
    struct NetInput {
        QString name;
        QString value;   // current string value
        bool defined;    // whether a value has ever been sent
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
    enum Tool { SelectTool, PlaceTool, TransitionTool, ArcTool, DeleteTool };
    Tool currentTool;

    PetriScene(QObject* parent = 0);
    void setTool(Tool tool);

    void stabilize();
    void scheduleTimers();
    void onTimerExpired(Transition* t);

    Cflat::Environment env;
    QString name;
    QString comment;
    // Public for opening from and saving to file
    std::vector<Transition*> transitions;
    std::vector<Place*> places;



    std::vector<NetInput> inputs;
    std::vector<NetOutput> outputs;
    std::vector<NetVariable> variables;
    bool running = false;

    enum Mode { EditMode, RunMode };
    Mode currentMode = EditMode;

    void setInput(const QString& name, const QString& value);
    void startRun();
    void stopRun();

    void rebuildCflatEnvironment();
    QString preprocessGuard(const QString& guard);
    void executeAction(const QString& action);


signals:
    void outputEmitted(const QString& name, const QString& value);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    bool isFireable(Transition* t);
    void fireTransition(Transition* t);
    void updateAvailability();
    void cancelTimer(Transition* t);
    bool evaluateGuard(const QString& guard);
    void postEvent(const QString& name);




private:
    int placeId;
    int transitionId;
    int arcId;
    int actionCounter = 0;

    QGraphicsItem* arcSource; // null when no source selected yet
};

#endif // PETRISCENE_H
