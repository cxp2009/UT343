#ifndef DMMDATA_H
#define DMMDATA_H

#include <QObject>
#include <QDateTime>

class DmmData : public QObject
{
    Q_OBJECT
public:
    explicit DmmData(QObject *parent = 0);

    double value() const;
    void setValue(double value);

    QString valueString() const;
    void setValueString(const QString &valueString);

    QString unit() const;
    void setUnit(const QString &unit);

    QString basis() const;
    void setBasis(const QString &basis);

    bool continuousMode() const;
    void setContinuousMode(bool continuousMode);

    bool autoPower() const;
    void setAutoPower(bool autoPower);

    bool lowBat() const;
    void setLowBat(bool lowBat);

    QDateTime datetime() const;
    void setDatetime(const QDateTime &datetime);

private:
    QDateTime m_datetime;
    double m_value;
    QString m_valueString;

    QString m_unit;//um or mil
    QString m_basis;// Fe铁基 or NFe非铁基
    bool m_continuousMode;//是否连续测量模式
    bool m_autoPower;//是否自动关机
    bool m_lowBat;//是否低电压

signals:

public slots:
};

#endif // DMMDATA_H
