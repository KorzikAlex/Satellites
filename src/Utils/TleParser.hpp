/*!
 * \file TleParser.hpp
 * \brief Заголовочный файл для класса TleParser.
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \details
 * Этот файл содержит определение класса TleParser, который предназначен для
 * чтения и разбора TLE (Two-Line Element) данных из файлов или URL.
 */
#ifndef TLEPARSER_HPP
#define TLEPARSER_HPP

#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QPair>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QUrl>
#include <QVector>
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

/*!
 * \brief Структура TleRecord
 * \details
 * Хранит данные одной записи TLE (Two-Line Element).
 */
struct TleRecord
{
    //! Поля для хранения данных TLE:
    QString name;  //! Заголовок (имя спутника или объекта)
    QString line1; //! Первая строка TLE (начинается с '1 ')
    QString line2; //! Вторая строка TLE (начинается с '2 ')

    //! Поля из первой строки TLE:
    int catalogNumber;      //! Номер спутника (из line1)
    QString classification; //! Класс (например, 'U' — unclassified)
    int yearLaunch;         //! Год запуска (последние 2 цифры года)
    int numberLaunch;       //! Номер запуска (например, 001, 002 и т.д.)
    QString launchPiece;    //! Часть запуска (например, 'A', 'B' и т.д.)
    double epoch;           //! Эпоха в формате года + день года (например, 21234.12345678)
    double meanMotionFirstDerivative;  //! Первая производная от среднего движения (rev/day^2)
    double meanMotionSecondDerivative; //! Вторая производная от среднего движения (rev/day^3)
    QString brakingCoefficient;        //! Коэффициент торможения B*
    int ephemerisType;                 //! Тип эфемерид (обычно 0)
    int elementSetNumber;              //! Номер элемента
    int checksum1;                     //! Контрольная сумма (из line1)

    //! Поля из второй строки TLE:
    double inclination;          //! Наклонение (градусы)
    double rightAscension;       //! Долгота восходящего узла (градусы)
    double eccentricity;         //! Эксцентриситет (но без точки, например, "0006703" → 0.0006703)
    double argPerigee;           //! Аргумент перигея (градусы)
    double meanAnomaly;          //! Средняя аномалия (градусы)
    double meanMotion;           //! Среднее движение (обращения в день)
    int revolutionNumberOfEpoch; //! Номер обращения
    int checksum2;               //! Контрольная сумма (из line2)
};

enum class ERROR_PARSING_CODE {
    NO_ERROR,       //! Нет ошибки
    INVALID_FORMAT, //! Неверный формат TLE
    CHECKSUM_ERROR, //! Ошибка контрольной суммы
    MISSING_LINES,  //! Отсутствуют строки TLE
    UNKNOWN_ERROR   //! Неизвестная ошибка
};

typedef QPair<TleRecord, ERROR_PARSING_CODE> TleRecordPair;

/*!
 * \brief Класс TleParser
 * \details
 * Предназначен для загрузки и разбора TLE (Two-Line Element) данных
 * из файлов или URL.
 */
class TleParser : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief TleParser — конструктор класса TleParser
     * \param parent Родитель объекта (обычно nullptr)
     */
    explicit TleParser(QObject *parent = nullptr);
    /*!
     * \brief ~TleParser — деструктор класса TleParser
     * \details
     * Освобождает ресурсы, связанные с сетевыми запросами.
     */
    ~TleParser();

    /*!
     * \brief records
     * \return Список всех разобранных TLE записей.
     */
    QVector<TleRecord> records() const;

    bool loadFromFile(const QString &filePath);

    bool loadFromUrl(const QUrl &url);

signals:
    /*!
     * \brief parsingFinished
     * \details
     * Сигнал, который вызывается после завершения разбора TLE данных.
     */
    void parsingFinished();

    /*!
     * \brief errorOccurred - сигнал, который вызывается при ошибке
     * \param message - Сообщение об ошибке.
     * \details
     * Этот сигнал используется для уведомления об ошибках,
     */
    void errorOccurred(const QString &message);

private slots:
    /*!
     * \brief onNetworkReplyFinished - слот, который вызывается при завершении сетевого запроса
     * \details
     * Обрабатывает ответ от сети, читает данные и вызывает разбор текста.
     */
    void onNetworkReplyFinished();

private:
    /*!
     * \brief parseText - разбор текста TLE данных
     * \param text - текст, содержащий TLE данные
     * \details
     * Этот метод разбивает текст на строки, проверяет их формат,
     * и вызывает разбор каждой пары строк TLE.
     */
    bool parseText(const QString &text);

    /*!
     * \brief parseSingleTle - разбор одной записи TLE
     * \param nameLine - имя спутника или объекта (опционально)
     * \param l1 - первая строка TLE (начинается с '1 ')
     * \param l2 - вторая строка TLE (начинается с '2 ')
     * \param outRecord - выходной параметр, куда будет записана разобранная запись TLE
     * \param err - выходной параметр для сообщения об ошибке
     * \return true, если разбор успешен, иначе false
     */
    bool parseSingleTle(const QString &nameLine,
                        const QString &l1,
                        const QString &l2,
                        TleRecord &outRecord);

    /*!
     * \brief checkTleLile - проверка контрольной суммы TLE файла
     * \param line - строка TLE, которую нужно проверить
     * \return true, если контрольная сумма верна, иначе false
     * \details
     * Этот метод читает файл и проверяет контрольные суммы
     */
    bool checkTleLine(const QString &line) const;

    /*!
     * \brief networkManager_ - менеджер сетевых запросов
     * \details
     * Этот объект используется для выполнения асинхронных сетевых запросов
     */
    QNetworkAccessManager *networkManager_;

    /*!
     * \brief currentReply_ - хранит текущий сетевой ответ
     * \details
     * Это указатель на QNetworkReply, который используется для асинхронного получения данных.
     * Если запрос ещё не завершён, то currentReply_ не равен nullptr.
     */
    QNetworkReply *currentReply_;

    /*!
     * \brief records_ - хранит все разобранные TLE записи
     * \details
     * Это вектор, который содержит все TLE записи,
     * разобранные из файла или URL.
     */
    QVector<TleRecord> records_;
};

#endif // TLEPARSER_HPP
