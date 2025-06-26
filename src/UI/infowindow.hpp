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

#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QStandardItemModel>
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
public slots:
    /*!
     * \brief saveResults - слот для сохранения результатов в файл.
     * \details
     * Этот метод открывает диалоговое окно для выбора файла,
     * в который будут сохранены результаты.
     * После выбора файла, результаты сохраняются в указанный файл.
     */
    void saveResults();
    /*!
     * \brief copyResults - слот для копирования результатов в буфер обмена.
     * \details
     * Этот метод копирует текстовое представление результатов в буфер обмена.
     */
    void copyResults();

private:
    /*!
     * \brief ui_ Указатель на пользовательский интерфейс, созданный с помощью Qt Designer.
     * \details
     * Используется для доступа к элементам интерфейса в классе InfoWindow.
     */
    Ui::InfoWindow *ui_;
    /*!
     * \brief records_ Список записей TLE, которые будут отображаться в окне.
     * \details
     * Содержит информацию о спутниках, полученную из TLE-файлов.
     */
    QList<TleRecord> records_;
};

#endif // INFOWINDOW_HPP
