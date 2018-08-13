#include "cp2110.h"
//#include <Windows.h>
#include <Dbt.h>
//#include<devguid.h>
//#include<SetupAPI.h>
//#include<InitGuid.h>
#include <QDebug>

static const WORD VID = 0X10C4;
static const WORD PID = 0XEA80;

CP2110::CP2110(QObject *parent):SerialDev(parent),
  m_fileName(QString()),
  m_dev(NULL),
  m_usbNotification(NULL),
  m_baudRate(9600),
  m_dataBits(HID_UART_EIGHT_DATA_BITS),
  m_parity(HID_UART_NO_PARITY),
  m_stopBits(HID_UART_SHORT_STOP_BIT),
  m_flowControl(HID_UART_NO_FLOW_CONTROL),
  m_usb(NULL)
{

}

CP2110::CP2110(const QString &fileName, QObject *parent) : CP2110(parent)
{    
    setFileName(fileName);
}

CP2110::~CP2110()
{
    delete m_usbNotification;
    if(lib.isLoaded())
        lib.unload();
}

bool CP2110::connectA(const char *signal, const QObject *receiver, const char *member, bool isConnect)
{
    if(isConnect){
        return connect(this,signal,receiver,member);
    }else
    {
        return disconnect(this,signal,receiver,member);
    }
}

bool CP2110::open()
{
    USB * usb = NULL;
    if(actived())
        return false;
    if(m_usbs.count() == 0)
        return false;

    usb = m_usbs.at(0);
    if(HID_UART_SUCCESS != HidUart_Open(&m_dev,usb->index,usb->VID,usb->PID ))
        return false;
    if(!m_dev)
        return false;

    HidUart_SetUartConfig(m_dev,m_baudRate,m_dataBits,m_parity,m_stopBits,m_flowControl);
    HidUart_SetTimeouts(m_dev,0,500);

    HidUart_FlushBuffers(m_dev,TRUE,TRUE);

    return SerialDev::open();
}

void CP2110::close()
{
    if(actived() && m_dev){
        HidUart_Close(m_dev);
        m_dev = NULL;
    }

    m_usb = NULL;

    SerialDev::close();
}

int CP2110::write(QByteArray &data)
{
    DWORD numWrite = 0;
    HID_UART_STATUS status = HID_UART_INVALID_DEVICE_OBJECT;

    if(!actived() || !m_dev)
        return status;

    status = HidUart_Write(m_dev,(BYTE *)data.constData(),data.size(),&numWrite);

    return status;
}

QList<USB *> CP2110::usbs() const
{
    return m_usbs;
}
/**
 * @brief CP2110::loadLib
 * 加载库文件，并获得相关函数
 * @return
 */
