#include "tledata.h"

QString TleData::name() const
{
    return name_;
}

QString TleData::line1() const
{
    return line1_;
}

QString TleData::line2() const
{
    return line2_;
}

void TleData::setName(const QString &newName)
{
    name_ = newName;
}

void TleData::setLine1(const QString &newLine1)
{
    line1_ = newLine1;
}

void TleData::setLine2(const QString &newLine2)
{
    line2_ = newLine2;
}
