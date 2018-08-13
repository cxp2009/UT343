#ifndef DMMMODEL_H
#define DMMMODEL_H

#include <QAbstractTableModel>
#include "dmmdata.h"
#include <QMutex>
#include "ut8805.h"

class DMMModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DMMModel(UT8805 * dmm, QObject *parent = 0);
    ~DMMModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;

    int addData(const DmmData *data);
    void clear();

private:
    QList<const DmmData *> m_datas;
    UT8805 * m_dmm;
signals:

public slots:
};

#endif // DMMMODEL_H
