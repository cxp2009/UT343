#include "widget.h"
#include "ui_widget.h"

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QToolBar>
#include <QAction>
#include <QDebug>
#include <QVariant>
#include <QMessageBox>
#include <QFileDialog>
#include <QAxObject>
#include <QDesktopWidget>
#include <QStyle>
#include <QSettings>
#include <QPushButton>
#include <QToolButton>
#include <QProcess>
#include <QBuffer>
#include <QPluginLoader>
#include <QFile>
#include "optiondialog.h"
#include "utils.h"
#include "datamodel.h"
#include "dmmdata.h"
#include <QXmlStreamReader>
#include <QContextMenuEvent>
#include <QtMath>
#include <QMetaEnum>
#include <QProgressBar>
#include <QDesktopServices>
#include <QTimer>
#include <QStandardItem>
#include "mycoreinterface.h"
#include "cp2110inf.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include "serialrs232inf.h"
#include "serialrs232virinf.h"
#include "serialdeviceinf.h"
#include "idmm.h"


const static char TITLE[] = "UT343";   //型号
const static char VERSION[] = "1.0";    //版本

Widget::Widget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    if(!loadPlugins()){
        QTimer::singleShot(0,qApp,QApplication::quit);
    }

    initObj();

    initWidget();

    initSelf();

    initConnec();    

}

Widget::~Widget()
{
    m_dmm->close();
    delete ui;

//    QDir path(qApp->applicationDirPath());
//    QPluginLoader loader(path.absoluteFilePath("MyCore.dll"));

//    QObject * obj = loader.instance();
//    if(obj){
//        MyCoreInterface *m_myCore = qobject_cast<MyCoreInterface *>(obj);
//        m_myCore->unInit();
//    }

}

void Widget::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
//    restoreSettings();
}

void Widget::closeEvent(QCloseEvent *event)
{    
    m_dmm->close();

    saveSettings();
    QMainWindow::closeEvent(event);
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    QMenu * m;

    if(event->type() == QEvent::ContextMenu){        
        QContextMenuEvent *e = static_cast<QContextMenuEvent *>(event);

        if(watched->objectName() == "tv" ){//listWidget
            return false;
        } else if(watched->objectName() == "listWidget" ){
            m = new QMenu(this);
            m->setAttribute(Qt::WA_DeleteOnClose);
            m->addAction(ui->actionClearMsg);

            m->exec(e->globalPos());
        }else if(watched->objectName() == "tvItems"){
            m = new QMenu(this);
            m->setAttribute(Qt::WA_DeleteOnClose);
            m->addAction(ui->actionItemNew);
            m->addAction(ui->actionItemRename);
            m->addAction(ui->actionItemDelete);
            m->exec(e->globalPos());
        }
        return QWidget::eventFilter(watched,event);
    }else{
         return QWidget::eventFilter(watched,event);
    }
}

void Widget::timerEvent(QTimerEvent *event)
{
    int n = qrand()%500;
    m_histogram->append(n);
}

void Widget::msg(const QString &m)
{
    ui->listWidget->addItem(m);
}

/**
 * @brief Widget::initObj 初始化需要的object
 */
void Widget::initObj(){
    m_dataModel = new DataModel;

    m_histogram = ui->Histogram1;
    m_histogram->setMargins(QMargins(30,20,20,30));

    DataLimit *dl;
    QList<DataLimit *> dls;


    for(int i=0;i<5;i++){
        dl = new DataLimit(100 * i + 1,(i+1) * 100);
        dls.append(dl);
    }

    m_histogram->setLimits(dls);
}

bool Widget::loadPlugins()
{
    SerialDeviceInf * dev_ = NULL;
    SerialRS232VirInf * dev = NULL;
    IDmm * dmm = NULL;

    QDir path(qApp->applicationDirPath());
     path.cd("Lib");//进去lib目录

    foreach (QString fileName, path.entryList()) {
        QFileInfo fi(path.absoluteFilePath(fileName));
        if(!fi.isFile() || fi.suffix().toLower() != "dll")//需要为dll文件
            continue;

        QPluginLoader loader(path.absoluteFilePath(fileName));
        QObject * obj = loader.instance();
        if(obj){//具体类型适配
            dev_ = qobject_cast<SerialDeviceInf *>(obj);
            if(dev_)
                m_dev_ = dev_;

            dev = qobject_cast<SerialRS232VirInf *>(obj);
            if(dev){
                m_dev = dev;
            }

            dmm = qobject_cast<IDmm *>(obj);
            if(dmm)
                m_dmm = dmm;

        }
    }

    return !dev_ || !dev || !dmm;
}
/**
 * @brief Widget::initConnec //连接信号和槽
 */
