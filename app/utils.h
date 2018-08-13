#ifndef UTILS_H
#define UTILS_H

#include <QDebug>

//class Utils
//{
//public:
//    Utils();
//};

class NullDebug
{
public:
    template <typename T>
    NullDebug& operator << (const T&){
        return *this;
    }
};

inline NullDebug nullDebug(){
    return NullDebug();
}

#ifdef DEBUG_EN
#    define cdebug qDebug()
#else
#    define cdebug nullDebug()
#endif

#endif // UTILS_H
