#include "dmmchart.h"

#include <QPainter>
#include <QResizeEvent>
#include <MATH.H>
#include <QDebug>

#define X_INTERVAL_MIN 80//x轴每个大刻度的间隔(像素)
#define Y_INTERVAL_MIN 30//y轴每个大刻度的间隔(像素)

DMMChart::DMMChart(QWidget *parent) : QFrame(parent),
    m_updateScale(true),
    m_updateLine(true),
    m_updateIndicator(false),
    m_updateSelectArea(false),
    m_left(60),
    m_top(40),
    m_right(20),
    m_bottom(40),
    m_background(Qt::white),
    m_yAuto(true),
    m_yMin(0.0),
    m_yMax(0.0),
    m_xStep(0.0),
    m_yStep(0.0),
//    m_xMin(0.0),
//    m_xMax(0.0),
    m_xScaleInterval(0.0),
    m_yScaleInterval(0.0),
    m_xScaleNum(0),
    m_yScaleMaxNum(0),
    m_yScaleMinNum(0),
    m_xIsTime(true),
    m_xMul(0),
    m_yMul(0),
    m_IndicatorEnable(true),
    m_showIndicator(false),
    m_indicatorIdx(-1),
    m_tmrID(0),
    m_tmrIndicator(new QTimer),
    m_allowZoom(true),
    m_zooming(false),
    m_zoomed(false),
    m_zoomStartIdx(0),
    m_zoomEndIdx(0),
    m_zoomStartPt(QPoint(0,0)),
    m_zoomEndPt(QPoint(0,0)),
    m_indicatorClr(QColor(0,255,255))
{
//    this->setAutoFillBackground(true);
    setMouseTracking(true);//即使没有鼠标按钮按下，也会产生mouseMove事件
    m_lines.append(new ChartLine(this));//默认有一条数据曲线
    m_tmrIndicator->setSingleShot(true);//只触发一次
    connect(m_tmrIndicator,SIGNAL(timeout()),this,SLOT(slotTmrIndicatorTimeout()));
}

DMMChart::~DMMChart()
{
    qDeleteAll(m_lines);
    m_lines.clear();
}
/**
 * @brief DMMChart::addData
 * 添加数据
 * @param data
 * @param lineIndex
 */
void DMMChart::addData(ChartData *data,int lineIndex)
{
    m_lines.at(lineIndex)->addData(data);
    if(!m_zooming && !m_zoomed){//不处于放大过程和已放大状态，才绘制新数据
        m_updateScale = true;//需要重绘刻度
        update();
    }
}
/**
 * @brief DMMChart::clear
 * 清空数据
 */
void DMMChart::clear()
{
    m_mutex.lock();
    foreach (ChartLine * line, m_lines)
        line->clear();
    m_mutex.unlock();

    m_updateScale = true;//需要重绘刻度
    update();
}
/**
 * @brief DMMChart::saveToPiture
 * 保存为图片
 * @param fileName
 */
void DMMChart::saveToPiture(const QString &fileName) const
{
    m_pixmapIndicator.save(fileName);
}

