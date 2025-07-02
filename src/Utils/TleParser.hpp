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
#include <QRegularExpression>
#include <QStringList>
#include <QTextStream>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "TleStatistics.hpp"

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

    /*!
     * \brief loadFromFile - загрузка TLE данных из файла
     * \param filePath Путь к файлу, содержащему TLE данные
     * \return true, если загрузка и разбор прошли успешно, иначе false
     */
    bool loadFromFile(const QString &filePath);

    /*!
     * \brief loadFromUrl - загрузка TLE данных из URL
     * \param url URL, откуда нужно загрузить TLE данные
     * \return true, если загрузка и разбор прошли успешно, иначе false
     * \details
     * Этот метод выполняет асинхронный запрос к указанному URL,
     * получает данные и вызывает разбор текста.
     */
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
