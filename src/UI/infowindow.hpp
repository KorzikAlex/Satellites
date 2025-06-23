#ifndef INFOWINDOW_HPP
#define INFOWINDOW_HPP

#include <QMainWindow>
#include "Utils/TleParser.hpp"

namespace Ui {
class InfoWindow;
}

class InfoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit InfoWindow(const QList<TleRecord>& records, QWidget *parent = nullptr);
    ~InfoWindow();

private:
    Ui::InfoWindow *ui;
};

#endif // INFOWINDOW_HPP
