#include <QApplication>
#include <QMainWindow>
#include <QGraphicsView>
#include <QPainter>
#include <QToolBar>
#include <QAction>
#include "petriscene.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;
    PetriScene* scene = new PetriScene();
    scene->setSceneRect(0, 0, 1200, 800);

    QGraphicsView* view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    window.setCentralWidget(view);

    QToolBar* toolbar = window.addToolBar("Tools");
    QAction* placeAction  = toolbar->addAction("Place");
    QAction* transAction  = toolbar->addAction("Transition");
    QAction* arcAction  = toolbar->addAction("Arc");
    QAction* selectAction = toolbar->addAction("Select");
    QAction* deleteAction = toolbar->addAction("Delete");

    QObject::connect(placeAction, &QAction::triggered,
        [scene]{ scene->setTool(PetriScene::PlaceTool); });
    QObject::connect(transAction, &QAction::triggered,
        [scene]{ scene->setTool(PetriScene::TransitionTool); });
    QObject::connect(arcAction, &QAction::triggered,
        [scene]{scene->setTool(PetriScene::ArcTool); });
    QObject::connect(selectAction, &QAction::triggered,
        [scene]{ scene->setTool(PetriScene::SelectTool); });
    QObject::connect(deleteAction, &QAction::triggered,
        [scene]{ scene->setTool(PetriScene::DeleteTool); });


    QAction* runAction = toolbar->addAction("Run");

    QObject::connect(runAction, &QAction::triggered,
        [scene]{ scene->stabilize(); });

    window.resize(1280, 720);
    window.show();
    return app.exec();
}
