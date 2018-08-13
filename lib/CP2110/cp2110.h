#ifndef CP2110_H
#define CP2110_H

#include <QWidget>
#include "serialdev.h"
#include <QLibrary>
#include <QMutex>
#include <QThread>
#include "serialdeviceinf.h"
#include "cp2110inf.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "SLABCP2110.h"
#ifdef __cplusplus
}
#endif // __cplusplus


typedef HID_UART_STATUS (__stdcall *HidUart_GetNumDevices_T)(DWORD* numDevices, WORD vid, WORD pid);
typedef HID_UART_STATUS (__stdcall *HidUart_GetString_T)(DWORD deviceNum, WORD vid, WORD pid, char* deviceString, DWORD options);
typedef HID_UART_STATUS (__stdcall *HidUart_GetOpenedString_T)(HID_UART_DEVICE device, char* deviceString, DWORD options);
typedef HID_UART_STATUS (__stdcall *HidUart_GetIndexedString_T)(DWORD deviceNum, WORD vid,WORD pid, DWORD stringIndex, char* deviceString);
typedef HID_UART_STATUS (__stdcall *HidUart_GetAttributes_T)(DWORD deviceNum, WORD vid, WORD pid, WORD* deviceVid, WORD* devicePid, WORD* deviceReleaseNumber);
typedef HID_UART_STATUS (__stdcall *HidUart_GetOpenedAttributes_T)(HID_UART_DEVICE device, WORD* deviceVid, WORD* devicePid, WORD* deviceReleaseNumber);
typedef HID_UART_STATUS (__stdcall *HidUart_Open_T)(HID_UART_DEVICE* device, DWORD deviceNum, WORD vid, WORD pid);
typedef HID_UART_STATUS (__stdcall *HidUart_Close_T)(HID_UART_DEVICE device);
typedef HID_UART_STATUS (__stdcall *HidUart_IsOpened_T)(HID_UART_DEVICE device, BOOL* opened);
typedef HID_UART_STATUS (__stdcall *HidUart_SetUartEnable_T)(HID_UART_DEVICE device, BOOL enable);
typedef HID_UART_STATUS (__stdcall *HidUart_GetUartEnable_T)(HID_UART_DEVICE device, BOOL* enable);
typedef HID_UART_STATUS (__stdcall *HidUart_Read_T)(HID_UART_DEVICE device, BYTE* buffer, DWORD numBytesToRead, DWORD* numBytesRead);
typedef HID_UART_STATUS (__stdcall *HidUart_Write_T)(HID_UART_DEVICE device, BYTE* buffer, DWORD numBytesToWrite, DWORD* numBytesWritten);
typedef HID_UART_STATUS (__stdcall *HidUart_FlushBuffers_T)(HID_UART_DEVICE device, BOOL flushTransmit, BOOL flushReceive);
typedef HID_UART_STATUS (__stdcall *HidUart_SetTimeouts_T)(HID_UART_DEVICE device, DWORD readTimeout, DWORD writeTimeout);
typedef HID_UART_STATUS (__stdcall *HidUart_GetTimeouts_T)(HID_UART_DEVICE device, DWORD* readTimeout, DWORD* writeTimeout);
typedef HID_UART_STATUS (__stdcall *HidUart_GetUartStatus_T)(HID_UART_DEVICE device, WORD* transmitFifoSize, WORD* receiveFifoSize, BYTE* errorStatus, BYTE* lineBreakStatus);
typedef HID_UART_STATUS (__stdcall *HidUart_SetUartConfig_T)(HID_UART_DEVICE device, DWORD baudRate, BYTE dataBits, BYTE parity, BYTE stopBits, BYTE flowControl);
typedef HID_UART_STATUS (__stdcall *HidUart_GetUartConfig_T)(HID_UART_DEVICE device, DWORD* baudRate, BYTE* dataBits, BYTE* parity, BYTE* stopBits, BYTE* flowControl);
typedef HID_UART_STATUS (__stdcall *HidUart_StartBreak_T)(HID_UART_DEVICE device, BYTE duration);
typedef HID_UART_STATUS (__stdcall *HidUart_StopBreak_T)(HID_UART_DEVICE device);
typedef HID_UART_STATUS (__stdcall *HidUart_Reset_T)(HID_UART_DEVICE device);
typedef HID_UART_STATUS (__stdcall *HidUart_ReadLatch_T)(HID_UART_DEVICE device, WORD* latchValue);
typedef HID_UART_STATUS (__stdcall *HidUart_WriteLatch_T)(HID_UART_DEVICE device, WORD latchValue, WORD latchMask);
typedef HID_UART_STATUS (__stdcall *HidUart_GetPartNumber_T)(HID_UART_DEVICE device, BYTE* partNumber, BYTE* version);
typedef HID_UART_STATUS (__stdcall *HidUart_GetLibraryVersion_T)(BYTE* major, BYTE* minor, signed char* release);
typedef HID_UART_STATUS (__stdcall *HidUart_GetHidLibraryVersion_T)(BYTE* major, BYTE* minor, signed char* release);
typedef HID_UART_STATUS (__stdcall *HidUart_GetHidGuid_T) (void* guid);


