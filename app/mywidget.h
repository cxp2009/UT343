#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>

class MyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MyWidget(QWidget *parent = 0);

protected:
    QSize sizeHint() const;

signals:

public slots:
};

#endif // MYWIDGET_H
