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
std::function<void()> rebuildPanel;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if (!globalLogWidget) return;

    QString typeStr;
    switch (type) {
        case QtDebugMsg:    typeStr = ""; break;
        case QtWarningMsg:  typeStr = "WARN "; break;
        case QtCriticalMsg: typeStr = "CRIT "; break;
        case QtFatalMsg:    typeStr = "FATAL"; break;
    }

    QString timestamp = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");

    // Use invokeMethod to ensure this is thread-safe (in case Cflat runs on a thread)
    QMetaObject::invokeMethod(globalLogWidget, "appendPlainText",
        Qt::QueuedConnection,
        Q_ARG(QString, QString("[%1] %2: %3").arg(timestamp, typeStr, msg)));
}


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;
    PetriScene* scene = new PetriScene();
    scene->setSceneRect(0, 0, 1600, 1200);

    QGraphicsView* view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    window.setCentralWidget(view);

    app.setWindowIcon(QIcon(":/res/program.png"));

    // --- Log dock ---
    globalLogWidget = new QPlainTextEdit();
    globalLogWidget->setReadOnly(true);
    QDockWidget* logDock = new QDockWidget("Execution Log", &window);
    logDock->setWidget(globalLogWidget);
    logDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    window.addDockWidget(Qt::BottomDockWidgetArea, logDock);
    qInstallMessageHandler(myMessageOutput);

    // --- Panel dock ---
    QWidget* panel = new QWidget();
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(panel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setMinimumWidth(200);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QDockWidget* panelDock = new QDockWidget("Communication", &window);
    panelDock->setWidget(scrollArea);
    panelDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    panel->setMinimumWidth(150);
    window.addDockWidget(Qt::RightDockWidgetArea, panelDock);

    // Rebuilds the panel buttons
    rebuildPanel = [=]() {

        if (scene->currentMode == PetriScene::RunMode) {
            panelDock->setMinimumWidth(300);
            panelDock->setMaximumWidth(QWIDGETSIZE_MAX);
        } else {
            panelDock->setMinimumWidth(200);
            panelDock->setMaximumWidth(QWIDGETSIZE_MAX);
        }
        // delete old layout and children
        delete panel->layout();
        QList<QWidget*> children = panel->findChildren<QWidget*>(
            QString(), Qt::FindDirectChildrenOnly);
        for (QWidget* w : children) delete w;

        QFormLayout* form = new QFormLayout(panel);


        if (scene->currentMode == PetriScene::RunMode) {
            form->setVerticalSpacing(5);

            // --- Inputs ---
            form->addRow(new QLabel("<b>Inputs</b>"));
            for (auto& inp : scene->inputs) {
                QHBoxLayout* row = new QHBoxLayout();
                row->setContentsMargins(0, 0, 0, 0);
                row->setSpacing(8);

                QLabel* nameLabel = new QLabel(inp.name);
                nameLabel->setMinimumWidth(60);
                row->addWidget(nameLabel);

                QLineEdit* edit = new QLineEdit(inp.value);
                edit->setMaximumWidth(150);
                QPushButton* btn = new QPushButton("Send");
                btn->setMaximumWidth(50);

                QObject::connect(btn, &QPushButton::clicked, [scene, edit, name = inp.name]{
                    QString currentValue = edit->text();
                    scene->setInput(name, currentValue);
                });

                row->addWidget(edit);
                row->addWidget(btn);

                form->addRow(row);
            }
            form->setVerticalSpacing(10);


            // --- Variables ---
            form->addRow(new QLabel("<b>Variables</b>"));
            for (auto& v : scene->variables) {
                QLabel* valueLabel = new QLabel(v.type + " " + v.name + " = " + v.initialValue.toString());
                form->addRow(valueLabel);

                // update label when any transition fires (variable may have changed)
                QObject::connect(scene, &PetriScene::variableChanged,
                    valueLabel,
                    [valueLabel, name=v.name, type=v.type](const QString& n, const QString& val){
                        if (n == name)
                            valueLabel->setText(type + " " + name + " = " + val);
                    },
                    Qt::QueuedConnection);
            }

            // --- Outputs ---
            form->addRow(new QLabel("<b>Outputs</b>"));
            for (auto& out : scene->outputs) {
            QLabel* fullLineLabel = new QLabel(out.name + " = undefined");
            form->addRow(fullLineLabel);

            QObject::connect(scene, &PetriScene::outputEmitted,
                fullLineLabel,
                [fullLineLabel, name = out.name](const QString& n, const QString& val) {
                    if (n == name) fullLineLabel->setText(name + " = " + val);
                },
                Qt::QueuedConnection);
            }
        }
        else {
            // --- Inputs section ---
            form->addRow(new QLabel("<b>Inputs</b>"));
            for (int i = 0; i < (int)scene->inputs.size(); i++) {
                QHBoxLayout* row = new QHBoxLayout();
                row->setContentsMargins(0, 2, 0, 0);
                row->setSpacing(5);
                row->addWidget(new QLabel(scene->inputs[i].name));
                QPushButton* del = new QPushButton("x");
                del->setFixedWidth(24);
                auto rebuild = rebuildPanel;
                QObject::connect(del, &QPushButton::clicked, [scene, i, rebuild](){
                    scene->inputs.erase(scene->inputs.begin() + i);
                    scene->rebuildCflatEnvironment();
                    rebuild();
                });
                row->addWidget(del);
                form->addRow(row);
            }
            QPushButton* addInput = new QPushButton("+ Add Input");
            QObject::connect(addInput, &QPushButton::clicked, [=](){
                bool ok;
                QString name = QInputDialog::getText(nullptr, "Add Input", "Name:", QLineEdit::Normal, "", &ok);
                if (ok && !name.isEmpty()) {
                    scene->inputs.push_back(PetriScene::NetInput(name));
                    auto rebuild = rebuildPanel;
                    rebuild();
                }
            });
            form->addRow(addInput);

            // --- Variables section ---
            form->addRow(new QLabel("<b>Variables</b>"));
            for (int i = 0; i < (int)scene->variables.size(); i++) {
                auto& v = scene->variables[i];
                QHBoxLayout* row = new QHBoxLayout();
                row->setContentsMargins(0, 2, 0, 0);
                row->setSpacing(5);
                row->addWidget(new QLabel(v.type + " " + v.name + " = " + v.initialValue.toString()));
                QPushButton* del = new QPushButton("x");
                del->setFixedWidth(24);
                auto rebuild = rebuildPanel;
                QObject::connect(del, &QPushButton::clicked, [scene, i, rebuild](){
                    scene->variables.erase(scene->variables.begin() + i);
                    scene->rebuildCflatEnvironment();
                    rebuild();
                });
                row->addWidget(del);
                form->addRow(row);
            }
            QPushButton* addVar = new QPushButton("+ Add Variable");
            QObject::connect(addVar, &QPushButton::clicked, [=](){
                QDialog dlg;
                dlg.setWindowTitle("Add Variable");
                QFormLayout* f = new QFormLayout(&dlg);

                QComboBox* typeBox = new QComboBox();
                typeBox->addItems({"int", "float", "char"});
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
                nameEdit->setFocus();

                if (dlg.exec() == QDialog::Accepted && !nameEdit->text().isEmpty()) {
                    PetriScene::NetVariable var;
                    var.type         = typeBox->currentText();
                    var.name         = nameEdit->text();
                    var.initialValue = valueEdit->text();
                    scene->variables.push_back(var);
                    auto rebuild = rebuildPanel;
                    rebuild();
                }
            });
            form->addRow(addVar);
            scene->updateFireability();

            // --- Outputs section ---
            form->addRow(new QLabel("<b>Outputs</b>"));
            for (int i = 0; i < (int)scene->outputs.size(); i++) {
                QHBoxLayout* row = new QHBoxLayout();
                row->setContentsMargins(0, 2, 0, 0);
                row->setSpacing(5);
                row->addWidget(new QLabel(scene->outputs[i].name));
                QPushButton* del = new QPushButton("x");
                del->setFixedWidth(24);
                auto rebuild = rebuildPanel;
                QObject::connect(del, &QPushButton::clicked, [scene, i, rebuild](){
                    scene->outputs.erase(scene->outputs.begin() + i);
                    scene->rebuildCflatEnvironment();
                    rebuild();
                });
                row->addWidget(del);
                form->addRow(row);
            }
            QPushButton* addOutput = new QPushButton("+ Add Output");
            QObject::connect(addOutput, &QPushButton::clicked, [=](){
                bool ok;
                QString name = QInputDialog::getText(nullptr, "Add Output", "Name:", QLineEdit::Normal, "", &ok);
                if (ok && !name.isEmpty()) {
                    scene->outputs.push_back({name});
                    auto rebuild = rebuildPanel;
                    rebuild();
                }
            });
            form->addRow(addOutput);
        }
    };

    // --- Toolbar ---
    QToolBar* toolbar = window.addToolBar("Tools");
    QAction* openAction  = toolbar->addAction(QIcon(":/res/open.png"), "Open");
    QAction* saveAction  = toolbar->addAction(QIcon(":/res/save.png"), "Save");
    QAction* clearAction = toolbar->addAction(QIcon(":/res/clear-alt.png"), "Clear");
    QAction* selectAction = toolbar->addAction(QIcon(":/res/select.png"), "Select");
    QAction* deleteAction = toolbar->addAction(QIcon(":/res/delete-alt.png"), "Delete");
    QAction* placeAction  = toolbar->addAction(QIcon(":/res/place.png"), "Place");
    QAction* transAction  = toolbar->addAction(QIcon(":/res/transition.png"), "Transition");
    QAction* arcAction    = toolbar->addAction(QIcon(":/res/arc.png"), "Arc");
    QAction* runAction  = toolbar->addAction(QIcon(":/res/run.png"), "Run");
    QAction* stopAction = toolbar->addAction(QIcon(":/res/stop.png"), "Stop");
    stopAction->setEnabled(false);

    QObject::connect(openAction, &QAction::triggered, [=]() {
        QString filePath = QFileDialog::getOpenFileName(
            toolbar,
            "Open Petri Net",
            "",
            FILE_IO_FILETYPES
        );

        if (!filePath.isEmpty()) {
            scene->clearNet();
            read_netdef(filePath, *scene);
            rebuildPanel();
        }

        if (stopAction->isEnabled()) {
            stopAction->trigger();
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

    QObject::connect(clearAction, &QAction::triggered, [=]() {
        scene->clearNet();
    });


    QActionGroup* toolGroup = new QActionGroup(&window);

    QAction* toolActions[] = {placeAction, transAction, arcAction, selectAction, deleteAction};

    for (QAction* action : toolActions) {
        action->setCheckable(true); // Make it stay "pressed" when clicked
        toolGroup->addAction(action); // Add to the group for exclusivity
    }

    selectAction->setChecked(true);


    QObject::connect(placeAction,  &QAction::triggered, [scene]{ scene->setTool(PetriScene::PlaceTool); });
    QObject::connect(transAction,  &QAction::triggered, [scene]{ scene->setTool(PetriScene::TransitionTool); });
    QObject::connect(arcAction,    &QAction::triggered, [scene]{ scene->setTool(PetriScene::ArcTool); });
    QObject::connect(selectAction, &QAction::triggered, [scene]{ scene->setTool(PetriScene::SelectTool); });
    QObject::connect(deleteAction, &QAction::triggered, [scene]{ scene->setTool(PetriScene::DeleteTool); });



    toolbar->setMovable(false);



    QObject::connect(runAction, &QAction::triggered, [=]{
        runAction->setEnabled(false);
        stopAction->setEnabled(true);
        for (QAction* a : {placeAction, transAction, arcAction, deleteAction})
            a->setEnabled(false);
        selectAction->setChecked(true);
        scene->startRun();
        auto rebuild = rebuildPanel;
        rebuild();
        scene->stabilize();
    });

    QObject::connect(stopAction, &QAction::triggered, [=]{
        scene->stopRun();
        stopAction->setEnabled(false);
        runAction->setEnabled(true);
        for (QAction* a : {placeAction, transAction, arcAction, deleteAction})
            a->setEnabled(true);
        auto rebuild = rebuildPanel;
        rebuild();
    });

    rebuildPanel(); // initial build (edit mode placeholder)

    qDebug() << "Petri Net Start";
    window.resize(1280, 720);
    window.show();
    return app.exec();
}
