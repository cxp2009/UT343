#ifndef COMPORT_H
#define COMPORT_H

#include <QObject>
#include <windows.h>
#include <QSerialPort>
#include <QThread>

class ComPortReadThread : public QThread
{
    Q_OBJECT
public:
    explicit ComPortReadThread(QObject *parent = 0);

    void transaction(HANDLE port);
    void stop();
protected:
    void run();
private:
    void WaiteData();

    bool m_stopped;
    HANDLE m_port;

    OVERLAPPED Olp_Read;
    HANDLE Close_Event;
    HANDLE EventHandles[2];
    QByteArray m_readData;

public slots:

signals:
    void read(QByteArray data);
};

class ComPort : public QObject
{
    Q_OBJECT
public:
    explicit ComPort(QObject *parent = 0);
    ~ComPort();

    bool open(const QString name, DWORD baudRate, BYTE dataBits, BYTE stopBits,
              BYTE parity);
    void setParam(quint32 baudRate, quint8 dataBits, quint8 stopBits, quint8 parity);
    void close();
    void write(const QByteArray &data);
    bool isOpen() const;
    void setIsOpen(bool isOpen);

private:

    HANDLE m_handle;
    ComPortReadThread *m_readThread;

    QString m_name;
    DWORD m_baudRate;
    BYTE m_dataBits;
    BYTE m_stopBits;
    BYTE m_parity;
    bool m_isOpen = false;
signals:


public slots:
    void slotRead(QByteArray data);


};

#endif // COMPORT_H
