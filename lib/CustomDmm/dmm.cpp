#include "dmm.h"

Dmm::Dmm(QObject *parent) : QObject(parent),
    m_actived(false)
{

}

Dmm::~Dmm()
{

}

QObject *Dmm::getInstance()
{
    return this;
}
/**
 * @brief DMM::open
 * 打开
 * @return
 */
bool Dmm::open(SerialDeviceInf *dev)
{
    if(actived())
        return false;

    m_serialDev = dev;

    connect(m_serialDev->getInstance(),SIGNAL(opened()),this,SIGNAL(opened()));
    connect(m_serialDev->getInstance(),SIGNAL(closed()),this,SIGNAL(closed()));
    connect(m_serialDev->getInstance(),SIGNAL(read(QByteArray)),this,SLOT(slotSerialDevRead(QByteArray)));


    m_actived = m_serialDev->open();
    return m_actived;
}
/**
 * @brief DMM::close
 * 关闭
 */
void Dmm::close()
{    
    if(actived()){
        serialDev()->close();

        disconnect(m_serialDev->getInstance(), SIGNAL(opened()),this,SIGNAL(opened()));
        disconnect(m_serialDev->getInstance(),SIGNAL(closed()),this,SIGNAL(closed()));
        disconnect(m_serialDev->getInstance(),SIGNAL(read(QByteArray)),this,SLOT(slotSerialDevRead(QByteArray)));

        m_actived = false;
        m_serialDev = NULL;
    }

}
SerialDeviceInf *Dmm::serialDev() const
{
    return m_serialDev;
}

void Dmm::setSerialDev(SerialDeviceInf *serialDev)
{
    m_serialDev = serialDev;
}
bool Dmm::actived() const
{
    return m_actived;
}

//QList<DmmData *> Dmm::dmmDatas() const
//{
//    return m_dmmDatas;
//}

//void Dmm::setDmmDatas(const QList<DmmData *> &dmmDatas)
//{
//    m_dmmDatas = dmmDatas;
//}





