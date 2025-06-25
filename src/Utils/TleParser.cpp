/*!
 * \file main.cpp
 * \brief Парсер TLE-файлов
 * \details
 * Этот файл содержит реализацию класса TleParser,
 * который используется для загрузки и разбора TLE-файлов
 * или URL-адресов, содержащих TLE-данные.
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \date 2025
 */
#include "TleParser.hpp"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

TleParser::TleParser(QObject *parent)
    : QObject(parent)
    , networkManager_(new QNetworkAccessManager(this))
    , currentReply_(nullptr)
{}

TleParser::~TleParser()
{
    if (this->currentReply_)
        this->currentReply_->deleteLater();
}

void TleParser::loadFromFile(const QString &filePath)
{
    QFile file(filePath); //! Открываем файл для чтения
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //! Если файл не удалось открыть, отправляем сигнал об ошибке
        emit this->errorOccurred(QString("Не удалось открыть файл: %1").arg(filePath));
        return;
    }

    QTextStream in(&file); //! Читаем содержимое файла

    QString allText = in.readAll(); //! Получаем весь текст файла
    file.close();                   //! Закрываем файл после чтения

    this->records_.clear();   //! Очищаем предыдущие записи
    this->parseText(allText); //! Разбираем текст на TLE записи
    emit parsingFinished();   //! Отправляем сигнал о завершении разбора
}

void TleParser::loadFromUrl(const QUrl &url)
{
    if (!url.isValid()) {
        //! Если URL недействителен, отправляем сигнал об ошибке
        emit this->errorOccurred("Неверный URL");
        return;
    }

    //! Если уже есть незавершённый запрос — отменим
    if (this->currentReply_) {
        this->currentReply_->disconnect(this); //! Отсоединяем сигналы от текущего ответа
        this->currentReply_->abort();          //! Прерываем текущий ответ
        this->currentReply_->deleteLater();    //! Удаляем текущий ответ
        this->currentReply_ = nullptr;         //! Обнуляем указатель на текущий ответ
    }

    QNetworkRequest request(url); //! Создаем сетевой запрос с указанным URL
    this->currentReply_ = this->networkManager_->get(request); //! Выполняем GET-запрос
    //! Подключаем слот для обработки завершения запроса
    connect(this->currentReply_, &QNetworkReply::finished, this, &TleParser::onNetworkReplyFinished);
}

void TleParser::onNetworkReplyFinished()
{
    //! Если текущий ответ не существует, выходим
    if (!this->currentReply_)
        return;

    //! Проверяем, произошла ли ошибка при выполнении запроса
    if (this->currentReply_->error() != QNetworkReply::NoError) {
        //! Если ошибка есть, формируем сообщение об ошибке
        QString errMsg = QString("Сетевая ошибка: %1").arg(this->currentReply_->errorString());
        this->currentReply_->deleteLater(); //! Удаляем текущий ответ
        this->currentReply_ = nullptr;      //! Обнуляем указатель на текущий ответ
        emit errorOccurred(errMsg);         //! Отправляем сигнал об ошибке
        return;
    }

    QByteArray bytes = this->currentReply_->readAll(); //! Читаем все данные из ответа
    QString allText = QString::fromUtf8(bytes);        //! Преобразуем байты в строку

    this->currentReply_->deleteLater(); //! Удаляем текущий ответ
    this->currentReply_ = nullptr;      //! Обнуляем указатель на текущий ответ

    this->records_.clear();   //! Очищаем предыдущие записи
    this->parseText(allText); //! Разбираем текст на TLE записи
    emit parsingFinished();   //! Отправляем сигнал о завершении разбора
}

void TleParser::parseText(const QString &text)
{
    const auto lines
        = text.split('\n',
                     Qt::SkipEmptyParts); //! Разбиваем текст на строки, пропуская пустые строки
    int i = 0;                            //! Индекс для перебора строк
    while (i + 1 < lines.size()) {
        //! Проверяем, что текущая строка начинается с '1 ' или '2 '
        QString nameLine, l1, l2;
        //! Проверка, что файл в формате 2LE или 3LE
        if (lines[i].startsWith("1 ") || lines[i].startsWith("2 ")) {
            //! 2LE формат
            nameLine.clear();  //! Если имя не указано, оставляем пустым
            l1 = lines[i];     //! Первая строка TLE
            l2 = lines[i + 1]; //! Вторая строка TLE
            i += 2;            //! Переходим к следующей паре строк
        } else {
            //! 3LE формат
            nameLine = lines[i];                                    //! Имя спутника или объекта
            l1 = (i + 1 < lines.size() ? lines[i + 1] : QString()); //! Первая строка TLE
            l2 = (i + 2 < lines.size() ? lines[i + 2] : QString()); //! Вторая строка TLE
            i += 3; //! Переходим к следующей паре строк
        }
        TleRecord rec; //! Создаем новую запись TLE
        if (!parseSingleTle(nameLine, l1, l2, rec))
            exit(1);                //! Если не удалось разобрать TLE, завершаем программу с ошибкой
        this->records_.append(rec); //! Добавляем запись в список записей
    }
}

