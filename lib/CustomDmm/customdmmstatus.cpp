#include "customdmmstatus.h"

CustomDmmStatus::CustomDmmStatus(QObject *parent) : QObject(parent)
{

}

double CustomDmmStatus::value() const
{
    return m_value;
}

void CustomDmmStatus::setValue(double value)
{
    m_value = value;
}

QString CustomDmmStatus::valueString() const
{
    return m_valueString;
}

void CustomDmmStatus::setValueString(QString valueString)
{
    m_valueString = valueString;
}

QString CustomDmmStatus::valueToString(double value)
{
    return QString::number(value,'f');
}
