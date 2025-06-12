#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "../Utils/TleParser.hpp"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Инициализация TleParser
    TleParser TleParser_(this);
    // Подключаем сигнал к кнопке "FilePushButton"
    connect(ui->FilePushButton, &QPushButton::clicked, this, [&]() {
        QFileDialog dialog(this);
        QString filePath = QFileDialog::getOpenFileName(this,
                                                        tr("Open TLE File"),
                                                        "~",
                                                        tr("TLE Files (*.txt *.tle)"));
    });

    // Подключаем сигнал к кнопке "UrlPushButton"
    connect(ui->UrlPushButton, &QPushButton::clicked, this, [&]() {
        bool ok{};
        QString url = QInputDialog::getText(this, tr("Enter URL"), tr("TLE URL:");
        // if (ok && !url.isEmpty())
        //     textLabel->setText(text);

    });
}

MainWindow::~MainWindow() {
    delete ui;
}
