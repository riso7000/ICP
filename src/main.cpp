// main.cpp
#include "main.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;
    auto* scene = new PetriScene();
    scene->setSceneRect(0, 0, 1200, 800);

    auto* view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    window.setCentralWidget(view);

    // Toolbar
    auto* toolbar = window.addToolBar("Tools");
    auto* placeAction = toolbar->addAction("Place (O)");
    auto* transAction = toolbar->addAction("Transition (|)");
    auto* selectAction = toolbar->addAction("Select");

    QObject::connect(placeAction, &QAction::triggered,
        [scene]{ scene->currentTool = PetriScene::PlaceTool; });
    QObject::connect(transAction, &QAction::triggered,
        [scene]{ scene->currentTool = PetriScene::TransitionTool; });
    QObject::connect(selectAction, &QAction::triggered,
        [scene]{ scene->currentTool = PetriScene::SelectTool; });

    window.resize(1280, 720);
    window.show();
    return app.exec();
}
