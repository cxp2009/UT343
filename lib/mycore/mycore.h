#ifndef MYCORE_H
#define MYCORE_H

#include <QObject>
#include <QDateTime>
#include "mycoreinterface.h"

class MyCore : public QObject,public MyCoreInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.xp.MyCoreInterface"/* FILE "extrafilters.json"*/)
    Q_INTERFACES(MyCoreInterface)
public:
    explicit MyCore(QObject *parent = 0);
    virtual bool init();
    virtual void unInit();

private:
    QDateTime m_firstRunTime;//第一次运行时间
    quint32 m_runCount;//运行次数

    bool getAccess();
    void setAccess();
    void processFile();

signals:

public slots:
};

#endif // MYCORE_H
