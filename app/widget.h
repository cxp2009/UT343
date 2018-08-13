#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>
//#include <visa.h>
//#include "visausb.h"
#include <QAbstractSocket>


class QToolBar;
class QAction;
class VISAUSB;
class dlgMath;
//class LanDev;
//class SerialDev;
class FunInfo;
class DataModel;
class QFile;
class DmmData;
class QProgressBar;
class MyCoreInterface;
class CP2110Inf;
class QStandardItemModel;
class SerialRS232Inf;
class SerialRS232VirInf;
class IDmm;
class SerialDeviceInf;
class Histogram;
class DataLimit;

namespace Ui {
class Widget;
}

class Widget : public QMainWindow
{
    Q_OBJECT

public:

    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *event);
    virtual bool eventFilter(QObject *watched, QEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    Ui::Widget *ui;
    IDmm * m_dmm;
    SerialDeviceInf * m_dev_ = NULL;
    SerialRS232VirInf * m_dev = NULL;
//    SerialRS232Inf * m_dev = NULL;
    QToolBar * m_toolBar;
//    QToolBar * m_tb2;
    DataModel * m_dataModel;
    QString m_calPort;
    MyCoreInterface * m_myCore;
    QIcon m_itemIcon;
    QStandardItemModel * m_itemModel;
    Histogram * m_histogram;
    QList<DataLimit *> m_histogramLimits;

    void msg(const QString &m);
private slots:
    void initObj();
    bool loadPlugins();
    void initConnec();
    void initWidget();
    void initSelf();
    void saveSettings();
    void restoreSettings();   

    void slotAction();
    void slotDmmOpened();
    void slotDmmClosed();
    void slotUSBRefreshed();
    void on_actionTest_triggered();
    void on_actionManual_triggered();
    void on_actionClearMsg_triggered();
    void on_actionItemNew_triggered();
    void on_actionItemDelete_triggered();
    void on_actionItemRename_triggered();

    void slotMessage(const QString &msg);

    void on_tvItems_clicked(const QModelIndex &index);

    void slotRS232Read(const QByteArray & data);

//    void slotUsbRemoved();
//    void slotusbArrival();

//    void slotTvCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
};

#endif // WIDGET_H