bool CP2110::loadLib()
{
    //    QFileInfo fi(m_fileName);
    //    if (!fi.isFile())
    //        return false;

    QLibrary lib(m_fileName);
    if(!lib.load())
        return false;

    HidUart_GetNumDevices = (HidUart_GetNumDevices_T)lib.resolve("HidUart_GetNumDevices");
    HidUart_GetString = (HidUart_GetString_T)lib.resolve("HidUart_GetString");;
    HidUart_GetOpenedString = (HidUart_GetOpenedString_T)lib.resolve("HidUart_GetOpenedString");;
    HidUart_GetIndexedString = (HidUart_GetIndexedString_T)lib.resolve("HidUart_GetIndexedString");
    HidUart_GetAttributes = (HidUart_GetAttributes_T)lib.resolve("HidUart_GetAttributes");;
    HidUart_GetOpenedAttributes = (HidUart_GetOpenedAttributes_T)lib.resolve("HidUart_GetOpenedAttributes");;
    HidUart_Open = (HidUart_Open_T)lib.resolve("HidUart_Open");;
    HidUart_Close = (HidUart_Close_T)lib.resolve("HidUart_Close");;
    HidUart_IsOpened = (HidUart_IsOpened_T)lib.resolve("HidUart_IsOpened");;
    HidUart_SetUartEnable = (HidUart_SetUartEnable_T)lib.resolve("HidUart_SetUartEnable");;
    HidUart_GetUartEnable = (HidUart_GetUartEnable_T)lib.resolve("HidUart_GetUartEnable");;
    HidUart_Read = (HidUart_Read_T)lib.resolve("HidUart_Read");;
    HidUart_Write = (HidUart_Write_T)lib.resolve("HidUart_Write");;
    HidUart_FlushBuffers = (HidUart_FlushBuffers_T)lib.resolve("HidUart_FlushBuffers");;
    HidUart_SetTimeouts = (HidUart_SetTimeouts_T)lib.resolve("HidUart_SetTimeouts");;
    HidUart_GetTimeouts = (HidUart_GetTimeouts_T)lib.resolve("HidUart_GetTimeouts");;
    HidUart_GetUartStatus = (HidUart_GetUartStatus_T)lib.resolve("HidUart_GetUartStatus");;
    HidUart_SetUartConfig = (HidUart_SetUartConfig_T)lib.resolve("HidUart_SetUartConfig");;
    HidUart_GetUartConfig = (HidUart_GetUartConfig_T)lib.resolve("HidUart_GetUartConfig");;
    HidUart_StartBreak = (HidUart_StartBreak_T)lib.resolve("HidUart_StartBreak");;
    HidUart_StopBreak = (HidUart_StopBreak_T)lib.resolve("HidUart_StopBreak");;
    HidUart_Reset = (HidUart_Reset_T)lib.resolve("HidUart_Reset");;
    HidUart_ReadLatch = (HidUart_ReadLatch_T)lib.resolve("HidUart_ReadLatch");;
    HidUart_WriteLatch = (HidUart_WriteLatch_T)lib.resolve("HidUart_WriteLatch");;
    HidUart_GetPartNumber = (HidUart_GetPartNumber_T)lib.resolve("HidUart_GetPartNumber_T");;
    HidUart_GetLibraryVersion = (HidUart_GetLibraryVersion_T)lib.resolve("HidUart_GetLibraryVersion");;
    HidUart_GetHidLibraryVersion = (HidUart_GetHidLibraryVersion_T)lib.resolve("HidUart_GetHidLibraryVersion");
    HidUart_GetHidGuid = (HidUart_GetHidGuid_T)lib.resolve("HidUart_GetHidGuid");


    return true;
}

QString CP2110::fileName() const
{
    return m_fileName;
}

void CP2110::setFileName(const QString &fileName)
{
    m_fileName = fileName;

    if (m_usbNotification){
        delete m_usbNotification;
    }

    if(lib.isLoaded()){
        lib.unload();
    }

    loadLib();
    refreshUsbs();

    m_usbNotification = new USBNotification(this);
    m_usbNotification->hide();
    connect(m_usbNotification,SIGNAL(removed()),this,SLOT(slotUsbRemoved()));
    connect(m_usbNotification,SIGNAL(arrival()),this,SLOT(slotUsbArrival()));
}

int CP2110::usbCount()
{
    return m_usbs.count();
}
/**
 * @brief CP2110::slotUsbRemoved
 * 处理设备移除的槽
 */
void CP2110::slotUsbRemoved()
{
    close();
    emit usbRemoved();
}
/**
 * @brief CP2110::slotUsbArrival
 * 处理设备添加的槽
 */
void CP2110::slotUsbArrival()
{
    emit usbArrival();
}
/**
 * @brief CP2110::refreshUsbs
 * 获得全部的设备，并指定当前使用的设备
 * @return
 */
bool CP2110::refreshUsbs()
{
    DWORD numDevices = 0;
    char serialChar[260]={0};
    char pathChar[512]={0};
    QString serialStr;
    QString pathStr;

    if(actived())
        return false;

    qDeleteAll(m_usbs);
    m_usbs.clear();

    if(HID_UART_SUCCESS != HidUart_GetNumDevices(&numDevices,VID,PID))
        return false;

    for(int i=0;(DWORD)i<numDevices;i++){        
        HidUart_GetString(i,VID,PID,serialChar,HID_UART_GET_SERIAL_STR);
        HidUart_GetString(i,VID,PID,pathChar,HID_UART_GET_PATH_STR);
        serialStr = QString(serialChar);
        pathStr = QString(pathChar);
        if(!serialStr.isEmpty() && !pathStr.isEmpty())
            m_usbs.append(new USB(i,VID,PID,QString(serialStr),QString(pathStr)));
    }

    if(m_usbs.count() > 0)
        m_usb = m_usbs.at(0);//默认第一个设备为active device

    emit usbRefreshed(m_usbs.count());

    return true;
}

#define READ_BUFF_SIZE 1024
/**
 * @brief CP2110::readData
 * 读数据
 */
