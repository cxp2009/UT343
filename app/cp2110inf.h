#ifndef CP2110INF_H
#define CP2110INF_H

#include <QObject>

class CP2110Inf{
public:
    virtual void setFileName(const QString &fileName) = 0;
    virtual bool refreshUsbs() = 0;
    virtual int usbCount() = 0;

};

Q_DECLARE_INTERFACE(CP2110Inf,"com.xp.CP2110Inf")

#endif // CP2110INF_H
