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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling); //! Включение поддержки HighDPI
    //! Использование высококачественных пиктограмм
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv); //! Создание экземпляра QApplication
    MainWindow initial_window;    //! Создание главного окна приложения
    initial_window.show();        //! Отображение главного окна
    return app.exec();            //! Запуск главного цикла обработки событий приложения
}
