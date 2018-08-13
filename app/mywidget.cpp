#include "mywidget.h"

MyWidget::MyWidget(QWidget *parent) : QWidget(parent)
{

}

QSize MyWidget::sizeHint() const
{
    QSize rst = QWidget::sizeHint();
    rst.setWidth(280);
    return rst;
}