bool TleParser::parseSingleTle(const QString &nameLine,
                               const QString &l1,
                               const QString &l2,
                               TleRecord &outRecord)
{
    static const QRegularExpression re_1(
        R"(^(1) (\d{5})([UCS ]) (\d{2})(\d{3})([A-Z ]{1,3}) (\d{2})(\d.{11}) ([- +].\d{8}) ([-+ ]\d{5}[-+ ]\d) ([-+ ]\d{5}[-+]\d) ([0-4]) ([ \d]{4})(\d)$)");
    static const QRegularExpression re_2(
        R"(^(2) (\d{5}) ([ \d]{3}.[\d ]{4}) ([ \d]{3}.[ \d]{4}) (\d{7}) ([ \d]{3}.[\d ]{4}) ([ \d]{3}.[\d ]{4}) ([ \d]{2}.[\d ]{8})([ \d]{5})(\d)$)");
    //! Проверяем, что первая строка совпдает с первой строкой формата TLE
    QRegularExpressionMatch m1 = re_1.match(l1);
    if (!m1.hasMatch())
        //! Если первая строка не совпадает, возвращаем false
        return false;
    //! Проверяем, что вторая строка совпадает со второй строкой формата TLE
    QRegularExpressionMatch m2 = re_2.match(l2);
    if (!m2.hasMatch())
        //! Если вторая строка не совпадает, возвращаем false
        return false;
    outRecord.name = nameLine; //! Записываем имя спутника или объекта
    outRecord.line1 = l1;      //! Записываем первую строку TLE
    outRecord.line2 = l2;      //! Записываем вторую строку TLE

    outRecord.catalogNumber = m1.captured(2).toInt();    //! Записываем номер спутника
    outRecord.classification = m1.captured(3).trimmed(); //! Записываем классификацию спутника
    outRecord.intDesignator = m1.captured(4) + m1.captured(5)
                              + m1.captured(6);           //! Записываем международное обзоначение
    outRecord.yearLaunch = 1900 + m1.captured(4).toInt(); //! Записываем год запуска спутника
    outRecord.numberLaunch = m1.captured(5).toInt();      //! Записываем номер запуска спутника
    outRecord.launchPiece = m1.captured(6).trimmed();     //! Записываем часть запуска спутника
    outRecord.epoch = m1.captured(7).toDouble();          //! Записываем эпоху спутника
    outRecord.meanMotionFirstDerivative
        = m1.captured(8).toDouble(); //! Записываем первую производную от среднего движения
    outRecord.meanMotionSecondDerivative
        = m1.captured(9).toDouble(); //! Записываем вторую производную от среднего движения
    outRecord.brakingCoefficient = m1.captured(10).trimmed(); //! Записываем коэффициент торможения B*
    outRecord.ephemerisType = m1.captured(11).toInt(); //! Записываем тип эфемерид (сейчас всегда 0)
    outRecord.elementSetNumber = m1.captured(12).trimmed().toInt(); //! Записываем номер элемента
    outRecord.checksum1 = m1.captured(13).toInt(); //! Записываем контрольную сумму из первой строки

    outRecord.inclination = m2.captured(2).toDouble(); //! Записываем наклонение спутника в градусах
    outRecord.rightAscension = m2.captured(3)
                                   .toDouble(); //! Записываем долготу восходящего узла в градусах
    QString ecc = m2.captured(4);               //! Записываем эксцентриситет спутника (без точки)
    outRecord.eccentricity
        = QString("0.%1").arg(ecc).toDouble(); //! Записываем эксцентриситет в формате с точкой
    outRecord.argPerigee = m2.captured(5).toDouble();  //! Записываем аргумент перигея в градусах
    outRecord.meanAnomaly = m2.captured(6).toDouble(); //! Записываем среднюю аномалию в градусах
    outRecord.meanMotion = m2.captured(7)
                               .toDouble(); //! Записываем среднее движение в обращениях в день
    outRecord.revolutionNumberOfEpoch = m2.captured(8)
                                            .toInt(); //! Записываем номер витка на момент эпохи
    outRecord.checksum2 = m2.captured(9).toInt(); //! Записываем контрольную сумму из второй строки
    return true;
}

QVector<TleRecord> TleParser::records() const
{
    return this->records_; //! Возвращаем список всех разобранных TLE записей
}
