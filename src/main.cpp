#include "UI/mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow initial_window;
    initial_window.show();
    return app.exec();
}
