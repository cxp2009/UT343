#ifndef MYDMM_H
#define MYDMM_H

#include <QObject>
#include "dmm.h"

class DmmData;
class MyDmmStatus;

class MyDmm : public Dmm
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.xp.Dmm"/* FILE "extrafilters.json"*/)
public:
    explicit MyDmm(QObject *parent = 0);

    virtual bool open(SerialDeviceInf *dev);
    virtual void close();

    bool sendCmd(quint8 code, const QByteArray &data);

    QString arrayToHexString(const QByteArray &array);
protected:
    virtual void decodeFrame(const QByteArray &data);

private:
    bool parseReceiveData();
    void parseLiveData(const QByteArray & data);
    char * pointFromArray(const QByteArray & arr,quint32 pos);
    double valueFromArray(const QByteArray & arr);

    QByteArray m_receivedData;
    DataState *m_ds;
    quint8 m_msgType;//0:device id; 1:live data; 2: store data; 3: result;
    QString m_deviceID;//设备号
signals:

public slots:
    virtual void slotSerialDevRead(const QByteArray &data);
    void slotUSBRemoved();
};

#endif // MYDMM_H
