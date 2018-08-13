#include "serialrs232.h"
#include <Windows.h>
#include "utils.h"

SerialRS232::SerialRS232(QObject *parent) : QObject(parent),
    m_actived(false),
    m_readThread(NULL)
{
    m_buffer.clear();
    m_port = new QSerialPort();
    connect(m_port,&QSerialPort::readyRead,this,&SerialRS232::slotPortReadyRead);
}

bool SerialRS232::open()
{
    bool rst = false;

    if(m_actived)
        return rst;
    if(m_portInfo.isNull())
        return rst;
    if(m_portInfo.isBusy())
        return rst;

    rst = m_port->open(QIODevice::ReadWrite);
    if(!rst){
//        cdebug << m_port->error() << m_port->errorString();
        return rst;
    }

    //thread
//    m_readThread = new SerialRS232ReadThread(this);
//    m_readThread->transaction();

    m_actived = true;

    emit opened();

    rst = true;
    return rst;
}

bool SerialRS232::open(QSerialPortInfo portInfo, QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits,
                       QSerialPort::StopBits stopBits,QSerialPort::Parity parity)
{    
    setParameters(portInfo,baudRate,dataBits,stopBits,parity);

    return SerialRS232::open();
}

void SerialRS232::setParameters(QSerialPortInfo portInfo, QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits,
                                 QSerialPort::StopBits stopBits,QSerialPort::Parity parity)
{
    m_portInfo = portInfo;
    m_port->setPort(m_portInfo);
    m_port->setBaudRate(baudRate);
    m_port->setDataBits(dataBits);
    m_port->setParity(parity);
    m_port->setStopBits(stopBits);
}

void SerialRS232::close()
{
//    if(m_readThread){
//        m_readThread->stop();
//        if(!m_readThread->wait(5000))
//            cdebug << "Failed to close SerialRS232ReadThread";
//        delete m_readThread;
//        m_readThread = NULL;
//    }

    m_port->close();
    m_actived = false;

    emit closed();
}

int SerialRS232::write(QByteArray &data)
{
    if(!m_actived)
        return -1;
    return m_port->write(data.constData());
}

QObject *SerialRS232::getInstance()
{
    return this;
}

QList<QSerialPortInfo> SerialRS232::availablePorts() const
{
    return QSerialPortInfo::availablePorts();
}

void SerialRS232::appendData(const QByteArray &data)
{
    m_buffer = data;

    emit read(m_buffer);
}

QSerialPortInfo SerialRS232::getPortInfo() const
{
    return m_portInfo;
}

void SerialRS232::setPortInfo(const QSerialPortInfo &portInfo)
{
    m_portInfo = portInfo;
}

QSerialPort *SerialRS232::getPort() const
{
    return m_port;
}

bool SerialRS232::getActived() const
{
    return m_actived;
}

void SerialRS232::setActived(bool actived)
{
    m_actived = actived;
}

void SerialRS232::slotPortReadyRead()
{
    int cnt = m_port->bytesAvailable();
    if(cnt > 0){
        m_buffer = m_port->read(cnt);
        emit read(m_buffer);
    }
}


SerialRS232ReadThread::SerialRS232ReadThread(SerialRS232 *dev)
{
    m_dev = dev;
}

void SerialRS232ReadThread::transaction()
{
    if(!isRunning())
        start();
}

void SerialRS232ReadThread::stop()
{
    m_stoped = true;
}

void SerialRS232ReadThread::run()
{
    if(!m_dev)
        return;
    m_stoped = false;

    int len;

    while(!m_stoped){
        if(m_dev->getPort()->waitForReadyRead(1000)){
            len = m_dev->getPort()->bytesAvailable();
            m_data = m_dev->getPort()->read(len);

//            while(m_dev->getPort()->waitForReadyRead()){
//                QByteArray d = m_dev->getPort()->readAll();
//                if(d.size() > 0)
//                    data += d;
//            }

            if(m_data.size() > 0)
                m_dev->appendData(m_data);
        }
    }
    m_stoped = true;
}
