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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QEvent>
#include <QFileDialog>
#include <QGuiApplication>
#include <QInputDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QStyleHints>

#include "Utils/TleParser.hpp"
#include "InfoWindow.hpp"

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
     * \brief openLocalFile — слот для открытия локального файла
     * \details
     * Слот, который вызывается при выборе локального файла для открытия.
     */
    void openLocalFile();

    /*!
     * \brief openUrl — слот для ввода URL
     * \details
     * Слот, который вызывается при выборе URL для открытия.
     */
    void openUrl();

    /*!
     * \brief showError — слот для отображения сообщения об ошибке
     * \param message Сообщение, которое будет отображено в окне информации
     * \details
     * Слот, который отображает информацию о программе.
     */
    void showError(const QString &message);

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    /*!
     * \brief changeEvent — функция для изменения типа событий
     * \param event Указатель на объект события
     * \details
     * Этот метод переопределяет стандартное поведение каких-то событий.
     * \warning На данный момент реализована только обработка события смены темы.
     * Работает на версии Qt 6.5.0 и выше.
     */
    void changeEvent(QEvent *event) override;
#endif

private:
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    /*!
     * \brief updateStyles - обновляет стили приложения при смене темы
     * \details
     * Этот метод вызывается при смене темы приложения
     * и обновляет стили всех виджетов,
     * чтобы они соответствовали новой теме.
     */
    void updateStyles();
#endif
    /*!
     * \brief showInfoWindow - показывает окно с информацией о спутниках
     * \param records Список записей TLE, которые будут отображаться в окне
     * \details
     * Этот метод создает новое окно с информацией о спутниках,
     * используя данные из списка records.
     */
    void showInfoWindow(const QVector<TleRecord> &records);

    /*!
     * \brief bindActions - связывает действия с соответствующими слотами
     * \details
     * Этот метод подключает сигналы от элементов управления к слотам,
     * чтобы обеспечить функциональность приложения.
     */
    void bindActions();

    /*!
     * \brief ui_ — Указатель на объект пользовательского интерфейса
     * Содержит все элементы управления, созданные в Qt Designer.
     */
    Ui::MainWindow *ui_;

    /*!
     * \brief tleParser_ — Указатель на объект TleParser
     * \details
     * Этот объект используется для загрузки и обработки TLE-файлов.
     */
    TleParser *tleParser_ = nullptr;
};

#endif // MAINWINDOW_H
