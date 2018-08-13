#include "serialrs232vir.h"
#include <Dbt.h>
#include <usbiodef.h>
#include "utils.h"

SerialRS232Vir::SerialRS232Vir(QObject *parent) : SerialRS232(parent),
    m_usb(new USB),
    m_usbNotification(NULL)
{
    m_usbNotification = new USBNotification(this);
    m_usbNotification->hide();
    connect(m_usbNotification,SIGNAL(removed()),this,SIGNAL(removed()));
}

SerialRS232Vir::~SerialRS232Vir()
{
    delete m_usbNotification;
}

QSerialPortInfo SerialRS232Vir::getUseDev(quint16 VID, quint16 PID)
{
    QSerialPortInfo rst;

    foreach (QSerialPortInfo inf, QSerialPortInfo::availablePorts()) {
        if(inf.hasProductIdentifier() && inf.hasVendorIdentifier()){
            if(inf.vendorIdentifier() == VID && inf.productIdentifier() == PID){
                setPortInfo(inf);
                rst = inf;

                m_usb->index = 0;
                m_usb->path = QString();
                m_usb->PID = inf.productIdentifier();
                m_usb->VID = inf.vendorIdentifier();
                m_usb->serial = QString();

                break;
            }
        }
    }

    return rst;
}


void SerialRS232Vir::refreshUsbs()
{
    emit usbRefreshed();
}

USB *SerialRS232Vir::getUsb() const
{
    return m_usb;
}

/**
 * @brief USBNotification::winEvent
 *          处理windows消息，这里只处理了USB设备的插入和移除消息
 * @param message
 * @param result
 * @return
 */
USBNotification::USBNotification(SerialRS232Vir *dev):QWidget(),
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
    GUID hidGuid = {0xA5DCBF10L, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};

    //{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };//HID

    //{ 0x25dbce51, 0x6c8f, 0x4a72, 0x8a,0x6d,0xb5,0x4c,0x2b,0x4f,0xc8,0x35 }
    DEV_BROADCAST_DEVICEINTERFACE notificationFilter;

//    hidGuid = GUID_DEVINTERFACE_USB_DEVICE;

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
    if(m_handle){
        UnregisterDeviceNotification(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }
}

bool USBNotification::getID(const QString &src, quint16 *vid, quint16 *pid)
{
    int pos;
    QString str;
    bool ok;
    //get vid
    pos = src.indexOf("VID_");
    if(pos == -1)
        return false;
    str = src.mid(pos + 4,4);
    *vid = str.toInt(&ok,16);
    if(!ok)
        return false;
    //get pid
    pos = src.indexOf("PID_");
    if(pos == -1)
        return false;
    str = src.mid(pos + 4,4);
    *pid = str.toInt(&ok,16);
    if(!ok)
        return false;

    return true;
}


bool USBNotification::nativeEvent(const QByteArray &eventType, void *msg, long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result)
    quint16 vid,pid;

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
//                cdebug << "usb arrival ";
                emit arrival();

                m_dev->refreshUsbs();
            }

            bResult = true;
            break;

        case DBT_DEVICEREMOVECOMPLETE://移除
            if(pdevInf->dbcc_devicetype == DBT_DEVTYP_DEVICEINTERFACE){
                devStr = QString::fromUtf16((ushort *)&pdevInf->dbcc_name);
//                cdebug << "usb removed " <<devStr;
                getID(devStr,&vid,&pid);
                if(m_dev->getActived() && m_dev->getUsb()){
                    if(vid == m_dev->getUsb()->VID && pid == m_dev->getUsb()->PID)//当前使用的设备被移除
                        emit removed();
                }
                m_dev->refreshUsbs();
            }

            bResult = true;
            break;

        default:
            /*TODO*/
            bResult = false;
            break;
        }
    }

    return bResult;
}

