#include "mydmm.h"
#include "utils.h"
#include "serialrs232inf.h"
#include "serialrs232virinf.h"

MyDmm::MyDmm(QObject *parent) : Dmm(parent),
    m_ds(new DataState)
{
    m_ds->reset();

}

bool MyDmm::open(SerialDeviceInf *dev)
{
    setSerialDev(dev);
//    SerialRS232Inf * dev_ = qobject_cast<SerialRS232Inf *>(serialDev()->getInstance());
    SerialRS232VirInf * dev_ = qobject_cast<SerialRS232VirInf *>(dev->getInstance());
    QSerialPortInfo pi;
    pi = dev_->getUseDev(RS232Vir_VID,RS232Vir_PID);
//    pi = QSerialPortInfo::availablePorts().at(0);
    if(pi.isNull())
        return false;
    SerialRS232Inf * dev__ = qobject_cast<SerialRS232Inf *>(dev->getInstance());
    dev__->setParameters(pi,
                         QSerialPort::Baud19200,
                         QSerialPort::Data7,
                         QSerialPort::OneStop,
                         QSerialPort::NoParity);

    connect(serialDev()->getInstance(),SIGNAL(removed()),this,SLOT(slotUSBRemoved()));
    return Dmm::open(dev);
}

void MyDmm::close()
{
    if(serialDev() && actived()){
        disconnect(serialDev()->getInstance(),SIGNAL(removed()),this,SLOT(slotUSBRemoved()));
    }

    Dmm::close();
}

bool MyDmm::sendCmd(quint8 code, const QByteArray &data)
{

    Q_UNUSED(code)
    Q_UNUSED(data)

    QByteArray d ;

    emit message("Dmm send cmd: " + arrayToHexString(d));

    return serialDev()->write(d);
}

QString MyDmm::arrayToHexString(const QByteArray &array)
{
    QString str;
    QString s;
    for(int i=0;i<array.size();i++)
        str += s.sprintf("%02x ",(quint8)array.at(i));
    return str.toUpper();
}

void MyDmm::decodeFrame(const QByteArray &data)
{
    m_msgType = (quint8)data.at(3);
    switch (m_msgType) {
    case 0://device id
        m_deviceID = QString::fromLatin1(pointFromArray(data,4),20).trimmed();
        break;
    case 1://live data
        parseLiveData(data);
        break;
    case 2://store data

        break;
    case 3://result

        break;
    default:
        break;
    }

}

bool MyDmm::parseReceiveData()
{
    DataState *ds = m_ds;


    if(m_receivedData.size() < 5)//header*2 + length*1 + checksum * 2
        return false;

    if((quint8)m_receivedData.at(0) != 0xAA || (quint8)m_receivedData.at(1) != 0xBB){//检查帧头
        m_receivedData.clear();
        return false;
    }

    quint8 len = (quint8)m_receivedData.at(2) & 0x00ff;
    if(m_receivedData.size() - 2 - 1 < len)//不足一帧
        return false;

    ds->data.clear();
    ds->data.append(m_receivedData,len + 3);//一帧数据放到全局变量

    m_receivedData.remove(0,len + 3);//删除已处理的数据

    return true;
}

void MyDmm::parseLiveData(const QByteArray &data)
{
//    double value = valueFromArray(arr);
//    setValue(value);
//    setValueString(QString::number(value));

//    setUnit(QString::fromLatin1(pointFromArray(arr,10),3).trimmed());
//    setBasis((quint8)arr.at(13) == 0x46?"Fe":"NFe");
//    setContinuousMode((quint8)arr.at(14) == 0x31?true:false);

//    quint8 status = (quint8)arr.at(15);
//    setAutoPower(status & 0x80 ? true:false);
//    setLowBat(status & 0x40 ? true:false);

}

char *MyDmm::pointFromArray(const QByteArray &arr, quint32 pos)
{
    char * p = (char *)arr.data();
    p += pos;

    return p;
}

double MyDmm::valueFromArray(const QByteArray &arr)
{
    double result = 0.0;


    QString str = QString::fromLatin1(pointFromArray(arr,4),6);

    result = str.toDouble();

    return result;
}


void MyDmm::slotSerialDevRead(const QByteArray &data)
{
    static int cnt = 0;
//    QString s;
//    static bool clear = false;
//    s.sprintf("")

//    cdebug << ++cnt  <<"MyDmm::slotSerialDevRead" << arrayToHexString(data);
//    emit message(QString("Received data(%1):%2").arg(cnt).arg(arrayToHexString(data)));

    m_receivedData.append(data);

    DataState *ds = m_ds;

    while(parseReceiveData()){

        emit message(QString("Dmm Received: %1").arg(arrayToHexString(ds->data)));

        decodeFrame(ds->data);
    }


}

void MyDmm::slotUSBRemoved()
{
    close();
}