void DMMChart::paintEvent(QPaintEvent *event)
{
    bool underupdate = false;

    QPainter painter(this)    ;

    m_mutex.lock();
    /*
    某层重绘，其上面的层都需要重绘
    */

    if(m_updateScale){//绘制scale
        m_updateScale = false;

        if(!m_zoomed){//不处于放大状态，重置m_zoomStartIdx和m_zoomEndIdx
            m_zoomStartIdx = 0;
            m_zoomEndIdx = m_lines.at(0)->m_showDatas.count() -1;
        }

        getPoints();  //获得绘制区域的point、m_yMin、m_yMax、m_xStep、m_yStep
        calDrawVar();   //m_xMul,m_yMul,m_xScaleInterval,m_yScaleInterval,m_xScaleMinNum,m_xScaleMaxNum,m_yScaleMinNum,m_yScaleMaxNum

        QPainter ptPixmapScale(&m_pixmapScale);

        ptPixmapScale.fillRect(0,0,width(),height(),m_background);//绘制背景

        ptPixmapScale.save();
        drawScale(&ptPixmapScale);//绘制scale
        ptPixmapScale.restore();

        underupdate = true;
    }

    if(m_updateLine || underupdate){
        m_updateLine = false;
        m_pixmapLine = m_pixmapScale;
        if(haveData()){
            QPainter ptPixmapLine(&m_pixmapLine);
            ptPixmapLine.save();
//            windowTransfer(&ptPixmapLine);
            for(int i=0;i<m_lines.count();i++)
                drawData(&ptPixmapLine,m_lines.at(i));
            ptPixmapLine.restore();
            underupdate = true;
        }
    }

    if(m_updateSelectArea || underupdate){
        m_updateSelectArea = false;
        m_pixmapSelArea = m_pixmapLine;
        QPainter ptPixmapSelArea(&m_pixmapSelArea);
        if(m_zooming){
            drawSelArea(&ptPixmapSelArea);
        }
        underupdate = true;
    }

    if(m_updateIndicator || underupdate){
        if(!m_tmrIndicator->isActive())
            m_updateIndicator = false;
        m_pixmapIndicator = m_pixmapSelArea;
        if(haveData()){
            QPainter ptPixmapIndicator(&m_pixmapIndicator);
            if(m_showIndicator){
                ptPixmapIndicator.save();
                drawIndicator(&ptPixmapIndicator);
                ptPixmapIndicator.restore();
                underupdate = true;
            }

        }
    }

    m_mutex.unlock();
    painter.drawPixmap(0,0,width(),height(),m_pixmapIndicator);


    QFrame::paintEvent(event);
}
/**
 * @brief DMMChart::resizeEvent
 * 窗口大小改变后，绘制区域和绘制pixmap同步改变大小
 * @param event
 */
void DMMChart::resizeEvent(QResizeEvent *event)
{
    m_drawRect = QRect(m_left,m_top,event->size().width()-m_left-m_right,
                       event->size().height()-m_top-m_right);
    m_pixmapScale = QPixmap(event->size().width(),event->size().height());

    m_updateScale = true;

    QFrame::resizeEvent(event);

}
/**
 * @brief DMMChart::mousePressEvent
 * 鼠标按下
 * @param event
 */
void DMMChart::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton &&  m_allowZoom && haveData()){//允许缩放并且有数据才能缩放
        m_indicatorIdx = -1;
        m_showIndicator = false;//鼠标按下，就进入放大区域选择过程，此时不需要绘制指示器
        update();

        if(!m_zooming){
            m_zooming = true;
            m_zoomStartPt = QPoint(event->pos().x() - m_drawRect.left(),
                                   event->pos().y() - m_drawRect.top());
            //开始点
            m_zoomStartPt.rx()
                    = qMin(qMax(m_zoomStartPt.x(),0),m_drawRect.width());//在m_drawRect范围内
            m_zoomStartPt.ry()
                    = qMin(qMax(m_zoomStartPt.y(),0),m_drawRect.height());//在m_drawRect范围内
        }
    }

    QFrame::mousePressEvent(event);
}

void DMMChart::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->pos().x() - m_drawRect.left();
    x = qMin(qMax(x,0),m_drawRect.width());//在m_drawRect范围内

    int idx ;

    /*处于缩放过程就绘制缩放框选线；一般状态时就重绘指示器*/

    if(m_zooming){//处于缩放过程
        m_zoomEndPt = QPoint(event->pos().x() - m_drawRect.left(),
                               event->pos().y() - m_drawRect.top());
        //结束点
        m_zoomEndPt.rx() =
                qMin(qMax(m_zoomEndPt.x(),0),m_drawRect.width());//在m_drawRect范围内
        m_zoomEndPt.ry() =
                qMin(qMax(m_zoomEndPt.y(),0),m_drawRect.height());//在m_drawRect范围内

        m_updateSelectArea = true;
        update();

    }
    else{//刷新指示器位置
        if(m_IndicatorEnable){
            idx = -1;
            idx = getIndexFromX(x);//获得光标处的point
            //光标要移动一定的位置才重绘指示器
            m_showIndicator = (idx != m_indicatorIdx) && (idx != -1);
            if(m_showIndicator){
                m_indicatorIdx = idx;//更新指示器index

                if(m_tmrIndicator->isActive())
                    m_tmrIndicator->stop();
                m_tmrIndicator->start(3000);//如果超过一定时间m_indicatorIdx没变化，就会隐藏指示器

                //重绘指示器
                m_updateIndicator = true;
                update();
            }
        }
    }

    QFrame::mouseMoveEvent(event);
}

