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

#include <QClipboard>
#include <QEvent>
#include <QFileDialog>
#include <QGuiApplication>
#include <QLatin1Char>
#include <QLatin1String>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStyleHints>
#include <QTimer>
#include <QToolBar>

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
     * \param records Вектор записей TLE, данные которых будут отображаться в окне.
     * \param parent Указатель на родительский виджет (по умолчанию nullptr).
     */
    explicit InfoWindow(const QVector<TleRecord> &records, QWidget *parent = nullptr);

    /*!
     * \brief ~InfoWindow - деструктор класса InfoWindow.
     * Освобождает ресурсы, используемые окном.
     */
    ~InfoWindow();

signals:
    /*!
     * \brief requestOpenLocalFile - сигнал, который запрашивает открытие локального файла.
     * \details
     * Этот сигнал используется для запроса открытия локального файла в окне InfoWindow.
     */
    void requestOpenLocalFile();

    /*!
     * \brief requestOpenUrl - сигнал, который запрашивает открытие URL.
     * \details
     * Этот сигнал используется для запроса открытия URL в окне InfoWindow.
     */
    void requestOpenUrl();

    /*!
     * \brief errorOccurred - сигнал, который сообщает об ошибке.
     * \param message Сообщение об ошибке, которое будет отображено в окне.
     * \details
     * Этот сигнал вызывается, когда происходит ошибка,
     * например, при сохранении результатов в файл
     * или при копировании результатов в буфер обмена.
     */
    void errorOccurred(const QString &message);

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

    /*!
     * \brief showError - слот для отображения сообщения об ошибке.
     * \details
     * Этот метод отображает сообщение об ошибке в статусной строке окна.
     */
    void showError(const QString &message);

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    /*!
     * \brief changeEvent - обработчик события изменения темы приложения.
     * \details
     * Этот метод вызывается при изменении темы приложения и обновляет стили виджетов.
     * \param event Указатель на событие изменения темы.
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
     * \brief bindActions - связывает действия с соответствующими слотами.
     * \details
     * Этот метод связывает действия, такие как сохранение результатов и копирование в буфер обмена,
     * с соответствующими слотами в классе InfoWindow.
     */
    void bindActions();
    /*!
     * \brief formattedResults - формирует текстовое представление результатов.
     * \return Строка, содержащая отформатированные результаты.
     */
    QString formattedResults() const;

    /*!
     * \brief modelFromMap - создает модель QAbstractItemModel из QMap.
     * \param map QMap, содержащий данные для модели.
     * \param headers Список заголовков для модели.
     * \return Указатель на созданную модель.
     * \details
     * Этот метод создает модель из QMap, которая будет использоваться для отображения данных в таблице.
     */
    QAbstractItemModel *modelFromMap(const QMap<int, int> &map, const QStringList &headers);

    /*!
     * \brief fillUiFromStats - заполняет пользовательский интерфейс данными из статистики.
     */
    void fillUiFromStats();

    /*!
     * \brief ui_ Указатель на пользовательский интерфейс, созданный с помощью Qt Designer.
     * \details
     * Используется для доступа к элементам интерфейса в классе InfoWindow.
     */
    Ui::InfoWindow *ui_;

    /*!
     * \brief stats_ Структура, содержащая статистику о спутниках.
     * \details
     * Содержит данные, такие как количество спутников, даты запусков и наклоны орбит и сам набор спутников.
     */
    TleStatistics stats_;
};

#endif // INFOWINDOW_HPP
