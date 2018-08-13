#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QTransform>
#include <QMargins>


class DataLimit;
class LimitData;
class Bar;

class Histogram : public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = 0);
    void append(double data);
    void append(QList<double> datas);
    void clear();
    QList<DataLimit *> limits() const;
    void setLimits(const QList<DataLimit *> &limits);
    QMargins margins() const;
    void setMargins(const QMargins &margins);

protected:
    void paintEvent(QPaintEvent *event);
    virtual void  mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    QTransform tf;
    QMargins m_margins;                 //空白区域
    QRectF m_validRec;
    QList<DataLimit *> m_limits;       //分组条件
    QList <double> m_sourceDatas;      //原始数据
    QList<LimitData *> m_limitDatas;  //分组后的数据
    QList<Bar *> m_bars;
    bool m_topTextVisible;
    bool m_leftTextVisible;
    bool m_bottomTextVisible;
    QString topText;
    QString leftText;
    QString bottomText;
    double m_barHeight;
    bool m_updateScale;
    bool m_updateData;
    bool m_updateIn;
    QPixmap m_pixmapScale;
    QPixmap m_pixmapData;
    QPixmap m_pixmapIn;


    void refreshLimitDatas();
    void drawScale(QPainter &painter);
    void drawData(QPainter *painter);
signals:

public slots:
};

class DataLimit
{
public:
    DataLimit(double low,double high){this->m_low = low;this->m_high = high;}
    double low() const
    {
        return m_low;
    }
    void setLow(double low){
        m_low = low;
    }

    double high() const{
        return m_high;
    }
    void setHigh(double high){
        m_high = high;
    }

private:
    double m_low;
    double m_high;
};

class LimitData
{
public:
    LimitData(DataLimit * limit,const QList<double> &sourceData){
        m_sourceData = sourceData;
        m_limit = limit;
        refreshLimitDatas();
    }

    ~LimitData(){
        m_sourceData.clear();
        m_datas.clear();
    }


    void refreshLimitDatas(){
        if(!m_limit || m_sourceData.isEmpty())
            return;

        m_datas.clear();
        foreach (double d, m_sourceData) {
            if(d >= m_limit->low() && d <= m_limit->high())
                m_datas.append(d);
        }
    }

    QList<double> datas() const{return m_datas;}
//    void setDatas(const QList<double> &datas){
//        m_datas = datas;
//    }

    DataLimit *limit() const{return m_limit;}
    void setLimit(DataLimit *limit){
        m_limit = limit;

    }

    QList<double> sourceData() const
    {
        return m_sourceData;
    }

    void setSourceData(const QList<double> &sourceData)
    {
        m_sourceData = sourceData;
    }



private:
    QList<double> m_sourceData;
    DataLimit * m_limit;//范围
    QList<double> m_datas;//处于范围的值
};

class Bar
{
public:
    Bar(){
        m_color = Qt::blue;
    }

    void drawData(QPainter *painter);

    QRectF validRect() const
    {
        return m_validRect;
    }

    void setValidRect(const QRectF &validRect)
    {
        m_validRect = validRect;
    }

    LimitData * limitData() const
    {
        return m_limitData;
    }

    void setLimitData(LimitData *limitData)
    {
        m_limitData = limitData;
    }

    QColor color() const
    {
        return m_color;
    }

    void setColor(const QColor &color)
    {
        m_color = color;       
    }


private:
    QRectF m_validRect;
    LimitData * m_limitData;
    QColor m_color;
};

#endif // HISTOGRAM_H
