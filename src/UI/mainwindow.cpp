#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "../Utils/TleParser.hpp"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Создание справки
    connect(ui->AboutProgrammAction, &QAction::triggered, this, [&]() {
        QMessageBox::about(this, tr("О программе"), tr("\" Спутники\" - это программа для работы с TLE-файлами."));
    });

    // Инициализация TleParser
    TleParser TleParser_(this);

    // Подключаем сигнал к кнопке "FilePushButton"
    connect(ui->FilePushButton, &QPushButton::clicked, this, [&]() {
        QFileDialog dialog(this);
        QString filePath = QFileDialog::getOpenFileName(this,
                                                        tr("Open TLE File"),
                                                        "",
                                                        tr("TLE Files (*.txt *.tle)"));
        if (!filePath.isEmpty())
            TleParser_.loadFromFile(filePath);
    });

    // Подключаем сигнал к действию "FileAction"
    connect(ui->FileAction, &QAction::triggered, this, [&]() {
        QFileDialog dialog(this);
        QString filePath = QFileDialog::getOpenFileName(this,
                                                        tr("Open TLE File"),
                                                        "",
                                                        tr("TLE Files (*.txt *.tle)"));
        if (!filePath.isEmpty())
            TleParser_.loadFromFile(filePath);
    });

    // Подключаем сигнал к кнопке "UrlPushButton"
    connect(ui->UrlPushButton, &QPushButton::clicked, this, [&]() {
        QString url = QInputDialog::getText(this, tr("Enter URL"), tr("TLE URL:"));
        if (!url.isEmpty())
            TleParser_.loadFromUrl(url);
    });

    connect(ui->UrlAction, &QAction::triggered, this, [&]() {
        QString url = QInputDialog::getText(this, tr("Enter URL"), tr("TLE URL:"));
        if (!url.isEmpty())
            TleParser_.loadFromUrl(url);
    });


}

MainWindow::~MainWindow() {
    delete ui;
}

