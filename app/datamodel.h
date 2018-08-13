#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QAbstractTableModel>
#include "dmmdata.h"
#include <QMutex>

class DataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DataModel(QObject *parent = 0);
    ~DataModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;

    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(QModelIndexList indexList);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
//    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);


    int addData(const DmmData *data);
    void editData(const QModelIndex &index, DmmData *to);
    void insertData(const QModelIndex &index, DmmData *data);
    void clear();
    DmmData * dataAt(int index);
    DmmData * dataAt(const QModelIndex &index);

    QList<const DmmData *> datas() const;


private:
    QList<const DmmData *> m_datas;
signals:

public slots:
};

#endif // DATAMODEL_H
