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
    // in.setEncoding("UTF-8");

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

// -----------------------------
// Основной разбор текста
// -----------------------------
void TleParser::parseText(const QString &text)
{
    // Разбиваем текст на строки, убираем пустые (по желанию)
    // Заметка: в некоторых файлах первая строка может быть заголовком или комментариями.
    // Здесь считаем, что каждая запись — это три подряд идущие непустые строки:
    //   [имя (опционально)], TLE-LINE1, TLE-LINE2
    QStringList allLines = text.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);

    int i = 0;
    while (i + 1 < allLines.size()) {
        QString line1 = allLines.at(i).trimmed();
        QString line2 = allLines.at(i + 1).trimmed();
        QString line3;

        // Если текущая строка не начинается с '1 ' (TLE Line1) или следующая не '2 ',
        // то предполагаем, что это имя, и тогда:
        //    name = line1
        //    line1 = allLines[i+1], line2 = allLines[i+2]
        // Идём далее на +3. Иначе, если сразу line1='1 ' и line2='2 ', то имя может отсутствовать,
        // и ставим пустое имя, а сам TLE — это line1 и line2.
        QString name;
        if (line1.startsWith('1') && line1.size() > 1 && line1.at(1).isSpace()
            && line2.startsWith('2') && line2.size() > 1 && line2.at(1).isSpace()) {
            name = QString();  // Нет явного имени
            line3 = QString(); // не используется
            // Текущие line1,line2 — это TLE.
            QString tleLine1 = line1;
            QString tleLine2 = line2;
            TleRecord rec;
            QString err;
            if (this->parseSingleTle(name, tleLine1, tleLine2, rec, err))
                this->records_.append(rec);
            i += 2;
        } else {
            // Предполагаем, что line1 — это имя
            name = line1;
            if (i + 2 >= allLines.size())
                break; // нет полноценного TLE после имени
            QString tleLine1 = allLines.at(i + 1).trimmed();
            QString tleLine2 = allLines.at(i + 2).trimmed();
            TleRecord rec;
            QString err;
            if (this->parseSingleTle(name, tleLine1, tleLine2, rec, err))
                this->records_.append(rec);
            i += 3;
        }
    }
}

// -----------------------------
// Разбор одной записи TLE (две строки + опционально имя)
// -----------------------------
bool TleParser::parseSingleTle(const QString &nameLine,
                               const QString &l1,
                               const QString &l2,
                               TleRecord &outRecord,
                               QString &err)
{
    // Проверяем, что строки действительно начинаются с '1 ' и '2 '
    if (!(l1.startsWith('1') && l1.size() > 1 && l1.at(1).isSpace())) {
        err = QString("Первая строка TLE не начинается с '1 ': %1").arg(l1);
        return false;
    }
    if (!(l2.startsWith('2') && l2.size() > 1 && l2.at(1).isSpace())) {
        err = QString("Вторая строка TLE не начинается с '2 ': %1").arg(l2);
        return false;
    }

    // Заполняем базовые поля
    outRecord.name = nameLine;
    outRecord.line1 = l1;
    outRecord.line2 = l2;

    // Поля из первой строки (line1). Фиксированные позиции (индексы) взяты из стандарта:
    // https://www.celestrak.com/columns/v02n03/ (или любой другой ТЛЕ-спецификации)
    // Пример разбора (позиции 1-based):
    // Количество символов в строке всегда >= 69, можно считать, что строка «правильного» размера.

    // 1. Номер спутника [2–7]
    bool ok = false;
    QString satNumStr = l1.mid(2, 5).trimmed();
    outRecord.satelliteNumber = satNumStr.toInt(&ok);
    if (!ok) {
        err = "Не удалось распарсить номер спутника (line1).";
        return false;
    }

    // 2. Классификация [8]
    outRecord.classification = l1.mid(7, 1);

    // 3. Международный обозначитель [10–17]
    outRecord.intDesignator = l1.mid(9, 8).trimmed();

    // 4. Эпоха [19–32] (yyddd.dddddddd)
    QString epochStr = l1.mid(18, 14).trimmed();
    outRecord.epoch = epochStr.toDouble(&ok);
    if (!ok) {
        err = "Не удалось распарсить epoch (line1).";
        return false;
    }

    // 5. Первая производная среднего движения [34–43] (не используем для упрощённого примера)

    // 6. Вторая производная среднего движения и т.д. (не используем)

    // Поля из второй строки (line2):
    // 7. Инклинация [9–16] (в градусах)
    QString inclStr = l2.mid(8, 8).trimmed();
    outRecord.inclination = inclStr.toDouble(&ok);
    if (!ok) {
        err = "Не удалось распарсить inclination (line2).";
        return false;
    }

    // 8. Долгота восходящего узла [18–25]
    QString raanStr = l2.mid(17, 8).trimmed();
    outRecord.raOfAscNode = raanStr.toDouble(&ok);
    if (!ok) {
        err = "Не удалось распарсить RA of Asc Node (line2).";
        return false;
    }

    // 9. Эксцентриситет [27–33] (текст без десятичной точки, например "0006703" → 0.0006703)
    QString eccStr = l2.mid(26, 7).trimmed();
    // Добавляем «0.» перед строкой
    QString eccDot = "0." + eccStr;
    outRecord.eccentricity = eccDot.toDouble(&ok);
    if (!ok) {
        err = "Не удалось распарсить eccentricity (line2).";
        return false;
    }

    // 10. Аргумент перигея [35–42]
    QString argPerStr = l2.mid(34, 8).trimmed();
    outRecord.argOfPerigee = argPerStr.toDouble(&ok);
    if (!ok) {
        err = "Не удалось распарсить Argument of Perigee (line2).";
        return false;
    }

    // 11. Средняя аномалия [44–51]
    QString mAnomalyStr = l2.mid(43, 8).trimmed();
    outRecord.meanAnomaly = mAnomalyStr.toDouble(&ok);
    if (!ok) {
        err = "Не удалось распарсить Mean Anomaly (line2).";
        return false;
    }

    // 12. Среднее движение [53–63] (rev/day)
    QString mmStr = l2.mid(52, 11).trimmed();
    outRecord.meanMotion = mmStr.toDouble(&ok);
    if (!ok) {
        err = "Не удалось распарсить Mean Motion (line2).";
        return false;
    }

    return true;
}

// -----------------------------
// Получить разобранные записи
// -----------------------------
QVector<TleRecord> TleParser::records() const
{
    return records_;
}
