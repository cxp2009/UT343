#ifndef DMM_H
#define DMM_H

#include <QObject>
#include "dmmdata.h"
#include "serialdeviceinf.h"
#include "idmm.h"

class DataState;
class CustomDmmStatus;

/**
 * @brief The DMM class
 * 代表一个万用表的类
 */
class Dmm : public QObject,public IDmm
{
    Q_OBJECT

    Q_INTERFACES(IDmm)
public:
    explicit Dmm(QObject *parent = 0);
    ~Dmm();

    virtual QObject * getInstance();
    virtual bool open(SerialDeviceInf *dev);
    virtual void close();

    SerialDeviceInf *serialDev() const;
    void setSerialDev(SerialDeviceInf *serialDev);

    bool actived() const;

    DataState *ds() const;
    void setDs(DataState *ds);

protected:
    virtual void decodeFrame(const QByteArray &data) = 0;

private:
    SerialDeviceInf  * m_serialDev = NULL ;  //包含一个串行数据端口
    bool m_actived;

signals:
    void opened();
    void closed();
    void message(const QString&msg);
    void display(const DmmData * data);

public slots:
    virtual void slotSerialDevRead(const QByteArray &data) = 0;//子类必须重实现，用来接收数据
};




#endif // DMM_H
