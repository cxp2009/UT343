#include "comport.h"
#include <QMutexLocker>
#include <string>
#include <QDebug>


ComPort::ComPort(QObject *parent) : QObject(parent)
{
    m_handle = NULL;
    m_baudRate = 9600;
    m_dataBits = 8;
    m_stopBits = ONESTOPBIT;
    m_parity = NOPARITY;

    m_readThread = new ComPortReadThread;
    connect(m_readThread,SIGNAL(read(QByteArray)),this,SLOT(slotRead(QByteArray)));
}

ComPort::~ComPort()
{
    delete m_readThread;
}
/**
 * @brief ComPort::open
 * @param name  COMX
 * @param baudRate
 * @param dataBits  4~8
 * @param stopBits ONESTOPBIT,ONE5STOPBITS,TWOSTOPBITS
 * @param parity  NOPARITY,ODDPARITY,EVENPARITY,MARKPARITY,SPACEPARITY
 * @return
 */
bool ComPort::open(const QString name,DWORD baudRate,BYTE dataBits,
                   BYTE stopBits,BYTE parity)
{
    if(m_isOpen)
        return true;

    m_name = name;
    m_baudRate = baudRate;
    m_dataBits = dataBits;
    m_stopBits = stopBits;
    m_parity = parity;

    QString str = name;

    if(str.toUpper().startsWith("COM")){
        if(str.right(m_name.size() - 3).toInt() >= 10)
            str = "\\\\.\\" + str;
    }

    m_handle = CreateFile((LPCWSTR)str.unicode(),
                          GENERIC_READ  | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,
                          NULL);
    m_isOpen = m_handle != INVALID_HANDLE_VALUE;

    if(m_isOpen){

        if(!SetupComm(m_handle,5120,2048))
            qDebug() << GetLastError();

        DCB dcb;
        GetCommState(m_handle, &dcb);
        dcb.BaudRate = m_baudRate;//
        dcb.fBinary = 1;
        dcb.fParity = 1;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.ByteSize = m_dataBits;
        dcb.Parity = m_parity;
        dcb.StopBits = m_stopBits;
        if(!SetCommState(m_handle, &dcb))
            qDebug() << GetLastError();

        COMMTIMEOUTS ct;
        GetCommTimeouts(m_handle,&ct);
        ct.ReadIntervalTimeout = 300;
        ct.ReadTotalTimeoutMultiplier = 0;
        ct.ReadTotalTimeoutConstant = 0;
        ct.WriteTotalTimeoutMultiplier = 0;
        ct.WriteTotalTimeoutConstant = 0;
        if(!SetCommTimeouts(m_handle,&ct))
            qDebug() << GetLastError();

        if(!SetCommMask(m_handle,EV_RXCHAR))
            qDebug() << GetLastError();

        PurgeComm(m_handle,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);


    }

    if(m_isOpen)
        m_readThread->transaction(m_handle);



    return m_isOpen;
}

void ComPort::close()
{
    if(m_readThread->isRunning()){
        m_readThread->stop();
    }

    if((m_handle != INVALID_HANDLE_VALUE) && (m_handle != NULL)){
        CloseHandle(m_handle);
        m_handle = NULL;
    }
    m_isOpen = false;
}

void ComPort::write(const QByteArray &data)
{
    DWORD nWrite;

    OVERLAPPED Olp_Write;
    unsigned long h;

    memset(&Olp_Write,0,sizeof(Olp_Write));
    Olp_Write.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

    if(m_handle != INVALID_HANDLE_VALUE)
    {

        if(!WriteFile(m_handle,data.constData(),data.size(),&nWrite,&Olp_Write)){
            h = WaitForSingleObject(Olp_Write.hEvent,1000);
            if(h == WAIT_OBJECT_0)
                GetOverlappedResult(m_handle,&Olp_Write,&nWrite,false);
        }



    }
}

bool ComPort::isOpen() const
{
    return m_isOpen;
}

void ComPort::setIsOpen(bool isOpen)
{
    m_isOpen = isOpen;
}

void ComPort::slotRead(QByteArray data)
{

}

void ComPort::setParam(quint32 baudRate, quint8 dataBits, quint8 stopBits,
                       quint8 parity)
{
    m_baudRate = baudRate;
    m_dataBits = dataBits;
    m_stopBits = stopBits;
    m_parity = parity;

}


//////////////////////////////////////////////////////////////////////////////////////
ComPortReadThread::ComPortReadThread(QObject *parent) : QThread(parent),
    m_stopped(false)

{

}


void ComPortReadThread::transaction(HANDLE port)
{
    m_port = port;
    if(!this->isRunning()){
        start();
    }
}

void ComPortReadThread::stop()
{
    m_stopped = true;
    SetEvent(Close_Event);
    if(!wait(1000))//等待run()返回
        qDebug("Failed to stop the ThreadRead.");
}

void ComPortReadThread::run()
{
    DWORD waitemul;

    m_stopped = false;
    qDebug("ReadThread has started.") ;

    memset(&Olp_Read,0,sizeof(Olp_Read));
    Olp_Read.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    Close_Event = CreateEvent(NULL,TRUE,FALSE,NULL);
    EventHandles[0] = Close_Event;
    EventHandles[1] = Olp_Read.hEvent;

    WaiteData();
    while(!m_stopped){
        waitemul = WaitForMultipleObjects(2,EventHandles,FALSE,300);
        switch(waitemul)
        {
        case WAIT_OBJECT_0:
            m_stopped = true;
            continue;
            break;
        case WAIT_OBJECT_0 + 1:
            WaiteData();
            break;

        }

//        msleep(1);
    }

    if(Close_Event != NULL)
    {
        CloseHandle(Close_Event);
        Close_Event = NULL;
    }
    if(Olp_Read.hEvent != NULL)
    {
        CloseHandle(Olp_Read.hEvent);
        Olp_Read.hEvent = NULL;
    }

    qDebug("ReadThread has stopping.") ;
}

void ComPortReadThread::WaiteData()
{
    DWORD evtMask = 0;
    DWORD dwOvres = 0;
    DWORD waiteResult;
    DWORD errors;
    COMSTAT comstat;
    DWORD cbNum;
    DWORD cbread;
    bool rst;
    char *buff;

    rst = WaitCommEvent(m_port,&evtMask,&Olp_Read);
    if(!rst){
        if(GetLastError() == ERROR_IO_PENDING){
            waiteResult = WaitForSingleObject(Olp_Read.hEvent,500)   ;
            switch(waiteResult){
            case WAIT_OBJECT_0:
                rst = GetOverlappedResult(m_port,&Olp_Read,&dwOvres,false);
                break;
            case WAIT_TIMEOUT:
                rst = false;
                break;
            default:
                rst = false;
                break;
            }
        }
    }
    if(rst){
        if(ClearCommError(m_port,&errors,&comstat)){
            cbNum = comstat.cbInQue;
            if(cbNum > 0){
                buff = new char[cbNum];
                ReadFile(m_port,buff,cbNum,&cbread,&Olp_Read);

                m_readData = QByteArray(buff,cbNum);
//                qDebug() << cbNum << cbread << m_readData.toHex();
                emit read(m_readData);
                delete buff;
            }

        }
    }

}