void Widget::initConnec()
{
    connect(m_dmm->getInstance(),SIGNAL(opened()),this,SLOT(slotDmmOpened()));
    connect(m_dmm->getInstance(),SIGNAL(closed()),this,SLOT(slotDmmClosed()));
    connect(m_dmm->getInstance(),SIGNAL(message(QString)),this,SLOT(slotMessage(QString)));
    connect(m_dev_->getInstance(),SIGNAL(usbRefreshed()),this,SLOT(slotUSBRefreshed()));

    //action
    connect(ui->actionConnect,&QAction::triggered,this,&Widget::slotAction);
    connect(ui->actionDisconnect,&QAction::triggered,this,&Widget::slotAction);
    connect(ui->actionOption,&QAction::triggered,this,&Widget::slotAction);
    connect(ui->actionClose,&QAction::triggered,this,&Widget::slotAction);

//    connect(ui->tv->selectionModel(),&QItemSelectionModel::currentRowChanged,this,&Widget::slotTvCurrentRowChanged);
//    connect(ui->tv,&QTableView::customContextMenuRequested,this,&Widget::slottableViewContextMenuRequested);
}
/**
 * @brief Widget::initWidget 初始化Widget
 */
void Widget::initWidget()
{
    QTableView * view = ui->tv;

    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    view->horizontalHeader()->setSectionsClickable(false);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setModel(m_dataModel);
    //设置列宽，需要在setModel后才会有效
//    view->setColumnWidth(0,50);
//    view->setColumnWidth(1,60);
//    view->setColumnWidth(2,80);
//    view->setColumnWidth(3,60);
//    view->setColumnWidth(4,80);
//    view->setColumnWidth(5,40);

    ui->tvItems->installEventFilter(this);
    ui->tv->installEventFilter(this);
    ui->listWidget->installEventFilter(this);


    ui->actionConnect->setEnabled(!m_dev->getUseDev(RS232Vir_VID,RS232Vir_PID).isNull());

    //mainmenu
    QMenu *menu = new QMenu(tr("&File"));
    menu->addAction(ui->actionConnect);
    menu->addAction(ui->actionDisconnect);
    menu->addAction(ui->actionOption);
    menu->addAction(ui->actionClose);
    ui->menuBar->addMenu(menu);

    //init item widget
    m_itemIcon = QIcon(":/image/about.png");
    QTreeView *tv = ui->tvItems;
    m_itemModel = new QStandardItemModel(this);
    m_itemModel->setHorizontalHeaderLabels(QStringList() << "Name" << "Info" << "Checked");
    tv->setModel(m_itemModel);
    QStandardItem *itemParent = m_itemModel->invisibleRootItem();
    for(int i = 0;i<10;i++){
        QStandardItem *item = new QStandardItem(m_itemIcon,QString("Item%1").arg(i+1));
        itemParent->appendRow(item);
        itemParent->setChild(i,1,new QStandardItem(QString("Info%1").arg(i+1)));
//        model->setItem(i,1,new QStandardItem(QString("Info%1").arg(i+1)));
    }
}
/**
 * @brief Widget::initSelf 初始化主窗体
 */
void Widget::initSelf()
{
    this->setWindowTitle(QString("%1 v%2 ").arg(TITLE).arg(VERSION));
    QDesktopWidget * desktop = qApp->desktop();
    int w = desktop->width();
    int h = desktop->height();

    resize(1000,600);
    move((desktop->width()-this->width())/2,(desktop->height()-this->height())/2);

//    删掉CentralWidget
//    QWidget * p = takeCentralWidget();
//    if(p)
//        delete p;
//    setDockNestingEnabled(true);//允许嵌套dock

}

void Widget::saveSettings()
{
    QFile f(qApp->applicationDirPath() + "/options.xml");

    if(!f.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this,"",tr("Can not open file."));
        return;
    }

    QXmlStreamWriter writer(&f);
    writer.setAutoFormatting(true);

    writer.writeStartDocument();
    writer.writeStartElement("options");
    writer.writeStartElement("Geometry");
    QString str;
    str = saveGeometry().toHex();
    writer.writeCharacters(str);
    writer.writeEndElement();

    writer.writeStartElement("State");
    str = saveState().toHex();
    writer.writeCharacters(str);
    writer.writeEndElement();

    writer.writeEndElement();
    writer.writeEndDocument();


    f.close();

}