void DMMChart::mouseReleaseEvent(QMouseEvent *event)
{
    int startIdx = -1;
    int endIdx = -1;

    if(m_zooming){//处于缩放过程
        if(m_zoomEndPt.x() > m_zoomStartPt.x() && m_zoomEndPt.y() > m_zoomStartPt.y()){//放大
            for(int i = 0;i < m_lines.at(0)->m_points.count();i++){
                if(startIdx == -1)
                    if(m_lines.at(0)->m_points.at(i)->x() >= m_zoomStartPt.x())
                        startIdx = i;
                if(endIdx == -1)
                    if(m_lines.at(0)->m_points.at(i)->x() >= m_zoomEndPt.x())
                        endIdx = qMax(i - 1,1);

                if(startIdx != -1 && endIdx != -1)
                    break;
            }


            startIdx = qMax(startIdx,0);
            if(endIdx == -1 || endIdx > m_lines.at(0)->m_points.count() -1)
                endIdx = m_lines.at(0)->m_points.count() - 1;

            if(endIdx - startIdx > 0){//放大区域至少要两点
                m_zoomEndIdx = m_zoomStartIdx + endIdx;      //更新放大时结束点的数据index
                m_zoomStartIdx = m_zoomStartIdx + startIdx;  //更新放大时开始点的数据index
                m_zoomed = true;
            }
        }
        else{//恢复

            m_zoomed = false;
        }

        m_zooming = false;      //缩放过程结束
        m_updateScale = true;   //需要重绘scale
        update();

    }

    QFrame::mouseReleaseEvent(event);
}
/**
 * @brief DMMChart::drawScale
 *      绘制边框和刻度
 * @param painter
 */