void CP2110::readData()
{
    char buff[READ_BUFF_SIZE];

    DWORD numBytesRead = 0;

    if(!m_dev)
        return;
//    HidUart_IsOpened(m_dev,&opened);
//    if(!opened)
//        return;

    HidUart_Read(m_dev,(BYTE *)buff,READ_BUFF_SIZE,&numBytesRead);
    if(numBytesRead > 0){
        m_mutexRead.lock();
        m_readBuff = QByteArray(buff,numBytesRead);
//        qDebug() << "CP2110::readData()" << m_readBuff.toHex();
        emit read(m_readBuff);

        m_mutexRead.unlock();
    }
}

bool CP2110::actived() const
{
    return SerialDev::actived();
}
USB *CP2110::usb() const
{
    return m_usb;
}

BYTE CP2110::flowControl() const
{
    return m_flowControl;
}

void CP2110::setFlowControl(const BYTE &flowControl)
{
    m_flowControl = flowControl;
}


BYTE CP2110::stopBits() const
{
    return m_stopBits;
}

void CP2110::setStopBits(const BYTE &stopBits)
{
    m_stopBits = stopBits;
}

BYTE CP2110::parity() const
{
    return m_parity;
}

void CP2110::setParity(const BYTE &parity)
{
    m_parity = parity;
}

BYTE CP2110::dataBits() const
{
    return m_dataBits;
}

void CP2110::setDataBits(const BYTE &dataBits)
{
    m_dataBits = dataBits;
}

DWORD CP2110::baudRate() const
{
    return m_baudRate;
}

void CP2110::setBaudRate(const DWORD &baudRate)
{
    m_baudRate = baudRate;
}

/**
 * @brief USBNotification::winEvent
 *          处理windows消息，这里只处理了USB设备的插入和移除消息
 * @param message
 * @param result
 * @return
 */
bool USBNotification::nativeEvent(const QByteArray &eventType, void *msg, long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result)

    MSG *message = reinterpret_cast<MSG*>(msg);

    bool bResult = false;
    PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)message->lParam;
    PDEV_BROADCAST_DEVICEINTERFACE pdevInf = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
    QString devStr;


    if(message->message == WM_DEVICECHANGE){
        switch(message->wParam)
        {
        case DBT_DEVICEARRIVAL://插入
            if(pdevInf->dbcc_devicetype == DBT_DEVTYP_DEVICEINTERFACE){
                emit arrival();
            }
            m_dev->refreshUsbs();

            bResult = true;
            break;

        case DBT_DEVICEREMOVECOMPLETE://移除
            if(pdevInf->dbcc_devicetype == DBT_DEVTYP_DEVICEINTERFACE){
                devStr = QString::fromUtf16((ushort *)&pdevInf->dbcc_name);
                if(m_dev->actived() && m_dev->usb()){
                    if(devStr.toUpper() == m_dev->usb()->path.toUpper())//当前使用的设备被移除
                        emit removed();
                }
                m_dev->refreshUsbs();
            }

            bResult = true;
            break;
            //        case DBT_DEVNODES_CHANGED:
            //            /*TODO*/
            //            bResult = true;
            //            break;

        default:
            /*TODO*/
            bResult = false;
            break;
        }
    }

    return bResult;
}

USBNotification::USBNotification(CP2110 *dev):QWidget(),
    m_handle(NULL)
{
    m_dev = dev;
    RegisterDeviceChange();
}

USBNotification::~USBNotification()
{
    UnregisterDeviceChange();
}

void USBNotification::RegisterDeviceChange()
{
    GUID hidGuid = { 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };
    //{ 0x25dbce51, 0x6c8f, 0x4a72, 0x8a,0x6d,0xb5,0x4c,0x2b,0x4f,0xc8,0x35 }
    DEV_BROADCAST_DEVICEINTERFACE notificationFilter;

//    HidD_GetHidGuid(&hidGuid);
    ZeroMemory(&notificationFilter,sizeof(notificationFilter));
    notificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    notificationFilter.dbcc_classguid = hidGuid;
    m_handle = RegisterDeviceNotification((HANDLE)this->winId(),
                                          &notificationFilter,
                                          DEVICE_NOTIFY_WINDOW_HANDLE);

}

void USBNotification::UnregisterDeviceChange()
{
    if(m_handle)
        UnregisterDeviceNotification(m_handle);
}



