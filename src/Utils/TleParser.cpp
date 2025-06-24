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
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit this->errorOccurred(QString("Не удалось открыть файл: %1").arg(filePath));
        return;
    }

    QTextStream in(&file);

    QString allText = in.readAll();
    file.close();

    this->records_.clear();
    this->parseText(allText);
    emit parsingFinished();
}

void TleParser::loadFromUrl(const QUrl &url)
{
    if (!url.isValid()) {
        emit this->errorOccurred("Неверный URL");
        return;
    }

    // Если уже есть незавершённый запрос — отменим
    if (this->currentReply_) {
        this->currentReply_->disconnect(this);
        this->currentReply_->abort();
        this->currentReply_->deleteLater();
        this->currentReply_ = nullptr;
    }

    QNetworkRequest request(url);
    currentReply_ = networkManager_->get(request);
    connect(this->currentReply_, &QNetworkReply::finished, this, &TleParser::onNetworkReplyFinished);
}

void TleParser::onNetworkReplyFinished()
{
    if (!this->currentReply_)
        return;

    if (this->currentReply_->error() != QNetworkReply::NoError) {
        QString errMsg = QString("Сетевая ошибка: %1").arg(this->currentReply_->errorString());
        this->currentReply_->deleteLater();
        this->currentReply_ = nullptr;
        emit errorOccurred(errMsg);
        return;
    }

    // Получаем весь ответ как текст
    QByteArray bytes = this->currentReply_->readAll();
    QString allText = QString::fromUtf8(bytes);

    this->currentReply_->deleteLater();
    this->currentReply_ = nullptr;

    records_.clear();
    parseText(allText);
    emit parsingFinished();
}

QVector<TleRecord> parseText(const QString &text, QString &err)
{
    QVector<TleRecord> recs;
    const auto lines = text.split('\n', Qt::SkipEmptyParts);
    int i = 0;
    while (i + 1 < lines.size()) {
        QString nameLine, l1, l2;
        // Detect 3LE vs 2LE
        if (lines[i].startsWith('1 ') || lines[i].startsWith('2 ')) {
            // 2LE: no name line
            nameLine.clear();
            l1 = lines[i];
            l2 = lines[i+1];
            i += 2;
        } else {
            // 3LE: name + two lines
            nameLine = lines[i];
            l1 = (i+1 < lines.size() ? lines[i+1] : QString());
            l2 = (i+2 < lines.size() ? lines[i+2] : QString());
            i += 3;
        }

        TleRecord rec;
        if (!parseSingleTle(nameLine, l1, l2, rec, err)) return {};
        recs.append(rec);
    }
    return recs;
}


bool TleParser::parseSingleTle(const QString &nameLine,
                               const QString &l1,
                               const QString &l2,
                               TleRecord &outRecord,
                               QString &err)
{
    static const QRegularExpression re_1(R"(^(1) (\d{5})([UCS ]) (\d{2})(\d{3})([A-Z ]{1,3}) (\d{2})(\d\.{11}) ([+-]\d\.\d{8}) ([+- ]\d{5}[+- ]\d) ([+- ]\d{5}[+-]\d) ([0-4]) ([ \d]{4})(\d)$)");
    static const QRegularExpression re_2(R"(^(2) (\d{5}) ([ \d]{3}\.[\d ]{4}) ([ \d]{3}\.[ \d]{4}) (\d{7}) ([ \d]{3}\.[\d ]{4}) ([ \d]{3}\.[\d ]{4}) ([ \d]{2}\.[\d ]{8})([ \d]{5})(\d)$)");

    // Try match Line1
    QRegularExpressionMatch m1 = re_1.match(l1);
    if (!m1.hasMatch()) {
        err = QStringLiteral("Line1 TLE format invalid");
        return false;
    }
    // Try match Line2
    QRegularExpressionMatch m2 = re_2.match(l2);
    if (!m2.hasMatch()) {
        err = QStringLiteral("Line2 TLE format invalid");
        return false;
    }
    outRecord.name = nameLine;
    outRecord.line1 = l1;
    outRecord.line2 = l2;

    outRecord.catalogNumber = m1.captured(2).toInt();
    outRecord.classification = m1.captured(3).trimmed();
    outRecord.intDesignator = m1.captured(4) + m1.captured(5) + m1.captured(6);
    outRecord.yearLaunch = 1900 + m1.captured(4).toInt();
    outRecord.numberLaunch = m1.captured(5).toInt();
    outRecord.launchPiece = m1.captured(6).trimmed();
    outRecord.epoch = m1.captured(7).toDouble();
    outRecord.meanMotionFirstDerivative = m1.captured(8).toDouble();
    outRecord.meanMotionSecondDerivative = m1.captured(9).toDouble();
    outRecord.brakingCoefficient = m1.captured(10).trimmed();
    outRecord.ephemerisType = m1.captured(11).toInt();
    outRecord.elementSetNumber = m1.captured(12).trimmed().toInt();
    outRecord.checksum1 = m1.captured(13).toInt();

    outRecord.inclination = m2.captured(2).toDouble();
    outRecord.rightAscension = m2.captured(3).toDouble();
    QString ecc = m2.captured(4);
    outRecord.eccentricity = QString("0.%1").arg(ecc).toDouble();
    outRecord.argPerigee = m2.captured(5).toDouble();
    outRecord.meanAnomaly = m2.captured(6).toDouble();
    outRecord.meanMotion = m2.captured(7).toDouble();
    outRecord.revolutionNumberOfEpoch = m2.captured(8).toInt();
    outRecord.checksum2 = m2.captured(9).toInt();

    return true;
}

QVector<TleRecord> TleParser::records() const
{
    return records_;
}
