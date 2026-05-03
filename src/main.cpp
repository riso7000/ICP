#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Petri Net Editor");
    window.resize(1280, 720);
    window.show();

    return app.exec();
}
