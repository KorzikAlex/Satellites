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

bool TleParser::loadFromFile(const QString &filePath)
{
    //! Проверяем, что путь к файлу не пустой
    if (filePath.isEmpty())
        return false;

    QFileInfo info(filePath); //! Получаем информацию о файле по указанному пути

    //! Проверяем, существует ли файл
    if (!info.exists()) {
        emit errorOccurred(tr("Файл \"%1\" не найден!").arg(filePath));
        return false;
    }
    //! Проверяем, является ли это файлом
    if (!info.isFile()) {
        emit errorOccurred(tr("\"%1\" не является файлом!").arg(filePath));
        return false;
    }
    //! Проверяем права доступа (на чтение)
    if (!info.isReadable()) {
        emit errorOccurred(tr("Нет прав на чтение файла \"%1\"!").arg(filePath));
        return false;
    }
    QFile file(filePath); //! Открываем файл для чтения

    //! Проверяем, является ли файл текстовым
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //! Если файл не удалось открыть, отправляем сигнал об ошибке
        emit this->errorOccurred(tr("Не удалось открыть файл \"%1\"").arg(filePath));
        return false;
    }

    QString text = QTextStream(&file).readAll(); //! Читаем весь текст из файла
    file.close();                                //! Закрываем файл после чтения

    this->records_.clear(); //! Очищаем предыдущие записи

    //! Разбираем текст на TLE записи
    if (this->parseText(text)) {
        //! Если разбор текста успешен, отправляем сигнал о завершении разбора
        emit parsingFinished();
        return true; //! Возвращаем true, если разбор успешен
    } else {
        //! Если разбор текста не удался, отправляем сигнал об ошибке
        emit this->errorOccurred(tr("Ошибка разбора TLE данных из файла \"%1\.").arg(filePath));
        return false; //! Возвращаем false, если разбор не удался
    }
}

bool TleParser::loadFromUrl(const QUrl &url)
{
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
    return true;
}

void TleParser::onNetworkReplyFinished()
{
    //! Если текущий ответ не существует, выходим
    if (!this->currentReply_)
        return;

    QNetworkReply *reply = currentReply_;
    currentReply_ = nullptr;
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(tr("Сетевая ошибка: %1").arg(reply->errorString()));
        return;
    }

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode != 200) {
        emit errorOccurred(tr("Ошибка HTTP: %1").arg(statusCode));
        return;
    }

    QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    if (!contentType.contains("text/plain", Qt::CaseInsensitive)) {
        emit errorOccurred(tr("Неверный тип содержимого: %1").arg(contentType));
        return;
    }

    QString text = QString::fromUtf8(reply->readAll());
    if (text.trimmed().isEmpty()) {
        emit errorOccurred(tr("Ответ от сервера пустой"));
        return;
    }

    this->records_.clear();
    if (!this->parseText(text)) {
        emit this->errorOccurred(tr("Не удалось разобрать TLE данные"));
        return;
    }
    emit parsingFinished();
}

bool TleParser::parseText(const QString &text)
{
    //! Разбиваем текст на строки, пропуская пустые строки
    const QStringList lines = text.split(QRegularExpression(R"(\r\n|\n|\r)"), Qt::SkipEmptyParts);
    int i = 0; //! Индекс для перебора строк
    while (i < lines.size()) {
        //! Проверяем, что текущая строка начинается с '1 ' или '2 '
        QString nameLine, line1, line2;
        //! Проверка, что файл в формате 2LE или 3LE

        if (i + 2 < lines.size() && !lines[i].startsWith("1 ") && !lines[i].startsWith("2 ")) {
            //! 3LE (с именем)
            nameLine = lines[i].trimmed();
            line1 = lines[i + 1].trimmed();
            line2 = lines[i + 2].trimmed();
            i += 3;
        }

        else if (i + 1 < lines.size() && (lines[i].startsWith("1 ") || lines[i].startsWith("2 "))) {
            //! 2LE (без имени)
            nameLine.clear();
            line1 = lines[i].trimmed();
            line2 = lines[i + 1].trimmed();
            i += 2;
        } else
            break;

        TleRecord rec; //! Создаем новую запись TLE
        if (parseSingleTle(nameLine, line1, line2, rec)) {
            rec.name = nameLine;
            this->records_.append(rec); //! Добавляем запись в список записей
        }
    }
    return !records_.isEmpty();
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
    //! Если первая строка не совпадает, возвращаем false
    if (!m1.hasMatch())
        return false;

    //! Проверяем, что вторая строка совпадает со второй строкой формата TLE
    QRegularExpressionMatch m2 = re_2.match(l2);
    //! Если вторая строка не совпадает, возвращаем false
    if (!m2.hasMatch())
        return false;

    if (!this->checkTleLine(l1) || !this->checkTleLine(l2))
        return false; //! Если контрольные суммы не совпадают, возвращаем false

    outRecord.name = nameLine; //! Записываем имя спутника или объекта
    outRecord.line1 = l1;      //! Записываем первую строку TLE
    outRecord.line2 = l2;      //! Записываем вторую строку TLE

    outRecord.catalogNumber = m1.captured(2).toInt();
    outRecord.classification = m1.captured(3).trimmed();
    outRecord.yearLaunch = m1.captured(4).toInt();
    outRecord.numberLaunch = m1.captured(5).toInt();
    outRecord.launchPiece = m1.captured(6).trimmed();
    outRecord.epoch = (m1.captured(7) + m1.captured(8)).toDouble();
    outRecord.epochYearSuffix = m1.captured(7).toInt();
    outRecord.epochTime = m1.captured(8).toDouble();
    outRecord.meanMotionFirstDerivative = m1.captured(9).toDouble();
    outRecord.meanMotionSecondDerivative = m1.captured(10).toDouble();
    outRecord.brakingCoefficient = m1.captured(11).trimmed();
    outRecord.ephemerisType = m1.captured(12).toInt();
    outRecord.elementSetNumber = m1.captured(13).trimmed().toInt();
    outRecord.checksum1 = m1.captured(14).toInt();

    outRecord.inclination = m2.captured(3).toDouble();
    outRecord.rightAscension = m2.captured(4).toDouble();
    QString ecc = m2.captured(5);
    outRecord.eccentricity = QString("0.%1").arg(ecc).toDouble();
    outRecord.argPerigee = m2.captured(6).toDouble();
    outRecord.meanAnomaly = m2.captured(7).toDouble();
    outRecord.meanMotion = m2.captured(8).toDouble();
    outRecord.revolutionNumberOfEpoch = m2.captured(9).toInt();
    outRecord.checksum2 = m2.captured(10).toInt();
    return true;
}

bool TleParser::checkTleLine(const QString &line) const
{
    bool ok;
    int checksum = line.right(1).toInt(&ok); //! последний символ
    if (!ok)
        return false; //! не цифра
    int sum = 0;
    for (int i = 0; i < line.length() - 1; ++i) {
        QChar c = line[i]; //! берем символы строки, кроме последнего
        if (c.isDigit())
            sum += c.digitValue(); //! цифра дает свое значение
        else if (c == QChar('-'))
            sum += 1; //! минус дает 1
        // остальные символы дают 0
    }
    return (sum % 10) == checksum;
}

QVector<TleRecord> TleParser::records() const
{
    return this->records_; //! Возвращаем список всех разобранных TLE записей
}
