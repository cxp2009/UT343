#ifndef SERIALRS232_H
#define SERIALRS232_H

#include <QObject>
#include <QThread>
#include <QSerialPortInfo>
#include <QSerialPort>
#include "SerialRS232Inf.h"

class SerialRS232ReadThread;

class SerialRS232 : public QObject,public SerialDeviceInf, public SerialRS232Inf
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "com.xp.SerialRS232"/* FILE "extrafilters.json"*/)
    Q_INTERFACES(SerialDeviceInf)
    Q_INTERFACES(SerialRS232Inf)
public:
    explicit SerialRS232(QObject *parent = 0);

    virtual bool open();
    bool open(QSerialPortInfo portInfo, QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits,
              QSerialPort::Parity parity);

    void virtual setParameters(QSerialPortInfo portInfo, QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits,
                       QSerialPort::Parity parity);
    virtual void close();
    virtual int write(QByteArray &data);
    virtual QObject * getInstance();

    QList<QSerialPortInfo> availablePorts() const;
    void appendData(const QByteArray & data);

    QSerialPortInfo getPortInfo() const;
    void setPortInfo(const QSerialPortInfo &portInfo);

    QSerialPort *getPort() const;

    bool getActived() const;
    void setActived(bool actived);

private:
    bool m_actived;
    QByteArray m_buffer;
    QSerialPort * m_port;
    QSerialPortInfo m_portInfo;
    SerialRS232ReadThread * m_readThread;
signals:
    void opened();
    void closed();
    void read(const QByteArray &data);
    void message(const QString &msg);
public slots:
    void slotPortReadyRead();
};

class SerialRS232ReadThread: public QThread
{
    Q_OBJECT
public:
    SerialRS232ReadThread(SerialRS232 * dev);
    void transaction();
    void stop();

protected:
    void run();

private:
    SerialRS232 * m_dev;
    bool m_stoped;
    QByteArray m_data;
};

#endif // SERIALRS232_H
