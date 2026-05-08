#include <QApplication>
#include <QMainWindow>
#include <QGraphicsView>
#include <QPainter>
#include <QToolBar>
#include <QAction>
#include "petriscene.h"

#include <QPlainTextEdit>
#include <QTime>
#include <QDockWidget>
#include <QDebug>

// Global pointer to your log widget
static QPlainTextEdit* globalLogWidget = nullptr;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if (!globalLogWidget) return;

    QString typeStr;
    switch (type) {
        case QtDebugMsg:    typeStr = "DEBUG"; break;
        case QtWarningMsg:  typeStr = "WARN "; break;
        case QtCriticalMsg: typeStr = "CRIT "; break;
        case QtFatalMsg:    typeStr = "FATAL"; break;
    }

    QString timestamp = QTime::currentTime().toString("hh:mm:ss");

    // Use invokeMethod to ensure this is thread-safe (in case Cflat runs on a thread)
    QMetaObject::invokeMethod(globalLogWidget, "appendPlainText",
        Qt::QueuedConnection,
        Q_ARG(QString, QString("[%1] %2: %3").arg(timestamp, typeStr, msg)));
}


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;
    PetriScene* scene = new PetriScene();
    scene->setSceneRect(0, 0, 1200, 800);

    QGraphicsView* view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    window.setCentralWidget(view);


    globalLogWidget = new QPlainTextEdit();
    globalLogWidget->setReadOnly(true);
    globalLogWidget->setBackgroundRole(QPalette::Base);

    // 3. Put the widget into a Dock so it's at the bottom
    QDockWidget* logDock = new QDockWidget("Execution Log", &window);
    logDock->setWidget(globalLogWidget);
    window.addDockWidget(Qt::BottomDockWidgetArea, logDock);

    // 4. Install the handler
    qInstallMessageHandler(myMessageOutput);




    QToolBar* toolbar = window.addToolBar("Tools");
    QAction* openAction  = toolbar->addAction("Open");
    QAction* saveAction  = toolbar->addAction("Save");
    QAction* placeAction  = toolbar->addAction("Place");
    QAction* transAction  = toolbar->addAction("Transition");
    QAction* arcAction  = toolbar->addAction("Arc");
    QAction* selectAction = toolbar->addAction("Select");
    QAction* deleteAction = toolbar->addAction("Delete");

    toolbar->setMovable(false);

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


    qDebug() << "Petri Net Editor Started.";

    window.resize(1280, 720);
    window.show();
    return app.exec();
}



