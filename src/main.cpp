/*!
 * \file main.cpp
 * \brief Главный файл приложения.
 * \details
 * Этот файл содержит точку входа в приложение. Он создает экземпляр QApplication,
 * создает главное окно приложения и запускает главный цикл обработки событий.
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \date 2025
 */
#include <QApplication>

#include "UI/mainwindow.hpp"

/*!
 * \brief qMain функция приложения.
 * \param argc Входной аргумент, количество аргументов командной строки.
 * \param argv Входной аргумент, массив аргументов командной строки.
 * \details
 * Эта функция инициализирует приложение,
 * создает главное окно и запускает главный цикл обработки событий.
 * \return Возвращает код завершения приложения.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow initial_window;
    initial_window.show();
    return app.exec();
}