void DMMChart::drawScale(QPainter *painter)
{
    QPoint pt1,pt2;
    QVector<QLine> lines;
    QVector<QLine> dotLines;
    QLine l;
    QFontMetrics fm(painter->font());
    QString strText;
    QRect recText;
    int w ;
    int x;


    //画x轴线
    painter->drawLine(m_drawRect.left() -1,
                      m_drawRect.bottom() + 1,
                      m_drawRect.right() + 1,
                      m_drawRect.bottom() + 1);

    // 画x大刻度
    lines.clear();
    for (int i = 0; i < m_xScaleNum; ++i)
    {
        x = m_drawRect.left() + m_xScaleInterval * i + 0.5;
        l = QLine(x,m_drawRect.bottom(),x,m_drawRect.bottom() + 5);
        lines << l;
        dotLines << QLine(QPoint(l.p1().x(),m_drawRect.top()),QPoint(l.p1().x(),m_drawRect.bottom()));
    }
    painter->drawLines(lines);//画刻度
    painter->save();
    painter->setPen(Qt::DotLine);
    painter->drawLines(dotLines);//画dotline
    painter->restore();
    //x刻度值
    for (int i = 0; i < m_xScaleNum; ++i){
        if(m_xIsTime){//x轴代表时间
            if(haveData()){
                strText = m_lines.at(0)->showDatas().at(m_xMul * i + m_zoomStartIdx)->time()
                        .toString("hh:mm:ss");
                w = fm.width(strText);
                recText = QRect(lines.at(i).p2().x() - w/2,
                      lines.at(i).p2().y() + 2,
                      w,fm.height());
            }
        }else{//x轴代表序号
            strText = QString::number(i* m_xMul + m_zoomStartIdx);
            w = fm.width(strText);
            recText = QRect(lines.at(i).p2().x() - w/2,
                            lines.at(i).p2().y() + 2,
                            w,fm.height());
        }
        painter->drawText(recText,
                          Qt::AlignCenter,
                          strText);
    }

    //画y轴线
    painter->drawLine(m_left-1,m_top-1,m_left-1,m_drawRect.bottom() + 1);

    //计算y轴0刻度线的开始、结束point
    if(m_yMin >= 0 && m_yMax >= 0){//都是正值
        pt1 = QPoint(m_drawRect.left(),m_drawRect.bottom());
        pt2 = QPoint(m_drawRect.right(),m_drawRect.bottom());
    }
    else if(m_yMin <= 0 && m_yMax >= 0){//有正负值
        pt1 = QPoint(m_drawRect.left(),m_drawRect.top() + m_yStep *m_yMax );
        pt2 = QPoint(m_drawRect.right(),m_drawRect.top() + m_yStep *m_yMax);
    }
    else if(m_yMin <= 0 && m_yMax <= 0){//都是负值
        pt1 = QPoint(m_drawRect.left(),m_drawRect.top() );
        pt2 = QPoint(m_drawRect.right(),m_drawRect.top());
    }

    // 画y刻度
    //正数部分
    dotLines.clear();
    lines.clear();
    int y;
    for (int i = 0; i <= m_yScaleMaxNum ; ++i)
    {
        y = pt1.y() - m_yScaleInterval * i;
        l = QLine(m_drawRect.left(),y,m_drawRect.left()-5,y);
        lines << l;
        dotLines << QLine(l.p1(),QPoint(l.p1().x() + m_drawRect.width(),l.p1().y()));
    }
    painter->drawLines(lines);
    painter->save();
    painter->setPen(Qt::DotLine);
    painter->drawLines(dotLines);
    painter->restore();
    //y's scale value
    for (int i = 0; i <= m_yScaleMaxNum ; ++i)
    {
        double transVal = i* m_yMul;
        QString strU = QString();
        if(transVal >= 1000000){
            transVal /= 1000000;
            strU = "M";
        }else if(transVal >= 1000){
            transVal /= 1000;
            strU = "k";
        }

        strText = QString::number(transVal,'f',2) + strU;//保留了2位小数
        w = fm.width(strText);
        recText = QRect(lines.at(i).p2().x() - 2 - w,
                        lines.at(i).p2().y() - fm.height()/2,
                        w,
                        fm.height());
        painter->drawText(recText,
                          Qt::AlignCenter,
                          strText);
    }


    //负数部分
    lines.clear();
    dotLines.clear();
    for (int i = 0; i <= m_yScaleMinNum; ++i)
    {
        y = pt1.y() + m_yScaleInterval * i;
        l = QLine(m_drawRect.left(),y,m_drawRect.left()-5,y);
        lines << l;
        dotLines << QLine(l.p1(),QPoint(l.p1().x() + m_drawRect.width(),l.p1().y()));
    }
    painter->drawLines(lines);
    painter->save();
    painter->setPen(Qt::DotLine);
    painter->drawLines(dotLines);
    painter->restore();

    //y's scale value
    for (int i = 0; i <= m_yScaleMinNum ; ++i)
    {
        double transVal = i* m_yMul;
        QString strU = QString();
        if(transVal >= 1000000){
            transVal /= 1000000;
            strU = "M";
        }else if(transVal >= 1000){
            transVal /= 1000;
            strU = "k";
        }

        strText = QString::number(-transVal,'f',2) + strU;
        w = fm.width(strText);
        recText = QRect(lines.at(i).p2().x() - 2 - w,
                        lines.at(i).p2().y() - fm.height()/2,
                        w,
                        fm.height());
        painter->drawText(recText,
                          Qt::AlignCenter,
                          strText);
    }
}
/**
 * @brief DMMChart::drawData
 *      绘制line
 * @param painter
 * @param line
 */
