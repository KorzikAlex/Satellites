/*!
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \file mainwindow.cpp
 * \brief Исходный файл для главного окна приложения
 * \details
 * Этот файл содержит реализацию класса MainWindow, который представляет главное окно приложения.
 */
#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "Utils/TleParser.hpp"

void MainWindow::openLocalFile()
{
    // Инициализация TleParser
    TleParser TleParser_(this);
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Открыть TLE файл"),
                                                    "",
                                                    tr("TLE файлы (*.txt *.tle)"));
    if (!filePath.isEmpty())
        TleParser_.loadFromFile(filePath);
}

void MainWindow::openUrl()
{
    // Инициализация TleParser
    TleParser TleParser_(this);
    QString url = QInputDialog::getText(this, tr("Введите ссылку"), tr("TLE URL:"));
    if (!url.isEmpty())
        TleParser_.loadFromUrl(url);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Создание справки
    connect(ui->aboutProgrammAction, &QAction::triggered, this, [&]() {
        QMessageBox::about(this,
                           tr("О программе"),
                           tr("\"Спутники\" - это программа для работы с TLE-файлами."));
    });

    // Подключаем сигнал к кнопке "filePushButton"
    connect(ui->filePushButton, &QPushButton::clicked, this, &MainWindow::openLocalFile);

    // Подключаем сигнал к действию "fileAction"
    connect(ui->fileAction, &QAction::triggered, this, &MainWindow::openLocalFile);

    // Подключаем сигнал к кнопке "UrlPushButton"
    connect(ui->urlPushButton, &QPushButton::clicked, this, &MainWindow::openUrl);

    // Подключаем сигнал к действию "UrlAction"
    connect(ui->urlAction, &QAction::triggered, this, &MainWindow::openUrl);
}

MainWindow::~MainWindow()
{
    delete ui;
}
