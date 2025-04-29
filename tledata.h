#ifndef TLEDATA_H
#define TLEDATA_H

#include <QString>

class TleData
{
public:
    explicit TleData(const QString &name, const QString &line1, const QString &line2);
    ~TleData();
    bool isValid() const;
    QString toString() const;

    // Геттеры
    QString name() const;
    QString line1() const;
    QString line2() const;

    // Для сериализации
    friend QDataStream &operator<<(QDataStream &out, const TleData &data);
    friend QDataStream &operator>>(QDataStream &in, TleData &data);

    // Сеттеры
    void setName(const QString &newName);
    void setLine1(const QString &newLine1);
    void setLine2(const QString &newLine2);

private:
    QString name_;
    QString line1_;
    QString line2_;
};

#endif // TLEDATA_H