void DMMChart::drawData(QPainter *painter, ChartLine *line)
{
//    QPoint pt1,pt2;
    QPoint pt11,pt22;
    int idx = 0;
    int ptscnt = m_zoomEndIdx - m_zoomStartIdx + 1;
    QPoint *pts = new QPoint[ptscnt];

    if(line->showDatas().count() == 0)
        return;

    if(m_yMin >= 0 && m_yMax >= 0){//都是正值
        pt11 = QPoint(m_drawRect.left(),m_drawRect.bottom());
        pt22 = QPoint(m_drawRect.right(),m_drawRect.bottom());
    }
    else if(m_yMin <= 0 && m_yMax >= 0){//正负值
        pt11 = QPoint(m_drawRect.left(),m_drawRect.top() + m_yStep *m_yMax );
        pt22 = QPoint(m_drawRect.right(),m_drawRect.top() + m_yStep *m_yMax);
    }
    else if(m_yMin <= 0 && m_yMax <= 0){//都是负值

        pt11 = QPoint(m_drawRect.left(),m_drawRect.top() );
        pt22 = QPoint(m_drawRect.right(),m_drawRect.top());
    }

//    pt1 = QPoint(0,m_yStep * line->showDatas().at(0 + m_zoomStartIdx)->value());
    painter->setPen(line->color());
    for(int i = m_zoomStartIdx;i <= m_zoomEndIdx;i++,idx++){
        if( i >= line->m_showDatas.count())
            break;
        pts[idx] = QPoint(m_drawRect.x() + m_xStep * idx,pt11.y() + -(m_yStep * line->showDatas().at(i)->value()));
//        pt2 = QPoint(m_xStep * idx,m_yStep * line->showDatas().at(i + m_zoomStartIdx)->value());
//        painter->drawLine(pt1,pt2);
//        pt1 = pt2;

    }
    painter->drawPolyline(pts,ptscnt);

    delete [] pts;

//    painter->setPen(line->color());
//    for(int i=1;i<line->m_points.count();i++){
//        painter->drawLine(*line->m_points.at(i-1),*line->m_points.at(i));
    //    }
}
/**
 * @brief DMMChart::drawSelArea
 *      绘制缩放过程中的选择框
 * @param painter
 */
void DMMChart::drawSelArea(QPainter *painter)
{
    QPoint pts[4];
    pts[0] = QPoint(m_drawRect.left() + m_zoomStartPt.x(),m_drawRect.top() + m_zoomStartPt.y());
    pts[1] = QPoint(m_drawRect.left() + m_zoomEndPt.x(),m_drawRect.top() + m_zoomStartPt.y());
    pts[2] = QPoint(m_drawRect.left() + m_zoomEndPt.x(),m_drawRect.top() + m_zoomEndPt.y());
    pts[3] = QPoint(m_drawRect.left() + m_zoomStartPt.x(),m_drawRect.top() + m_zoomEndPt.y());

//    QPen p = painter->pen();
//    p.setStyle(Qt::DotLine);
//    p.setColor(Qt::red);
//    painter->setPen(p);
    painter->setPen(Qt::DotLine);
//    painter->setBackground(Qt::red);

    painter->drawPolygon(pts,4);
}

/**
 * @brief DMMChart::drawIndicator
 *          绘制指示器
 * @param painter
 */
void DMMChart::drawIndicator(QPainter *painter)
{
    QRect recText;
    QFontMetrics fm(painter->font());
    QString strText;
    ChartLine * cl;
    int y=0;


    if(!haveData() || m_indicatorIdx == -1)
        return;

    if(m_indicatorIdx >=  m_lines.at(0)->m_points.count())
        m_indicatorIdx = m_lines.at(0)->m_points.count() - 1;

    //绘制刻度线
    painter->setPen(m_indicatorClr);
    int x = m_drawRect.left() + m_lines.at(0)->m_points.at(m_indicatorIdx)->x();
    painter->drawLine(x,m_drawRect.top(),x,m_drawRect.bottom());
    //绘制刻度值
    y = m_drawRect.top();
    for(int i=0;i<m_lines.count();i++,y++){
        cl = m_lines.at(i);
        ChartData * data = cl->m_showDatas.at(m_indicatorIdx + m_zoomStartIdx);
        strText = QString("%1: %2 %3\r\n")
                .arg(m_xIsTime?data->time().toString("hh:mm:ss"):QString::number( m_indicatorIdx + m_zoomStartIdx + 1))
                .arg(data->other().toString())
                .arg(data->valStr()/*.toStdString().data()*/);
        if(x + 2 + fm.width(strText) >= m_drawRect.right())
            x -= fm.width(strText) + 2;
        else
            x += 2;

        y -= fm.height();
        recText = QRect(x,
                        y /*+ cl->m_points.at(m_indicatorIdx)->y()*/,
                        fm.width(strText),
                        fm.height()
                        );
        painter->drawText(recText,Qt::AlignCenter,strText);

//        //显示不了β，所以用贴图的方式
//        if(cl->m_showDatas.at(m_indicatorIdx + m_zoomStartIdx)->other().toString() == "hFE"){
//            QPixmap pix(":/image/b2g.ico");
//            painter->drawPixmap(x + fm.width(strText) - fm.width("β"),
//                                m_drawRect.top(),
//                                pix);
//        }
    }
}

