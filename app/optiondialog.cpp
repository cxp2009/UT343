#include "optiondialog.h"
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QComboBox>
#include <QSerialPort>
#include <QSerialPortInfo>


OptionDialog::OptionDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Options"));

    m_cbComNum = new QComboBox;

    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        m_cbComNum->addItem(info.portName());
    }

    QGridLayout * lay = new QGridLayout;
    lay->addWidget(new QLabel(tr("Cal Port")),0,0,Qt::AlignRight);
    lay->addWidget(m_cbComNum,0,1);


    m_okBtn = new QPushButton(tr("Ok"));
    m_cancelBtn = new QPushButton(tr("Cancel"));
    QHBoxLayout *layBtn = new QHBoxLayout;
    layBtn->addStretch();
    layBtn->addWidget(m_okBtn);
    layBtn->addWidget(m_cancelBtn);

    QVBoxLayout * layV = new QVBoxLayout;
    layV->addLayout(lay);
    layV->addLayout(layBtn);

    setLayout(layV);


    connect(m_okBtn,SIGNAL(clicked()),this,SLOT(slotBtnClicked()));
    connect(m_cancelBtn,SIGNAL(clicked()),this,SLOT(slotBtnClicked()));

}

OptionDialog::~OptionDialog()
{

}

void OptionDialog::slotBtnClicked()
{
    if(sender() == m_okBtn)
        accept();
    else
        reject();
}




