#include "customwidget.h"

CustomWidget::CustomWidget(QWidget *parent) : QWidget(parent)
{

}

QSize CustomWidget::sizeHint() const
{
    QSize rst = QWidget::sizeHint();
    rst.setWidth(100);
    return rst;
}
