#ifndef SERIALRS232VIR_H
#define SERIALRS232VIR_H

#include <QObject>
#include "serialrs232.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include "SerialRS232Inf.h"
#include <Windows.h>
#include <QWidget>
#include "serialrs232virinf.h"

/**
 * 适用虚拟串口，用VID、PID来鉴别设备，监控USB插入、拔出
 *先用用getUseDev()通过VID,PID取得虚拟串口，用setParameters设置串口参数，然后再open
 */

class USB;
class USBNotification;

class SerialRS232Vir : public SerialRS232,public SerialRS232VirInf
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.xp.SerialRS232Vir"/* FILE "extrafilters.json"*/)
    //    Q_INTERFACES(SerialDeviceInf)
    //    Q_INTERFACES(SerialRS232Inf)
    Q_INTERFACES(SerialRS232VirInf)
public:
    explicit SerialRS232Vir(QObject *parent = 0);
    ~SerialRS232Vir();    

    virtual QSerialPortInfo getUseDev(quint16 VID,quint16 PID);

    void refreshUsbs();

    USB *getUsb() const;
    void setUsb(USB *usb);

private:
    USB *m_usb;
    USBNotification * m_usbNotification;
signals:
    void usbRefreshed();
    void removed();
public slots:
};

/**
 * @brief The USB class
 * 代表一个USB设备
 */
class USB{
public:
    USB(){}
    USB(DWORD index,WORD VID,WORD PID,const QString &serial,QString &path){
        this->index = index;
        this->VID = VID;
        this->PID = PID;
        this->serial = serial;
        this->path = path;
    }

    DWORD index;
    WORD VID;
    WORD PID;
    QString serial;
    QString path;
};

class CP2110;
/**
 * @brief The USBNotification class
 * 用来接收USB设备的消息
 */
class USBNotification: public QWidget{
    Q_OBJECT
public:
    USBNotification(SerialRS232Vir *dev);
    ~USBNotification();
    void RegisterDeviceChange();
    void UnregisterDeviceChange();
    bool getID(const QString &src, quint16 * vid, quint16 * pid);
protected:
    bool nativeEvent(const QByteArray &eventType, void *msg, long *result);
private:
    HDEVNOTIFY m_handle;
    SerialRS232Vir *m_dev;
signals:
    void removed();
    void arrival();
};


#endif // SERIALRS232VIR_H
