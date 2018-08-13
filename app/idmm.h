#ifndef IDMM_H
#define IDMM_H

class SerialDeviceInf;

class IDmm
{
public:
    virtual QObject * getInstance() = 0;
    virtual bool open(SerialDeviceInf *dev) = 0;
    virtual void close() = 0;
};

Q_DECLARE_INTERFACE(IDmm,"com.xp.IDmm")

#endif // IDMM_H