/**
 * @brief The USB class
 * 代表一个USB设备
 */
class USB{
public:
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
    USBNotification(CP2110 *dev);
    ~USBNotification();
    void RegisterDeviceChange();
    void UnregisterDeviceChange();
protected:
    bool winEvent(MSG *message, long *result);
    bool nativeEvent(const QByteArray &eventType, void *msg, long *result);
private:
    HDEVNOTIFY m_handle;
    CP2110 *m_dev;
signals:
    void removed();
    void arrival();
};

/**
 * @brief The CP2110 class
 * 封装CP2110库的类
 */
class CP2110 :public SerialDev,public SerialDeviceInf,public CP2110Inf
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.xp.SerialDeviceInf"/* FILE "extrafilters.json"*/)
    Q_PLUGIN_METADATA(IID "com.xp.CP2110Inf"/* FILE "extrafilters.json"*/)
    Q_INTERFACES(SerialDeviceInf)
    Q_INTERFACES(CP2110Inf)
public:
    explicit CP2110(QObject *parent = 0);
    explicit CP2110(const QString &fileName, QObject *parent = 0);
    ~CP2110();

//    virtual bool connectThis(const char *signal,const QObject *receiver, const char *member, bool isConnect);
//    virtual bool connectSupper(const char *signal,const QObject *receiver, const char *member, bool isConnect);

    virtual bool connectA(const char *signal,const QObject *receiver, const char *member, bool isConnect);

    virtual bool open();
    virtual void close();
    virtual int write(QByteArray &data);

    bool refreshUsbs();
    Q_INVOKABLE virtual void readData();

    virtual bool actived() const;


    QList<USB *> usbs() const;
    DWORD baudRate() const;
    void setBaudRate(const DWORD &baudRate);

    BYTE dataBits() const;
    void setDataBits(const BYTE &dataBits);

    BYTE parity() const;
    void setParity(const BYTE &parity);

    BYTE stopBits() const;
    void setStopBits(const BYTE &stopBits);

    BYTE flowControl() const;
    void setFlowControl(const BYTE &flowControl);
    USB *usb() const;
    QString fileName() const;
    virtual void setFileName(const QString &fileName);

    virtual int usbCount();


protected:
    //    void timerEvent(QTimerEvent *);
private:
    bool loadLib();

    QLibrary lib;
    HID_UART_DEVICE m_dev;
    QString m_fileName;                 //库文件名称
    QList<USB *> m_usbs;                //所有的CP2110设备
    USB * m_usb;                        //当前使用的设备
    QMutex m_mutexRead;
    QByteArray m_readBuff;              //读数据的buffer
    USBNotification * m_usbNotification;//处理设备的消息，主要是新添和移除设备消息的处理。


    DWORD m_baudRate;
    BYTE m_dataBits;
    BYTE m_parity;
    BYTE m_stopBits;
    BYTE m_flowControl;

    HidUart_GetNumDevices_T HidUart_GetNumDevices;
    HidUart_GetString_T HidUart_GetString;
    HidUart_GetOpenedString_T HidUart_GetOpenedString;
    HidUart_GetIndexedString_T HidUart_GetIndexedString;
    HidUart_GetAttributes_T HidUart_GetAttributes;
    HidUart_GetOpenedAttributes_T HidUart_GetOpenedAttributes;
    HidUart_Open_T HidUart_Open;
    HidUart_Close_T HidUart_Close;
    HidUart_IsOpened_T HidUart_IsOpened;
    HidUart_SetUartEnable_T HidUart_SetUartEnable;
    HidUart_GetUartEnable_T HidUart_GetUartEnable;
    HidUart_Read_T HidUart_Read;
    HidUart_Write_T HidUart_Write;
    HidUart_FlushBuffers_T HidUart_FlushBuffers;
    HidUart_SetTimeouts_T HidUart_SetTimeouts;
    HidUart_GetTimeouts_T HidUart_GetTimeouts;
    HidUart_GetUartStatus_T HidUart_GetUartStatus;
    HidUart_SetUartConfig_T HidUart_SetUartConfig;
    HidUart_GetUartConfig_T HidUart_GetUartConfig;
    HidUart_StartBreak_T HidUart_StartBreak;
    HidUart_StopBreak_T HidUart_StopBreak;
    HidUart_Reset_T HidUart_Reset;
    HidUart_ReadLatch_T HidUart_ReadLatch;
    HidUart_WriteLatch_T HidUart_WriteLatch;
    HidUart_GetPartNumber_T HidUart_GetPartNumber;
    HidUart_GetLibraryVersion_T HidUart_GetLibraryVersion;
    HidUart_GetHidLibraryVersion_T HidUart_GetHidLibraryVersion;
    HidUart_GetHidGuid_T HidUart_GetHidGuid;


signals:
    void usbRefreshed(int usbcnt);  //设备列表已刷新
    void usbRemoved();
    void usbArrival();

public slots:
    void slotUsbRemoved();
    void slotUsbArrival();

};

#endif // CP2110_H
