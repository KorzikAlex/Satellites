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
    qInfo() << "openLocalFile called";
    // Инициализация TleParser
    TleParser TleParser_(this);
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Открыть TLE файл"),
                                                    "",
                                                    tr("TLE файлы (*.txt *.tle)"));
    qInfo() << "Selected file path:" << filePath;
    if (!filePath.isEmpty()) {
        TleParser_.loadFromFile(filePath);
        InfoWindow *infoWindow = new InfoWindow(TleParser_.records(), this);
        infoWindow->setAttribute(Qt::WA_DeleteOnClose); // для автоматического удаления
        infoWindow->show();
    }
}

void MainWindow::openUrl()
{
    qInfo() << "openUrl called";
    // Инициализация TleParser
    TleParser TleParser_(this);
    QString urlPath = QInputDialog::getText(this, tr("Введите ссылку"), tr("TLE URL:"));
    qInfo() << "Selected url path:" << urlPath;
    if (!urlPath.isEmpty()) {
        TleParser_.loadFromUrl(urlPath);
        InfoWindow *infoWindow = new InfoWindow(TleParser_.records(), this);
        infoWindow->setAttribute(Qt::WA_DeleteOnClose); // для автоматического удаления
        infoWindow->show();
    }
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
