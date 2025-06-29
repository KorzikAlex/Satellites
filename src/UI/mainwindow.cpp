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
    TleParser *TleParser_ = new TleParser(this); //! Инициализация TleParser

    this->connect(TleParser_, &TleParser::errorOccurred, this, &MainWindow::showError);

    //! Открытие диалогового окна для выбора файла
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Открыть TLE файл"),
                                                    "",
                                                    tr("TLE файлы (*.txt *.tle);; Все файлы (*)"));
    //! Если пользователь выбрал файл
    if (TleParser_->loadFromFile(filePath)) {
        //! Создаём окно информации
        InfoWindow *infoWindow = new InfoWindow(TleParser_->records().toList(), this);
        infoWindow->setAttribute(Qt::WA_DeleteOnClose); //! для автоматического удаления
        infoWindow->show();                             //! Показываем окно информации
    }
}

void MainWindow::openUrl()
{
    TleParser *TleParser_ = new TleParser(this); //! Инициализация TleParser

    connect(TleParser_, &TleParser::errorOccurred, this, &MainWindow::showError);
    //! Запрос URL у пользователя
    QString urlPath = QInputDialog::getText(this, tr("Введите ссылку"), tr("Ссылка на TLE-файл:"));

    //! Если пользователь ввёл URL
    if (TleParser_->loadFromUrl(urlPath)) {
        //! Создаём окно информации
        InfoWindow *infoWindow = new InfoWindow(TleParser_->records().toList(), this);
        infoWindow->setAttribute(Qt::WA_DeleteOnClose); //! для автоматического удаления
        infoWindow->show();                             //! Показываем окно информации
    }
}

void MainWindow::showError(const QString &message)
{
    //! Показываем сообщение об ошибке при загрузке данных
    QMessageBox::critical(this, tr("Ошибка"), message);
}

void MainWindow::showAboutProgram()
{
    //! Показываем сообщение о программе
    QMessageBox::about(this,
                       tr("О программе"),
                       tr("\"Спутники\" - это программа для работы с TLE-файлами."));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
{
    this->ui_->setupUi(this); //! Инициализация пользовательского интерфейса

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

    //! Создание справки
    this->connect(this->ui_->aboutProgrammAction,
                  &QAction::triggered,
                  this,
                  &MainWindow::showAboutProgram);
}

MainWindow::~MainWindow()
{
    delete this->ui_; //! Освобождаем ресурсы пользовательского интерфейса
}
