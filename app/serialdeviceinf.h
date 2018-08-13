#ifndef SERIALDEVICEINF_H
#define SERIALDEVICEINF_H

#include <QObject>

class SerialDeviceInf{
public:
    virtual bool open() = 0;                    //打开
    virtual void close() = 0;                   //关闭
    virtual int write(QByteArray &data) = 0;  //写数据
    virtual QObject * getInstance() = 0;
};

/**
 * @brief 数据状态，用于读取一帧数据
 */
class DataState
{
public:
    DataState(){
        reset();
    }

    void reset(){
        flag = 0;
        remain = -1;
        data.clear();
    }

    int flag; // 0:读帧头  1:读长度 2：读数据;3: 一帧数据读取完成
    int remain;     //剩下数据的字节数，一般由数据包中的length指定
    QByteArray data;    //存放数据
};

Q_DECLARE_INTERFACE(SerialDeviceInf,"com.xp.SerialDeviceInf")

#endif // SERIALDEVICEINF_H
