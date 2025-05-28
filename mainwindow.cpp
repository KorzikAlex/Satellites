#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "include/TleParser.hpp"


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Инициализация TleParser
    TleParser TleParser_(this);
    // Подключаем сигнал к кнопке "FilePushButton"
    connect(ui->FilePushButton, &QPushButton::clicked, this, [&]() {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open TLE File"), "", tr("TLE Files (*.txt *.tle)"));
        if (!filePath.isEmpty()) TleParser_.loadFromFile(filePath);
    });

    // Подключаем сигнал к кнопке "UrlPushButton"
    connect(ui->UrlPushButton, &QPushButton::clicked, this, [&]() {
        QString url = QInputDialog::getText(this, tr("Enter URL"), tr("TLE URL:"));
        if (!url.isEmpty()) TleParser_.loadFromUrl(QUrl(url));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
