#include "DataModel.h"
#include <QModelIndex>
#include <QXmlStreamReader>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QModelIndexList>
#include <QVariant>

#include "utils.h"


int static const COL_CNT = 5;

DataModel::DataModel(QObject *parent) : QAbstractTableModel(parent)
{
}

DataModel::~DataModel()
{
    qDeleteAll(m_datas);
    m_datas.clear();
}

int DataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_datas.count();
}

int DataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return COL_CNT;
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    const DmmData * data;

    if(!index.isValid())
        return QVariant();
    if(index.row() >= rowCount())
        return QVariant();
    if(index.column() >= columnCount())
        return QVariant();

    data = m_datas.at(index.row());
    switch(role){
    case Qt::DisplayRole:
        switch(index.column()){
        case 0:
            return index.row() + 1;
            break;
        case 1:
            return data->datetime();
            break;
        case 2:
            return data->valueString();
            break;
        case 3:
            return data->unit();
            break;
        case 4:
            return data->basis();
            break;
        default:
            break;
        }
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
        break;
    }
    return QVariant();
}


QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal){
        switch(section){
        case 0:
            return tr("NO.");
            break;
        case 1:
            return tr("Time");
            break;
        case 2:
            return tr("Value");
            break;
        case 3:
            return tr("Unit");
            break;
        case 4:
            return tr("basis");
            break;
        }
    }
//    else
//        return QString::number(section) + 1;

    return QVariant();
}

bool DataModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(m_datas.count() - 1 < row )
        return false;

    beginRemoveRows(parent,row,row + count - 1);

    for(int i = row;i < row + count;i++)
        m_datas.removeAt(i);

    endRemoveRows();

    return true;
}

bool DataModel::removeRows(QModelIndexList indexList)
{
    if(indexList.isEmpty())
        return false;

    int r1 = indexList.first().row();
    int r2 = indexList.last().row();

//    cdebug << r1 << r2;

    beginRemoveRows(QModelIndex(),r1,r2);

    QList<DmmData *> datas;
    foreach (QModelIndex idx, indexList) {
        datas.append((DmmData *)m_datas.at(idx.row()));
    }

    foreach (DmmData *data, datas) {
        m_datas.removeOne(data);
        delete data;
    }

    endRemoveRows();

    return true;
}

Qt::ItemFlags DataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    if(!index.isValid())
        return flags;

    return flags;
}


int DataModel::addData(const DmmData *data)
{    
    beginInsertRows(QModelIndex(),rowCount(),rowCount());

    m_datas.append(data);

    endInsertRows();

    return m_datas.count();
}

void DataModel::editData(const QModelIndex &index, DmmData *to)
{
    int idx = index.row();
    if(idx >= 0){
        DmmData * old = (DmmData *)m_datas.at(idx);
        m_datas.replace(idx,to);
        delete old;

        emit dataChanged(this->index(idx,0),this->index(idx,m_datas.count()-1));
    }

}

void DataModel::insertData(const QModelIndex &index, DmmData *data)
{
    beginInsertRows(QModelIndex(),index.row(),index.row());
    m_datas.insert(index.row(),data);
    endInsertRows();
}

void DataModel::clear()
{
    if(rowCount() == 0)
        return ;

    beginRemoveRows(QModelIndex(),0,rowCount() - 1);

    qDeleteAll(m_datas)  ;
    m_datas.clear();

    endRemoveRows();

}

DmmData *DataModel::dataAt(int index)
{
    DmmData * rst = NULL;
    if(index >= 0 && index < m_datas.count()  )
        rst = (DmmData *)m_datas.at(index);

    return rst;
}

DmmData *DataModel::dataAt(const QModelIndex &index)
{
    if(!index.isValid() || index.row() >= m_datas.count())
        return NULL;

    return dataAt(index.row());
}

QList<const DmmData *> DataModel::datas() const
{
    return m_datas;
}



