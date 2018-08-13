#include <QtCore>
#include <QDebug>
#include "mycore.h"
#include <Windows.h>



static const quint32 PWD_CHAR = 0x5aa5a55a;
static const quint32 MAX_RUN_CNT = 183;
static const quint32 MAX_RUN_DAY = 182;

MyCore::MyCore(QObject *parent) : QObject(parent)
{

}

bool MyCore::init()
{
    bool rst;
    rst = getAccess();

    return rst;
}

void MyCore::unInit()
{
    setAccess();
    processFile();
}
/**
 * @brief MyCore::getAccess
 * @return
 */
bool MyCore::getAccess()
{
    quint32 pwd;
    uint firstRunTime;
    QDateTime currTime = QDateTime::currentDateTime();

    QString str = qApp->applicationDirPath() + "/" + qApp->applicationName() + ".exe";
    QFile f(str);
    if(f.open(QIODevice::ReadOnly)){

        QDataStream in(&f);
        in.setVersion(QDataStream::Qt_5_8);

        f.seek(f.size()-4);
        in >> (quint32)pwd;

        if(pwd != PWD_CHAR){//第一次运行
            m_firstRunTime  = currTime;
            m_runCount = 1;
        }else {
            f.seek(f.size() -4 - 4);
            in >> (quint32)m_runCount;
            f.seek(f.size() -4 - 4 - 4);
            in >> (uint)firstRunTime;

            m_runCount++;
            m_firstRunTime = QDateTime::fromTime_t(firstRunTime);

            //如果当前时间小于第一次运行时间，就涉嫌修改时间
            //如果当前时间大于限制时间，就使用时间到
            if((currTime <= m_firstRunTime) || (currTime >= m_firstRunTime.addDays(MAX_RUN_DAY)))
                return false;
            if(m_runCount >= MAX_RUN_CNT)
                return false;

        }
        f.close();
    }

    return true;
}
/**
 * @brief MyCore::setAccess 在exe文件末尾保存首次运行时间、运行次数、标准码
 */
void MyCore::setAccess()
{
    QString str = qApp->applicationDirPath() + "/" + qApp->applicationName() + ".exe";
    QFile f(str);
    if(f.open(QIODevice::ReadOnly)){
//        QFileInfo fi(f);

        QDataStream in(&f);
        in.setVersion(QDataStream::Qt_5_8);

        QFile f2(qApp->applicationDirPath() + "/" + qApp->applicationName() + "_1.exe");
        if(f2.open(QIODevice::ReadWrite)){
            QDataStream out(&f2);
            out.setVersion(QDataStream::Qt_5_8);

            quint8 b;
            while(!in.atEnd()){
                in >> b;
                out << b;
            }

            if(m_runCount == 1){
                uint t = m_firstRunTime.toTime_t() ;
                out << (uint)t;
                out << (quint32)m_runCount;
                out << (quint32)PWD_CHAR;

                QDateTime d = QDateTime::fromTime_t(t);
//                qDebug() << t << d;
            }else{
                f2.seek(f2.size() - 4 - 4);
                out << m_runCount;
                f2.seek(f2.size() - 4 - 4 - 4);
                out << (uint)m_firstRunTime.toTime_t();
            }

            f2.close();
        }

        f.close();
    }
}
/**
 * @brief MyCore::processFile 创建一个bat文件，用来删除自身和替换自身
 */
void MyCore::processFile()
{
    QDir path(qApp->applicationDirPath());
    QFile f(path.absoluteFilePath("1.bat"));
    if(f.open(QIODevice::ReadWrite)){
        QTextStream ts(&f);

        QString str = path.absoluteFilePath(qApp->applicationName()) + ".exe";
        str.replace("/","\\");
        ts << ":Repeat\r\n";
        ts << "del /Q \"" << str << "\"\r\n" ;
        ts << "if exist \"" << str << "\" goto Repeat\r\n";
        ts << "ren \"" << QString(path.absoluteFilePath(qApp->applicationName()) + "_1.exe\" \"").replace("/","\\")
           << qApp->applicationName() + ".exe\" \r\n";
        str = f.fileName();
        str.replace("/","\\");
        ts << "del /Q \"" << str  << "\"\r\n";
        f.close();

        ShellExecute(NULL,(LPCWSTR)QString("open").constData(),(LPCWSTR)str.constData(),
                     (LPCWSTR)QString("").constData(),NULL,SW_HIDE);

    }
}


//Q_EXPORT_PLUGIN2("MyCore",MyCore)

