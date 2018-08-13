#include "dmmdata.h"

DmmData::DmmData(QObject *parent) : QObject(parent)
{

}

double DmmData::value() const
{
    return m_value;
}

void DmmData::setValue(double value)
{
    m_value = value;
}

QString DmmData::valueString() const
{
    return m_valueString;
}

void DmmData::setValueString(const QString &valueString)
{
    m_valueString = valueString;
}

QString DmmData::unit() const
{
    return m_unit;
}

void DmmData::setUnit(const QString &unit)
{
    m_unit = unit;
}

QString DmmData::basis() const
{
    return m_basis;
}

void DmmData::setBasis(const QString &basis)
{
    m_basis = basis;
}

bool DmmData::continuousMode() const
{
    return m_continuousMode;
}

void DmmData::setContinuousMode(bool continuousMode)
{
    m_continuousMode = continuousMode;
}

bool DmmData::autoPower() const
{
    return m_autoPower;
}

void DmmData::setAutoPower(bool autoPower)
{
    m_autoPower = autoPower;
}

bool DmmData::lowBat() const
{
    return m_lowBat;
}

void DmmData::setLowBat(bool lowBat)
{
    m_lowBat = lowBat;
}

QDateTime DmmData::datetime() const
{
    return m_datetime;
}

void DmmData::setDatetime(const QDateTime &datetime)
{
    m_datetime = datetime;
}
