/*!
 * \file mainwindow.cpp
 * \brief Исходный файл для главного окна приложения
 * \details
 * Этот файл содержит реализацию класса MainWindow,
 * который представляет главное окно приложения.
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \date 2025
 */
#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "Utils/TleParser.hpp"

void MainWindow::openLocalFile()
{
    TleParser TleParser_(this); //! Инициализация TleParser
    //! Открытие диалогового окна для выбора файла
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Открыть TLE файл"),
                                                    "",
                                                    tr("TLE файлы (*.txt *.tle)"));
    //! Если пользователь выбрал файл
    if (!filePath.isEmpty()) {
        TleParser_.loadFromFile(filePath); //! Загружаем данные из файла
        //! Создаём окно информации
        InfoWindow *infoWindow = new InfoWindow(TleParser_.records(), this);
        infoWindow->setAttribute(Qt::WA_DeleteOnClose); //! для автоматического удаления
        infoWindow->show();                             //! Показываем окно информации
    }
}

void MainWindow::openUrl()
{
    TleParser TleParser_(this); //! Инициализация TleParser

    //! Запрос URL у пользователя
    QString urlPath = QInputDialog::getText(this, tr("Введите ссылку"), tr("TLE URL:"));

    //! Если пользователь ввёл URL
    if (!urlPath.isEmpty()) {
        TleParser_.loadFromUrl(urlPath); //! Загружаем данные из URL
        //! Создаём окно информации
        InfoWindow *infoWindow = new InfoWindow(TleParser_.records(), this);
        infoWindow->setAttribute(Qt::WA_DeleteOnClose); //! для автоматического удаления
        infoWindow->show();                             //! Показываем окно информации
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
{
    this->ui_->setupUi(this); //! Инициализация пользовательского интерфейса

    //! Создание справки
    this->connect(this->ui_->aboutProgrammAction, &QAction::triggered, this, [&]() {
        QMessageBox::about(this,
                           tr("О программе"),
                           tr("\"Спутники\" - это программа для работы с TLE-файлами."));
    });

    //! Подключаем сигнал к кнопке "filePushButton"
    this->connect(this->ui_->filePushButton,
                  &QPushButton::clicked,
                  this,
                  &MainWindow::openLocalFile);

    //! Подключаем сигнал к действию "fileAction"
    this->connect(this->ui_->fileAction, &QAction::triggered, this, &MainWindow::openLocalFile);

    //! Подключаем сигнал к кнопке "UrlPushButton"
    this->connect(this->ui_->urlPushButton, &QPushButton::clicked, this, &MainWindow::openUrl);

    //! Подключаем сигнал к действию "UrlAction"
    this->connect(this->ui_->urlAction, &QAction::triggered, this, &MainWindow::openUrl);
}

MainWindow::~MainWindow()
{
    delete this->ui_; //! Освобождаем ресурсы пользовательского интерфейса
}
