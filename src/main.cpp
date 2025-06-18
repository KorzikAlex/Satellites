/*!
 * \file main.cpp
 * \brief Главный файл приложения.
 * \author KorzikAlex
 */
#include "UI/mainwindow.hpp"

#include <QApplication>
#include <QPalette>

/*!
 * \brief qMain функция приложения.
 * \param argc Входной аргумент, количество аргументов командной строки.
 * \param argv Входной аргумент, массив аргументов командной строки.
 * \return
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow initial_window;
    initial_window.show();
    return app.exec();
}
