#ifndef SERIALDEV_H
#define SERIALDEV_H

#include <QObject>
#include <QThread>

//class QThread;
class ReadThread;
class DataState;

/**
 * @brief The SerialDev class
 * 串行通信端口，一个虚函数
 */
class SerialDev : public QObject
{
    Q_OBJECT
public:
    explicit SerialDev(QObject *parent = 0);

    virtual bool open();                    //打开
    virtual void close();                   //关闭
    virtual int write(QByteArray &data)=0;  //写数据
    Q_INVOKABLE virtual void readData() = 0;//读数据

    virtual bool actived() const;
    virtual void setActived(bool actived);

    DataState *ds() const;
    void setDs(DataState *ds);

private:
    bool m_actived;                         //打开状态
    ReadThread * m_readThread;              //读数据线程
    DataState *m_ds;                        //用于读取数据

signals:
    void opened();                          //打开后发送的信号
    void closed();                          //关闭后发送的信号
    void read(const QByteArray &data);            //读数据的信号

public slots:
};

/**
 * @brief The ReadThread class
 * 读数据线程
 */
class ReadThread: public QThread{
public:
    ReadThread(SerialDev *dev, QObject *parent = 0);
    void transaction();                     //开始
    void stop();                            //停止
    DataState *ds() const;
    void setDs(DataState *ds);

protected:
    void run();
private:
    bool m_stopped;                         //是否停止
    SerialDev * m_dev;                      //一个串行通信端口

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


#endif // SERIALDEV_H
