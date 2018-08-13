#include "dmmmodel.h"
#include <QModelIndex>
#include <QDebug>

int static const COL_CNT = 6;

DMMModel::DMMModel(UT8805 *dmm, QObject *parent) : QAbstractTableModel(parent),
    m_dmm(dmm)
{
}

DMMModel::~DMMModel()
{
    qDeleteAll(m_datas);
    m_datas.clear();
}

int DMMModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_datas.count();
}

int DMMModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return COL_CNT;
}

QVariant DMMModel::data(const QModelIndex &index, int role) const
{
    QString str;
    const DmmData * data;

    if(!index.isValid())
        return QVariant();
    if(index.row() >= rowCount())
        return QVariant();
    if(index.column() >= columnCount())
        return QVariant();

    data = m_datas.at(index.row());

    if(role == Qt::DisplayRole){
        switch(index.column()){
        case 0:
            return index.row() + 1;
            break;
        case 1:
            return data->time().toString("hh:mm:ss");
            break;
        case 2:
//            if(data->funInfo()->PriFun == "VOLT" || data->funInfo()->PriFun == "CURR"){
//                str = data->funInfo()->SecFun + data->funInfo()->UnitStr;
//            }else if(data->funInfo()->PriFun == "TEMP"){
//                str = data->funInfo()->PriFun + " " + data->funInfo()->SecFun;
//            }else if(data->funInfo()->PriFun == "TC"){
//                str = QString("%1 %2").arg("TEMP").arg("TC");
//            }
//            else{
//                str = data->funInfo()->PriFun;
//            }
            str = data->funInfo()->FunStr;
            return str;
            break;
        case 3:
            if(data->funInfo()->PriFun == "TEMP" || data->funInfo()->PriFun == "TC"
                    || data->funInfo()->PriFun == "CONT" || data->funInfo()->PriFun == "DIOD"){
                return QVariant();
            }
            else if(data->funInfo()->PriFun == "FREQ" || data->funInfo()->PriFun == "PER"){
                return QString("%1%2V").arg( data->funInfo()->RangeU ).arg(data->funInfo()->SecUnit);
            }
            else{
                return data->funInfo()->RangeU ;
            }
            break;
        case 4:
            str = data->valStr();
            return str;
            break;
        case 5:
            if(data->funInfo()->PriFun == "FREQ"){
                str = data->funInfo()->UnitStr ;
            } else if(data->funInfo()->PriFun == "PER"){
                str = data->unit2() ;
            }
            else if(data->funInfo()->PriFun == "TEMP" || data->funInfo()->PriFun == "TC"){
                if(m_dmm->getTempUnit() == "C")
                    str =  QString::fromLocal8Bit("℃");
                else if(m_dmm->getTempUnit() == "F")
                    str = QString::fromLocal8Bit("℉");
                else
                    str = "K";
            }
            if(data->funInfo()->PriFun == "RES" || data->funInfo()->PriFun == "FRES"){
                str = data->funInfo()->SecUnit + QString::fromLocal8Bit("Ω");
            }
            else
            {
                str = data->funInfo()->SecUnit +  QString::fromLocal8Bit("");
            }
            return str;
            break;

        }
    }else if(role == Qt::TextAlignmentRole){
        return Qt::AlignCenter;
    }

    return QVariant();
}

QVariant DMMModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal){
        switch(section){
        case 0:
            return "NO.";
            break;
        case 1:
            return "Time";
            break;
        case 2:
            return "Function";
            break;
        case 3:
            return "Range";;
            break;
        case 4:
            return "Value";
            break;
        case 5:
            return "Unit";
            break;

        }
    }
//    else
//        return QString::number(section) + 1;

    return QVariant();
}

int DMMModel::addData(const DmmData *data)
{    
        beginInsertRows(QModelIndex(),rowCount(),rowCount());

        m_datas.append(data);

        endInsertRows();

//    emit rowsInserted(QModelIndex(),rowCount()-1,rowCount()-1);

    return m_datas.count();
}

void DMMModel::clear()
{

    if(rowCount() == 0)
        return ;

    beginRemoveRows(QModelIndex(),0,rowCount() - 1);

    qDeleteAll(m_datas)  ;
    m_datas.clear();

    endRemoveRows();

}



