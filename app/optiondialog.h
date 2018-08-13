#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>

class QPushButton;
class QComboBox;

class OptionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OptionDialog(QWidget *parent = 0);
    ~OptionDialog();

    QComboBox * m_cbComNum;
    QPushButton * m_okBtn;
    QPushButton * m_cancelBtn;

signals:

public slots:
    void slotBtnClicked();
};

#endif // OPTIONDIALOG_H
