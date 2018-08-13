#ifndef SERIALRS232VIRINF_H
#define SERIALRS232VIRINF_H
#include "serialrs232inf.h"

#include <QObject>

static const quint16 RS232Vir_VID = 0x1198;
static const quint16 RS232Vir_PID = 0x000b;

class SerialRS232VirInf
{
public:
    virtual QSerialPortInfo getUseDev(quint16 VID,quint16 PID) = 0;

};

Q_DECLARE_INTERFACE(SerialRS232VirInf,"com.xp.SerialRS232VirInf")


#endif // SERIALRS232VIRINF_H
