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

#include <QObject>
#include <QString>
#include <QVector>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

/*!
 * \brief Структура TleRecord
 * \details
 * Хранит данные одной записи TLE (Two-Line Element).
 * Содержит поля для имени спутника, двух строк TLE и
 * различных параметров, таких как
 * номер спутника, классификация,
 * международный обозначитель,
 * эпоха, среднее движение,
 * наклонение, эксцентриситет,
 * долгота восходящего узла,
 * аргумент перигея и средняя аномалия.
 * Эти данные используются для
 * определения орбиты спутника
 * или другого объекта в космосе.
 */
struct TleRecord
{
    QString name;  // Заголовок (имя спутника или объекта)
    QString line1; // Первая строка TLE (начинается с '1 ')
    QString line2; // Вторая строка TLE (начинается с '2 ')

    // Поля из первой и второй строки TLE:
    int satelliteNumber;    // Номер спутника (из line1)
    QString classification; // Класс (например, 'U' — unclassified)
    QString intDesignator;  // Международный обозначитель (YYNNNPPP)
    double epoch;           // Эпоха в формате года + день года (например, 21234.12345678)
    double meanMotion;      // Среднее движение (rev/day)
    double inclination;     // Наклонение (градусы)
    double eccentricity;    // Эксцентриситет (но без точки, например, "0006703" → 0.0006703)
    double raOfAscNode;     // Долгота восходящего узла (градусы)
    double argOfPerigee;    // Аргумент перигея (градусы)
    double meanAnomaly;     // Средняя аномалия (градусы)
};

// Класс TleParser — умеет читать из файла и из сети, выдаёт список TleRecord
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

    /**
     * @brief records
     * @return
     */
    QVector<TleRecord> records() const;

signals:
    // Сигнал, который испускается после успешной загрузки и разбора
    void parsingFinished();

    // Сигнал, если произошла ошибка (например, не удалось открыть файл или сеть)
    void errorOccurred(const QString &message);

public slots:
    void loadFromFile(const QString &filePath);
    void loadFromUrl(const QUrl &url);

private slots:
    // Обработчики для завершения сетевого запроса
    void onNetworkReplyFinished();

private:
    // Разобрать всю входную строку (весь текст) и заполнить контейнер records_
    void parseText(const QString &text);

    // Разбор одной тройки строк (name, line1, line2). Возвращает true, если удалось успешно распарсить.
    bool parseSingleTle(const QString &nameLine,
                        const QString &l1,
                        const QString &l2,
                        TleRecord &outRecord,
                        QString &err);

    QNetworkAccessManager *networkManager_;
    QNetworkReply *currentReply_; // Хранит текущий QNetworkReply (или nullptr)
    QVector<TleRecord> records_;
};

#endif // TLEPARSER_HPP