void DMMChart::getStep()
{
    //每个line都计算YStep
    foreach (ChartLine * line, m_lines)
        line->getStep();

    m_yMax = 0;
    m_yMin = 0;

    /*ystep*/
    //获取Y轴的上下限
    if(m_yAuto){
        foreach (ChartLine * line, m_lines){
            m_yMax = qMax(m_yMax,line->yMax());
            m_yMin = qMin(m_yMin,line->yMin());
        }
    }

    //调整
    if(m_yMin == m_yMax){
        if(m_yMax == 0){//m_yMin和m_yMax都为0
            m_yMax = 1;
            m_yMin = -1;
        }
        else{
            m_yMax = qMax(qAbs(m_yMin) + 1,qAbs(m_yMax) + 1);
            m_yMin = -m_yMax;
        }
    }
    m_yStep = m_drawRect.height()/(m_yMax - m_yMin);


    /*xstep*/
    m_xStep = 0.0;

    double xMin = m_zoomStartIdx;
    double xMax = m_zoomEndIdx;
    if(xMax - xMin >= 0)
        m_xStep = m_drawRect.width()/(xMax - xMin + 1);

    /*为line设置step*/
    foreach (ChartLine * line, m_lines){
        line->setXStep(m_xStep);
        line->setYStep(m_yStep);
    }


}

/**
 * @brief DMMChart::getXIntervalMul
 *          获得X轴最小刻度是step的几倍
 * @param step：x轴每点间隔的像素
 * @param intervalMin：x轴每个大刻度的间隔(像素)
 * @return
 */
int DMMChart::getXIntervalMul(double step, double intervalMin)
{
    QList<int> intervalMuls ;
    intervalMuls << 1 << 2 << 3 << 5 << 10 << 20 << 30 << 40 ;

    foreach (int m, intervalMuls) {
        if(step * m >= intervalMin)
            return m;
    }

    //如果intervalMuls里面的倍数不满足，则用50*n来获得倍数
    int m = 50;
    while(step * m < intervalMin)
        m += 50;
    return m;
}

//获得Y轴最小刻度是step的几倍
double DMMChart::getYIntervalMul(double step, double intervalMin)
{
    QList<double> intervalMuls ;
    intervalMuls << 0.00001<<0.0001<<0.001<<0.01<<0.1<<0.2<<0.3<<0.5<<0.8
                 <<1<<2<<3<<5<<10<<20<<30<<40 ;

    foreach (double m, intervalMuls) {
        if(step * m >= intervalMin)
            return m;
    }

    int m = 50;
    while(step * m < intervalMin)
        m += 50;
    return m;
}

bool DMMChart::haveData()
{
    foreach (ChartLine * line, m_lines) {
        if(line->haveData())
            return true;
    }
    return false;
}

int DMMChart::maxDataCnt()
{
    int rst = 0;

    foreach (ChartLine * line, m_lines)
        rst = qMax(rst,line->m_showDatas.count());

    return rst;
}

void DMMChart::windowTransfer(QPainter *painter)
{
    QRect rec;

    if(haveData()){
        /*计算位置，为setWindow准备 */
        if(m_yMin >= 0 && m_yMax >= 0){
            rec = QRect(-m_left,
                        m_drawRect.height() + m_top,
                        (m_drawRect.width()+m_right) - (-m_left),
                        -m_bottom -(m_drawRect.height() + m_top)
                        );

        }
        else if(m_yMin <= 0 && m_yMax >= 0){
            rec = QRect(-m_left,
                        m_yStep * m_yMax + m_top,
                        (m_drawRect.width() + m_right) - (-m_left),
                        (m_yStep * m_yMin - m_bottom) - (m_yStep * m_yMax + m_top));

        }
        else if(m_yMin <= 0 && m_yMax <= 0){
            rec = QRect(-m_left,
                        m_top,
                        (m_drawRect.width() + m_right) - (-m_left),
                        (-m_drawRect.height() - m_bottom) - m_top
                        );
        }
        painter->setWindow(rec);

    }
    else{
        painter->setWindow(-m_left,height()/2,
                           (m_drawRect.width()+m_right) - (-m_left),
                           -(height()/2)-(height()/2));
    }
}
/**
 * @brief DMMChart::calDrawVar
 *      计算绘图需要的一些变量
 * m_xMul,m_yMul,m_xScaleInterval,m_yScaleInterval,m_xScaleMinNum,
 * m_xScaleMaxNum,m_yScaleMinNum,m_yScaleMaxNum
 */
