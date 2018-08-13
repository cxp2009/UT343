#include <QObject>

class MyCoreInterface{
public:
    virtual bool init() = 0;
    virtual void unInit() = 0;
};

Q_DECLARE_INTERFACE(MyCoreInterface,"com.xp.MyCoreInterface")
