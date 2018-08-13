#include "mydmmstatus.h"

MyDmmStatus::MyDmmStatus(QObject *parent) : CustomDmmStatus(parent)
{

}

void MyDmmStatus::parseData(const QByteArray &arr)
{
    double value = valueFromArray(arr);
    setValue(value);
    setValueString(QString::number(value));

    setUnit(QString::fromLatin1(pointFromArray(arr,10),3).trimmed());
    setBasis((quint8)arr.at(13) == 0x46?"Fe":"NFe");
    setContinuousMode((quint8)arr.at(14) == 0x31?true:false);

    quint8 status = (quint8)arr.at(15);
    setAutoPower(status & 0x80 ? true:false);
    setLowBat(status & 0x40 ? true:false);
}

double MyDmmStatus::valueFromArray(const QByteArray &arr)
{
    double result = 0.0;


    QString str = QString::fromLatin1(pointFromArray(arr,4),6);

    result = str.toDouble();

    return result;
}

char *MyDmmStatus::pointFromArray(const QByteArray &arr, quint32 pos)
{
    char * p = (char *)arr.data();
    p += pos;

    return p;
}


bool MyDmmStatus::lowBat() const
{
    return m_lowBat;
}

void MyDmmStatus::setLowBat(bool lowBat)
{
    m_lowBat = lowBat;
}

bool MyDmmStatus::autoPower() const
{
    return m_autoPower;
}

void MyDmmStatus::setAutoPower(bool autoPower)
{
    m_autoPower = autoPower;
}

bool MyDmmStatus::continuousMode() const
{
    return m_continuousMode;
}

void MyDmmStatus::setContinuousMode(bool continuousMode)
{
    m_continuousMode = continuousMode;
}

QString MyDmmStatus::basis() const
{
    return m_basis;
}

void MyDmmStatus::setBasis(const QString &basis)
{
    m_basis = basis;
}

QString MyDmmStatus::unit() const
{
    return m_unit;
}

void MyDmmStatus::setUnit(const QString &unit)
{
    m_unit = unit;
}
