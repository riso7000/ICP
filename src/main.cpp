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
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>

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

    // --- Log dock ---
    globalLogWidget = new QPlainTextEdit();
    globalLogWidget->setReadOnly(true);
    QDockWidget* logDock = new QDockWidget("Execution Log", &window);
    logDock->setWidget(globalLogWidget);
    window.addDockWidget(Qt::BottomDockWidgetArea, logDock);
    qInstallMessageHandler(myMessageOutput);

    // --- Input panel dock ---
    QWidget* inputPanel = new QWidget();
    QDockWidget* inputDock = new QDockWidget("Inputs", &window);
    inputDock->setWidget(inputPanel);
    window.addDockWidget(Qt::RightDockWidgetArea, inputDock);

    // Rebuilds the input panel buttons from current scene->inputs
    auto rebuildInputPanel = [=]() {
        // delete old layout and children
        delete inputPanel->layout();
        QList<QWidget*> children = inputPanel->findChildren<QWidget*>(
            QString(), Qt::FindDirectChildrenOnly);
        for (QWidget* w : children) delete w;

        QFormLayout* form = new QFormLayout(inputPanel);

        if (scene->currentMode == PetriScene::RunMode) {
            for (auto& inp : scene->inputs) {
                QPushButton* btn = new QPushButton(inp.name);
                QObject::connect(btn, &QPushButton::clicked, [scene, name=inp.name]{
                    scene->setInput(name);
                });
                form->addRow(btn);
            }
        } else {
            // Edit mode: show a placeholder or an "add input" UI
            form->addRow(new QLabel("Switch to Run mode to fire inputs."));
        }
    };

    // --- Toolbar ---
    QToolBar* toolbar = window.addToolBar("Tools");
    QAction* placeAction  = toolbar->addAction("Place");
    QAction* transAction  = toolbar->addAction("Transition");
    QAction* arcAction    = toolbar->addAction("Arc");
    QAction* selectAction = toolbar->addAction("Select");
    QAction* deleteAction = toolbar->addAction("Delete");

    QObject::connect(placeAction,  &QAction::triggered, [scene]{ scene->setTool(PetriScene::PlaceTool); });
    QObject::connect(transAction,  &QAction::triggered, [scene]{ scene->setTool(PetriScene::TransitionTool); });
    QObject::connect(arcAction,    &QAction::triggered, [scene]{ scene->setTool(PetriScene::ArcTool); });
    QObject::connect(selectAction, &QAction::triggered, [scene]{ scene->setTool(PetriScene::SelectTool); });
    QObject::connect(deleteAction, &QAction::triggered, [scene]{ scene->setTool(PetriScene::DeleteTool); });

    QAction* runAction  = toolbar->addAction("Run");
    QAction* stopAction = toolbar->addAction("Stop");
    stopAction->setEnabled(false);

    QObject::connect(runAction, &QAction::triggered, [=]{
        scene->startRun();
        runAction->setEnabled(false);
        stopAction->setEnabled(true);
        // disable edit tools
        for (QAction* a : {placeAction, transAction, arcAction, deleteAction})
            a->setEnabled(false);
        rebuildInputPanel();
    });

    QObject::connect(stopAction, &QAction::triggered, [=]{
        scene->stopRun();
        stopAction->setEnabled(false);
        runAction->setEnabled(true);
        for (QAction* a : {placeAction, transAction, arcAction, deleteAction})
            a->setEnabled(true);
        rebuildInputPanel();
    });

    rebuildInputPanel(); // initial build (edit mode placeholder)

    qDebug() << "Petri Net Editor Started.";
    window.resize(1280, 720);
    window.show();
    return app.exec();
}
