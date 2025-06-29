/*!
 * \file mainwindow.hpp
 * \brief Заголовочный файл для главного окна приложения
 * \details
 * Этот файл содержит определение класса MainWindow,
 * который представляет главное окно приложения.
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \date 2025
 */
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QFileDialog>
#include <QInputDialog>
#include <QMainWindow>
#include <QMessageBox>

#include "infowindow.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*!
 * \brief Класс MainWindow
 * \details
 * Представляет главное окно приложения, которое позволяет пользователю
 * взаимодействовать с TLE-файлами.
 * Содержит элементы управления для загрузки TLE из файлов и URL.
 * Также предоставляет справочную информацию о программе.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief MainWindow — Конструктор класса MainWindow
     * \details
     * Инициализирует пользовательский интерфейс и подключает сигналы к слотам.
     * \param parent Родительский виджет (обычно nullptr)
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /*!
     * \brief ~MainWindow — Деструктор класса MainWindow
     * \details
     * Освобождает ресурсы, связанные с пользовательским интерфейсом.
     */
    ~MainWindow();

public slots:
    /*!
     * \brief openLocalFile
     * \details
     * Слот, который вызывается при выборе локального файла для открытия.
     */
    void openLocalFile();

    /*!
     * \brief openUrl
     * \details
     * Слот, который вызывается при выборе URL для открытия.
     */
    void openUrl();
    /*!
     * \brief showInfo
     * \param message Сообщение, которое будет отображено в окне информации
     * \details
     * Слот, который отображает информацию о программе.
     */
    void showError(const QString &message);

    /*!
     * \brief showAboutProgram - слот, который отображает информацию о программе
     * \details
     * Этот слот вызывается при выборе пункта меню "О программе".
     */
    void showAboutProgram();

private:
    /*!
     * \brief ui_ — Указатель на объект пользовательского интерфейса
     * Содержит все элементы управления, созданные в Qt Designer.
     */
    Ui::MainWindow *ui_;
};

#endif // MAINWINDOW_HPP
