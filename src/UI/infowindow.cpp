#include "infowindow.hpp"
#include "ui_infowindow.h"

InfoWindow::InfoWindow(const QList<TleRecord> &records, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::InfoWindow)
{
    ui->setupUi(this);
}

InfoWindow::~InfoWindow()
{
    delete ui;
}