void DMMChart::calDrawVar()
{
    m_xScaleNum = 0;
    m_yScaleMaxNum = 0;
    m_yScaleMinNum = 0;

    if(haveData()){
        m_xMul = getXIntervalMul(m_xStep,X_INTERVAL_MIN);
        m_xScaleInterval = m_xStep * m_xMul;
        m_xScaleNum = m_drawRect.width()/m_xScaleInterval;

        m_yMul = getYIntervalMul(m_yStep,Y_INTERVAL_MIN);
        m_yScaleInterval = m_yStep * m_yMul;
        //        m_yScaleNum = m_drawRect.height()/m_yScaleInterval + 0.5;


        /*计算位置，为setWindow准备 */
        if(m_yMin >= 0 && m_yMax >= 0){//都是正值
            m_yScaleMaxNum = m_yStep * m_yMax / m_yScaleInterval ;
            m_yScaleMinNum = 0;

        }
        else if(m_yMin <= 0 && m_yMax >= 0){//有正负值
            m_yScaleMaxNum = m_yStep * m_yMax / m_yScaleInterval ;
            m_yScaleMinNum = m_yStep * qAbs(m_yMin) / m_yScaleInterval ;
        }
        else if(m_yMin <= 0 && m_yMax <= 0){//都是负值
            m_yScaleMaxNum = 0;
            m_yScaleMinNum = m_yStep * qAbs(m_yMin) / m_yScaleInterval ;
        }



    }
}

/**
 * @brief DMMChart::getIndexFromX
 *          根据x获得最合理的point的index
 * @param x: m_drawRect的内部坐标
 * @return -1:无效
 */
int DMMChart::getIndexFromX(int x)
{
    double v1,v2;

    if(m_lines.count() == 0)
        return -1;

    if(x < 0)
        return 0;
    else if(x > m_drawRect.width())
        return m_lines.at(0)->m_points.count() - 1;
    else{
        QPoint pt;
        for(int i=0;i<m_lines.at(0)->m_points.count();i++){
            pt = *m_lines.at(0)->m_points.at(i);
            if(pt.x() == x)
                return i;
            else{
                if(pt.x() > x){
                    v1 = pt.x();
                    v2 =  m_lines.at(0)->m_points.at(qMax(i-1,0))->x();
                    //处于两点之间时，返回更靠近的那个点
                    if(qAbs(v1 - x) < qAbs(v2 - x))
                        return i;
                    else
                        return qMax(0,i - 1);

                }
            }
        }
        return  m_lines.at(0)->m_points.count() - 1;
    }

    return -1;
}
bool DMMChart::allowZoom() const
{
    return m_allowZoom;
}

void DMMChart::setAllowZoom(bool allowZoom)
{
    m_allowZoom = allowZoom;
}

int DMMChart::zoomEndIdx() const
{
    return m_zoomEndIdx;
}

void DMMChart::setZoomEndIdx(int zoomEndIdx)
{
    m_zoomEndIdx = zoomEndIdx;
}

int DMMChart::zoomStartIdx() const
{
    return m_zoomStartIdx;
}

void DMMChart::setZoomStartIdx(int zoomStartIdx)
{
    m_zoomStartIdx = zoomStartIdx;
}


void DMMChart::slotTmrIndicatorTimeout()
{
    m_showIndicator = false;
    update();
}
bool DMMChart::xIsTime() const
{
    return m_xIsTime;
}

void DMMChart::setXIsTime(bool xIsTime)
{
    m_xIsTime = xIsTime;
}


QList<ChartLine *> DMMChart::lines() const
{
    return m_lines;
}

void DMMChart::setLines(const QList<ChartLine *> &lines)
{
    m_lines = lines;
}

double DMMChart::yMax() const
{
    return m_yMax;
}

void DMMChart::setYMax(int yMax)
{
    m_yMax = yMax;
}

