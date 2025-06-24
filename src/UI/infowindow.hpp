/*!
 * \file infowindow.hpp
 * \brief Заголовочный файл для класса InfoWindow.
 * \details
 * Этот файл содержит определение класса InfoWindow, который представляет окно с информацией о спутниках.
 * Он наследуется от QMainWindow и содержит методы для отображения информации о спутниках из TLE-файлов.
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \date 2025
 */
#ifndef INFOWINDOW_HPP
#define INFOWINDOW_HPP

#include <QMainWindow>
#include <QMessageBox>
#include "Utils/TleParser.hpp"

namespace Ui {
class InfoWindow;
}
/*! \brief Класс InfoWindow
 * \details
 * Представляет окно, в котором отображается информация о спутниках из TLE-файлов.
 * Наследуется от QMainWindow и содержит пользовательский интерфейс, созданный с помощью Qt Designer.
 */
class InfoWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief InfoWindow - конструктор класса InfoWindow.
     * \param records Список записей TLE, которые будут отображаться в окне.
     * \param parent Указатель на родительский виджет (по умолчанию nullptr).
     */
    explicit InfoWindow(const QList<TleRecord> &records, QWidget *parent = nullptr);
    /*!
     * \brief ~InfoWindow - деструктор класса InfoWindow.
     * Освобождает ресурсы, используемые окном.
     */
    ~InfoWindow();

private:
    /*!
     * \brief ui Указатель на пользовательский интерфейс, созданный с помощью Qt Designer.
     * Используется для доступа к элементам интерфейса в классе InfoWindow.
     */
    Ui::InfoWindow *ui;
};

#endif // INFOWINDOW_HPP
