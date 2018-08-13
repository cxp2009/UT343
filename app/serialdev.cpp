#include "serialdev.h"

SerialDev::SerialDev(QObject *parent) : QObject(parent),
    m_actived(false),
    m_ds(new DataState)
{

}

bool SerialDev::open()
{
    m_readThread = new ReadThread(this);
    m_readThread->transaction();

    setActived(true);
    emit opened();

    return true;
}

void SerialDev::close()
{
    if(m_readThread && m_readThread->isRunning()){
        m_readThread->stop();
        delete m_readThread;
        m_readThread = NULL;
    }

    m_ds->reset();

    if(actived()){
        setActived(false);
        emit closed();
    }
}
bool SerialDev::actived() const
{
    return m_actived;
}

void SerialDev::setActived(bool actived)
{
    if(m_actived != actived)
        m_actived = actived;
}
DataState *SerialDev::ds() const
{
    return m_ds;
}

void SerialDev::setDs(DataState *ds)
{
    m_ds = ds;
}


ReadThread::ReadThread(SerialDev *dev, QObject *parent):QThread(parent),
    m_stopped(true),
    m_dev(dev)
{

}


void ReadThread::transaction()
{
    if(!this->isRunning()){
        start();
    }
}

void ReadThread::stop()
{
    m_stopped = true;
    if(!wait(3000))//等待run()返回
        qDebug("Failed to stop the ThreadTask.");
}

void ReadThread::run()
{
    m_stopped = false;
    while(!m_stopped){
        if(m_dev)
            QMetaObject::invokeMethod(m_dev,"readData");
//            m_dev->readData();
        msleep(100);
    }
}