double DMMChart::yMin() const
{
    return m_yMin;
}

void DMMChart::setYMin(int yMin)
{
    m_yMin = yMin;
}

/**
 * @brief DMMChart::getPoints
 *      获得所有line需绘制points
 */
void DMMChart::getPoints()
{
    getStep();
    foreach (ChartLine * line, m_lines) {
        line->getPoints();
    }
}
bool DMMChart::yAuto() const
{
    return m_yAuto;
}

void DMMChart::setYAuto(bool yAuto)
{
    m_yAuto = yAuto;
}

QColor DMMChart::background() const
{
    return m_background;
}

void DMMChart::setBackground(const QColor &background)
{
    m_background = background;
}



ChartData::ChartData(double value, const QString &valStr, const QDateTime &time, qint64 index,const QVariant &other)
{
    m_value = value;
    m_valStr = valStr;
    m_time = time;
    m_index = index;
    m_other = other;
}

qint64 ChartData::index() const
{
    return m_index;
}

void ChartData::setIndex(const qint64 &index)
{
    m_index = index;
}
QDateTime ChartData::time() const
{
    return m_time;
}

void ChartData::setTime(const QDateTime &time)
{
    m_time = time;
}
double ChartData::value() const
{
    return m_value;
}

void ChartData::setValue(double value)
{
    m_value = value;
}
QString ChartData::valStr() const
{
    return m_valStr;
}

void ChartData::setValStr(const QString &valStr)
{
    m_valStr = valStr;
}
QVariant ChartData::other() const
{
    return m_other;
}

void ChartData::setOther(const QVariant &other)
{
    m_other = other;
}






ChartLine::ChartLine(DMMChart *chart, QColor color): QObject(NULL),
    m_chart(chart),
    m_color(color),
    m_yMin(0),
    m_yMax(0),
    m_xStep(0.0),
    m_yStep(0.0)
{

}

ChartLine::~ChartLine()
{
    clear();
}

void ChartLine::clear()
{
    qDeleteAll(m_showDatas);
    m_showDatas.clear();
}

void ChartLine::addData(ChartData *data)
{
    m_showDatas.append(data);

}
QColor ChartLine::color() const
{
    return m_color;
}

void ChartLine::setColor(const QColor &color)
{
    m_color = color;
}

QList<ChartData *> ChartLine::showDatas() const
{
    return m_showDatas;
}

void ChartLine::setShowDatas(const QList<ChartData *> &datas)
{
    m_showDatas = datas;
}

/**
 * @brief ChartLine::getPoints
 *      获得需绘制points
 */
void ChartLine::getPoints()
{
    qDeleteAll(m_points);
    m_points.clear();

    int idx = 0;
    for(int i=m_chart->zoomStartIdx();i<=m_chart->zoomEndIdx();i++){
        QPoint * point = new QPoint(m_xStep * idx++ + 0.5,m_yStep * m_showDatas.at(i)->value() + 0.5);
        m_points.append(point);
    }
}

/**
 * @brief ChartLine::getStep
 * 这里其实没有计算出step,只得到了Y轴的上下限数据值，
 * 具体的step需要综合各个line的Y轴上下限来获得
 */
void ChartLine::getStep()
{

    if(m_chart->yAuto()){
        m_yMin = 0;
        m_yMax = 0;

        for(int i=m_chart->zoomStartIdx();i<=m_chart->zoomEndIdx();i++){
            m_yMax = qMax(m_yMax,m_showDatas.at(i)->value());
            m_yMin = qMin(m_yMin,m_showDatas.at(i)->value());
        }
    }
}

bool ChartLine::haveData()
{
    return m_showDatas.count() > 0;
}
double ChartLine::yStep() const
{
    return m_yStep;
}

void ChartLine::setYStep(double yStep)
{
    m_yStep = yStep;
}

double ChartLine::xStep() const
{
    return m_xStep;
}

void ChartLine::setXStep(double xStep)
{
    m_xStep = xStep;
}

double ChartLine::yMax() const
{
    return m_yMax;
}

void ChartLine::setYMax(int yMax)
{
    m_yMax = yMax;
}

double ChartLine::yMin() const
{
    return m_yMin;
}

void ChartLine::setYMin(int yMin)
{
    m_yMin = yMin;
}

