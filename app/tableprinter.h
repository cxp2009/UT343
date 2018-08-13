#ifndef TABLEPRINTER_H
#define TABLEPRINTER_H

#include <QAbstractTableModel>
#include <QObject>
#include <QtPrintSupport/QPrinter>
#include <QTableView>
#include <dmmmodel.h>

class Column{
public:
    Column(const QString &title = QString(),int factor = 1){
        m_factor = factor;
        m_title = title;
    }
    QString m_title;
    QRect m_rect;
    int m_factor;
};

class TablePrinter : public QObject
{
    Q_OBJECT
public:
    explicit TablePrinter(DMMModel * models, QObject *parent = 0);
    ~TablePrinter();

    void print();

    QString title() const;
    void setTitle(const QString &title);

    QList<Column *> cols() const;
    void setCols(const QList<Column *> &cols);

private:
    DMMModel * m_model;
    QString m_title;
    QList<Column *> m_cols;


signals:

public slots:
    void slotPaintRequested ( QPrinter * printer );
};

#endif // TABLEPRINTER_H
