#include <QApplication>
#include <QMainWindow>
#include <QGraphicsView>
#include <QPainter>
#include <QToolBar>
#include <QAction>
#include "petriscene.h"
#include "net_def_io.hpp"

#include <QPlainTextEdit>
#include <QTime>
#include <QDockWidget>
#include <QDebug>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>

// Global pointer to your log widget
static QPlainTextEdit* globalLogWidget = nullptr;
std::function<void()> rebuildInputPanel;

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
    rebuildInputPanel = [=]() {
        // delete old layout and children
        delete inputPanel->layout();
        QList<QWidget*> children = inputPanel->findChildren<QWidget*>(
            QString(), Qt::FindDirectChildrenOnly);
        for (QWidget* w : children) delete w;

        QFormLayout* form = new QFormLayout(inputPanel);

        if (scene->currentMode == PetriScene::RunMode) {
            for (auto& inp : scene->inputs) {
                QHBoxLayout* row = new QHBoxLayout();
                QLineEdit* edit = new QLineEdit(inp.value);
                QPushButton* btn = new QPushButton("Send");
                QObject::connect(btn, &QPushButton::clicked, [scene, edit, name=inp.name]{
                    scene->setInput(name, edit->text());
                });
                row->addWidget(edit);
                row->addWidget(btn);
                QWidget* rowWidget = new QWidget();
                rowWidget->setLayout(row);
                form->addRow(inp.name, rowWidget);
            }
        }
        else {
            // --- Inputs section ---
            form->addRow(new QLabel("<b>Inputs</b>"));
            for (int i = 0; i < (int)scene->inputs.size(); i++) {
                QHBoxLayout* row = new QHBoxLayout();
                row->addWidget(new QLabel(scene->inputs[i].name));
                QPushButton* del = new QPushButton("x");
                del->setFixedWidth(24);
                auto rebuild = rebuildInputPanel;
                QObject::connect(del, &QPushButton::clicked, [scene, i, rebuild](){
                    scene->inputs.erase(scene->inputs.begin() + i);
                    scene->rebuildCflatEnvironment();
                    rebuild();
                });
                row->addWidget(del);
                QWidget* rowWidget = new QWidget();
                rowWidget->setLayout(row);
                form->addRow(rowWidget);
            }
            QPushButton* addInput = new QPushButton("+ Add Input");
            QObject::connect(addInput, &QPushButton::clicked, [=](){
                bool ok;
                QString name = QInputDialog::getText(nullptr, "Add Input", "Name:", QLineEdit::Normal, "", &ok);
                if (ok && !name.isEmpty()) {
                    scene->inputs.push_back({name});
                    auto rebuild = rebuildInputPanel;
                    rebuild();
                }
            });
            form->addRow(addInput);

            // --- Variables section ---
            form->addRow(new QLabel("<b>Variables</b>"));
            for (int i = 0; i < (int)scene->variables.size(); i++) {
                auto& v = scene->variables[i];
                QHBoxLayout* row = new QHBoxLayout();
                row->addWidget(new QLabel(v.type + " " + v.name + " = " + v.initialValue.toString()));
                QPushButton* del = new QPushButton("x");
                del->setFixedWidth(24);
                auto rebuild = rebuildInputPanel;
                QObject::connect(del, &QPushButton::clicked, [scene, i, rebuild](){
                    scene->variables.erase(scene->variables.begin() + i);
                    scene->rebuildCflatEnvironment();
                    rebuild();
                });
                row->addWidget(del);
                QWidget* rowWidget = new QWidget();
                rowWidget->setLayout(row);
                form->addRow(rowWidget);
            }
            QPushButton* addVar = new QPushButton("+ Add Variable");
            QObject::connect(addVar, &QPushButton::clicked, [=](){
                // small dialog to pick type, name, initial value
                QDialog dlg;
                dlg.setWindowTitle("Add Variable");
                QFormLayout* f = new QFormLayout(&dlg);

                QComboBox* typeBox = new QComboBox();
                typeBox->addItems({"bool", "int"});
                QLineEdit* nameEdit = new QLineEdit();
                QLineEdit* valueEdit = new QLineEdit("0");
                QDialogButtonBox* btns = new QDialogButtonBox(
                    QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
                QObject::connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
                QObject::connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

                f->addRow("Type:",          typeBox);
                f->addRow("Name:",          nameEdit);
                f->addRow("Initial value:", valueEdit);
                f->addRow(btns);

                if (dlg.exec() == QDialog::Accepted && !nameEdit->text().isEmpty()) {
                    PetriScene::NetVariable var;
                    var.type         = typeBox->currentText();
                    var.name         = nameEdit->text();
                    var.initialValue = valueEdit->text();
                    scene->variables.push_back(var);
                    auto rebuild = rebuildInputPanel;
                    rebuild();
                }
            });
            form->addRow(addVar);
        }
    };

    // --- Toolbar ---
    QToolBar* toolbar = window.addToolBar("Tools");
    QAction* openAction  = toolbar->addAction("Open");
    QAction* saveAction  = toolbar->addAction("Save");
    QAction* placeAction  = toolbar->addAction("Place");
    QAction* transAction  = toolbar->addAction("Transition");
    QAction* arcAction    = toolbar->addAction("Arc");
    QAction* selectAction = toolbar->addAction("Select");
    QAction* deleteAction = toolbar->addAction("Delete");

    QObject::connect(openAction, &QAction::triggered, [=]() {
        QString filePath = QFileDialog::getOpenFileName(
            toolbar,
            "Open Petri Net",
            "",
            FILE_IO_FILETYPES
        );

        if (!filePath.isEmpty()) {
            read_netdef(filePath, *scene);
            rebuildInputPanel();
        }
    });

    QObject::connect(saveAction, &QAction::triggered, [=]() {
        QString filePath = QFileDialog::getSaveFileName(
            toolbar,
            "Save Petri Net",
            "",
            FILE_IO_FILETYPES
        );

        if (!filePath.isEmpty()) {
            write_netdef(filePath, *scene);
        }
    });

    QObject::connect(placeAction,  &QAction::triggered, [scene]{ scene->setTool(PetriScene::PlaceTool); });
    QObject::connect(transAction,  &QAction::triggered, [scene]{ scene->setTool(PetriScene::TransitionTool); });
    QObject::connect(arcAction,    &QAction::triggered, [scene]{ scene->setTool(PetriScene::ArcTool); });
    QObject::connect(selectAction, &QAction::triggered, [scene]{ scene->setTool(PetriScene::SelectTool); });
    QObject::connect(deleteAction, &QAction::triggered, [scene]{ scene->setTool(PetriScene::DeleteTool); });

    QAction* runAction  = toolbar->addAction("Run");
    QAction* stopAction = toolbar->addAction("Stop");
    stopAction->setEnabled(false);

    toolbar->setMovable(false);



    QObject::connect(runAction, &QAction::triggered, [=]{
        scene->startRun();
        runAction->setEnabled(false);
        stopAction->setEnabled(true);
        // disable edit tools
        for (QAction* a : {placeAction, transAction, arcAction, deleteAction})
            a->setEnabled(false);
        auto rebuild = rebuildInputPanel;
        rebuild();
    });

    QObject::connect(stopAction, &QAction::triggered, [=]{
        scene->stopRun();
        stopAction->setEnabled(false);
        runAction->setEnabled(true);
        for (QAction* a : {placeAction, transAction, arcAction, deleteAction})
            a->setEnabled(true);
        auto rebuild = rebuildInputPanel;
        rebuild();
    });

    rebuildInputPanel(); // initial build (edit mode placeholder)

    qDebug() << "Petri Net Editor Started.";
    window.resize(1280, 720);
    window.show();
    return app.exec();
}
