#include "histogram.h"

#include <QPainter>
#include <QPaintEvent>

#include "utils.h"
#include <QFontMetrics>
#include <QPixmapCache>


Histogram::Histogram(QWidget *parent) : QWidget(parent),
    m_margins(20,20,20,20),
    m_updateScale(true)
{
    setMouseTracking(true);
}

void Histogram::append(double data)
{
    m_sourceDatas.append(data);
    refreshLimitDatas();
    m_updateData = true;
    update();
}

void Histogram::append(QList<double> datas)
{    
    m_sourceDatas.append(datas);
    refreshLimitDatas();
    m_updateData = true;
    update();
}

void Histogram::clear()
{
    m_sourceDatas.clear();
}

void Histogram::paintEvent(QPaintEvent *event)
{
    bool updateNext = false;
//    QPixmap pixmap(this->size());
    //绘制区域
    QPainter p(this);

//    QPixmap pixmap(this->width(),this->height());
//    QPainter painter(&pixmap);
//    painter.fillRect(0,0,this->width(),this->height(),Qt::white);//绘制背景
//    //转换坐标
//    painter.setWindow(-m_margins.left(),
//                      - (this->height()-m_margins.bottom()),
//                      this->width(),
//                      this->height());


//    //保存转换
//    tf = painter.combinedTransform().inverted();

    if(m_updateScale){
        m_validRec = QRectF(0,0,
                                this->width() - (m_margins.left() + m_margins.right()),
                                this->height() - (m_margins.top() + m_margins.bottom()));

        m_pixmapScale = QPixmap(this->width(),this->height());
        QPainter p1(&m_pixmapScale);
        drawScale(p1);
        m_updateScale = !m_updateScale;
        updateNext = true;
    }

    if(m_updateData || updateNext){
        m_pixmapData = m_pixmapScale;
        QPainter p2(&m_pixmapData);
        drawData(&p2);

        m_updateData = !m_updateData;
        updateNext = true;
    }

    if(m_updateIn || updateNext){
        m_pixmapIn = m_pixmapData;
        QPainter p3(&m_pixmapIn);

        m_updateIn = !m_updateIn;
        updateNext = false;
    }


    p.drawPixmap(0,0,m_pixmapIn);

//    if(!QPixmapCache::find("data",&pixmapTemp)){
//cdebug << "333";
//        pixmapTemp = QPixmap(this->width(),this->height());
//        QPainter p1(&pixmapTemp);
//        drawData(&p1);
//        QPixmapCache::insert("data",pixmapTemp);
//    }
//    p.drawPixmap(0,0,pixmapTemp);





//    QPen pen = painter.pen();
//    pen.setColor(Qt::transparent);
//    painter.setPen(pen);
//    painter.setBrush(Qt::darkMagenta);
//    painter.drawRect(QRectF(10,0,100,100));

}
/**
 * @brief Histogram::drawScale 绘制
 * @param painter
 */
void Histogram::drawScale(QPainter &painter)
{
    painter.save();

    painter.fillRect(0,0,this->width(),this->height(),Qt::white);//绘制背景
    //转换坐标
    painter.setWindow(-m_margins.left(),
                      - (this->height()-m_margins.bottom()),
                      this->width(),
                      this->height());

    QPen pen = painter.pen();
    pen.setColor(Qt::lightGray);
    painter.setPen(pen);
    painter.drawLine(QPointF(-1,1),QPointF(m_validRec.width(),1));
    painter.drawLine(QPointF(-1,1),QPointF(-1,-m_validRec.height()));
    painter.restore();
}
/**
 * @brief Histogram::drawData 绘制数据
 * @param painter
 */
void Histogram::drawData(QPainter *painter)
{
    int maxNum = 0;//数据的最大个数
    double numStep = 0;//一个数据占用的像素
    int yMaxNum = 0;    //Y轴， 也就是数据组数
    double yNumStep = 0;//一个数据组占用的高度
    painter->save();

//    painter->fillRect(0,0,this->width(),this->height(),Qt::white);//绘制背景
    //转换坐标
    painter->setWindow(-m_margins.left(),
                      - (this->height()-m_margins.bottom()),
                      this->width(),
                      this->height());

    foreach (LimitData *ld, m_limitDatas) {
        if(maxNum < ld->datas().count())
            maxNum = ld->datas().count();
    }
    numStep = maxNum == 0?1:m_validRec.width() / maxNum;

    yMaxNum = m_limits.count();
    yNumStep = yMaxNum == 0?1:m_validRec.height() / yMaxNum;

    int i = 0;
    foreach (Bar * bar, m_bars) {
        QRectF rect = QRectF(0,
                             -yNumStep * (i + 1) + yNumStep/5*1,//空白处各占用1/5
                             numStep * bar->limitData()->datas().count(),
                             yNumStep / 5 * 3//数据条占3/5
                             );
        bar->setValidRect(rect);
        i++;
    }

    foreach (Bar * bar, m_bars) {
        bar->drawData(painter);
    }
    painter->restore();
}


void Histogram::mouseMoveEvent(QMouseEvent *event)
{
//    QPointF pt =  tf.map(event->pos());
//    cdebug << event->pos() << pt ;
}

void Histogram::resizeEvent(QResizeEvent *event)
{
    m_updateScale = true;
}

QMargins Histogram::margins() const
{
    return m_margins;
}

void Histogram::setMargins(const QMargins &margins)
{
    m_margins = margins;
}
/**
 * @brief Histogram::takeGroup 根据分组条件m_limits，把数据分组保存在m_groupDatas
 */
void Histogram::refreshLimitDatas()
{
    if(m_limits.isEmpty() || m_sourceDatas.isEmpty())
        return;

    foreach (LimitData * limitData, m_limitDatas) {
        limitData->setSourceData(m_sourceDatas);
        limitData->refreshLimitDatas();
    }
}


QList<DataLimit *> Histogram::limits() const
{
    return m_limits;
}

void Histogram::setLimits(const QList<DataLimit *> &limits)
{
    m_limits = limits;

    //重新设置范围条件
    foreach (LimitData *limitData, m_limitDatas) {
        delete limitData;
    }
    m_limitDatas.clear();

    foreach (Bar *bar, m_bars) {
        delete bar;
    }
    m_bars.clear();

    foreach (DataLimit *d, m_limits) {
        LimitData *limitData = new LimitData(d,m_sourceDatas);
        m_limitDatas << limitData;
        Bar *bar = new Bar;
        bar->setLimitData(limitData);
        m_bars << bar;
    }
}

void Bar::drawData(QPainter *painter)
{
    painter->save();
    m_color = QColor::fromRgba(qRgba(250,150,0,128));
    painter->setPen(Qt::transparent);
    painter->setBrush(m_color);
    painter->drawRect(m_validRect);

    m_color = QColor::fromRgba(qRgba(0,100,250,128));
    painter->setPen(m_color);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(20);
    painter->setFont(font);

    QFontMetrics fm(painter->font());
    QString str = QString("%1").arg(m_limitData->datas().count());
    painter->drawText(m_validRect,Qt::AlignCenter ,str);
    painter->restore();
}


