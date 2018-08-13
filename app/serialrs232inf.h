#ifndef SERIALRS232INF_H
#define SERIALRS232INF_H
#include "serialdeviceinf.h"
#include "QSerialPort"
#include "QSerialPortInfo"

class SerialRS232Inf/*: public SerialDeviceInf*/
{
public:
    virtual void setParameters(QSerialPortInfo portInfo, QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits,
                               QSerialPort::StopBits stopBits,QSerialPort::Parity parity) = 0;
};

Q_DECLARE_INTERFACE(SerialRS232Inf,"com.xp.SerialRS232Inf")


#endif // SERIALRS232_H
