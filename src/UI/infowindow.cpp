/*!
 * \file infowindow.cpp
 * \brief Исходный файл для окна информации о спутниках
 * \details
 * Этот файл содержит реализацию класса InfoWindow,
 * который представляет окно с информацией о спутниках из TLE-файлов.
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \date 2025
 */
#include "infowindow.hpp"
#include "./ui_infowindow.h"

InfoWindow::InfoWindow(const QList<TleRecord> &records, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::InfoWindow)
{
    ui->setupUi(this);
    connect(ui->aboutAction, &QAction::triggered, this, [&]() {
        QMessageBox::about(this,
                           tr("О программе"),
                           tr("\"Спутники\" - это программа для работы с TLE-файлами."));
    });
}

InfoWindow::~InfoWindow()
{
    delete ui;
}