void Widget::restoreSettings()
{
    QFile f(qApp->applicationDirPath() + "/options.xml");
    if(!f.open(QIODevice::ReadOnly))
        return;

    QXmlStreamReader reader(&f);
    QString n;

    while(!reader.atEnd()){
        QXmlStreamReader::TokenType tt = reader.readNext();
        switch (tt) {
        case QXmlStreamReader::StartElement:
            if(reader.name() != "options"){
                n = reader.name().toString();
            }
            break;
        case QXmlStreamReader::EndElement:

            break;
        case QXmlStreamReader::Characters:
            if(!n.isEmpty() && !reader.isWhitespace()){
                if(n == "Geometry"){
                    this->restoreGeometry(QByteArray::fromHex(reader.text().toString().toLatin1()));
                }else if(n == "State"){
                    this->restoreState(QByteArray::fromHex(reader.text().toString().toLatin1()));
                }
                n = QString();
            }
            break;
        default:
            break;
        }
    }

    f.close();
}


void Widget::slotAction()
{
    QAction * act = qobject_cast<QAction *>(sender());
    if(!act)
        return;
    if(act == ui->actionConnect){        

        if(!m_dmm->open(m_dev_)){
            QMessageBox::critical(this,"",tr("Failed to open device."));
            return;
        }


    }else if(act == ui->actionDisconnect){
        m_dmm->close();

    }else if(act == ui->actionOption){
        OptionDialog *dialog = new OptionDialog(this);
        dialog->m_cbComNum->setCurrentText(m_calPort);
        if(dialog->exec() == QDialog::Accepted){
            m_calPort = dialog->m_cbComNum->currentText();
        }
        delete dialog;

    }else if(act == ui->actionClose){
        close();
    }
}

void Widget::on_actionTest_triggered()
{
    startTimer(500);

}

void Widget::on_actionManual_triggered()
{
    QProcess * p = new QProcess(this);
//    p->start(qApp->applicationDirPath() + "/Manual.pdf",QStringList());
    p->start(qApp->applicationDirPath() + "/Manual.pdf",QStringList(qApp->applicationDirPath() + "/Manual.pdf"));
    p->waitForStarted();
//    QDesktopServices::openUrl(qApp->applicationDirPath() + "/Manual.pdf");
}

void Widget::on_actionClearMsg_triggered()
{
    ui->listWidget->clear();
}

void Widget::on_actionItemNew_triggered()
{
    QStandardItem *itemParent = m_itemModel->invisibleRootItem();
    int row = itemParent->rowCount() + 1;
    QStandardItem *item = new QStandardItem(m_itemIcon,QString("Item%1").arg(row));
    //    item->setData();
    itemParent->appendRow(item);
    itemParent->setChild(row-1,1,new QStandardItem(QString("Info%1").arg(row)));
    item = new QStandardItem();
    item->setCheckState(Qt::Checked);
    m_itemModel->setItem(row -1 ,2,item);

}

void Widget::on_actionItemDelete_triggered()
{
    QModelIndex idx = ui->tvItems->currentIndex();
    if(!idx.isValid())
        return;

    m_itemModel->removeRow(idx.row());
}

void Widget::on_actionItemRename_triggered()
{

}


void Widget::slotMessage(const QString &msg)
{
    this->msg(msg);
    ui->listWidget->scrollToBottom();
}

void Widget::on_tvItems_clicked(const QModelIndex &index)
{
    QString str = QString("row:%1 column:%2:%3").arg(index.row())
            .arg(index.column()).arg(index.data().toString());

    msg(str);
}

void Widget::slotRS232Read(const QByteArray &data)
{
    cdebug << data.toHex();
}


void Widget::slotDmmOpened()
{
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(!ui->actionConnect->isEnabled());

    ui->actionStart->setEnabled(true);

    ui->actionStop->setEnabled(false);
}

void Widget::slotDmmClosed()
{
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(!ui->actionConnect->isEnabled());

    ui->actionStart->setEnabled(false);
    ui->actionStop->setEnabled(false);
}

void Widget::slotUSBRefreshed()
{
    ui->actionConnect->setEnabled(!m_dev->getUseDev(RS232Vir_VID,RS232Vir_PID).isNull());
//    cdebug << "Widget::slotUSBRefreshed()";
}
