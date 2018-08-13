#ifndef DMMCHART_H
#define DMMCHART_H

#include <QDateTime>
#include <QFrame>
#include <QPixmap>
#include <QTimer>
#include <QVariant>
#include <qmutex.h>

/**
 * @brief The ChartData class
 * 一个点的数据，包括index，time，value和other
 */
class ChartData{
public:
    ChartData(double value = 0.0,const QString &valStr = QString(),
              const QDateTime &time = QDateTime(),qint64 index = -1,
              const QVariant &other = QVariant());
    qint64 index() const;
    void setIndex(const qint64 &index);

    QDateTime time() const;
    void setTime(const QDateTime &time);

    double value() const;
    void setValue(double value);

    QString valStr() const;
    void setValStr(const QString &valStr);

    QVariant other() const;
    void setOther(const QVariant &other);

private:
    qint64 m_index;
    QDateTime m_time;
    double m_value;
    QString m_valStr;
    QVariant m_other;
//    bool m_isTimeData;
};

class DMMChart;
/**
 * @brief The ChartLine class
 * 一条数据曲线
 */
class ChartLine:public QObject
{
    Q_OBJECT
public:
    ChartLine(DMMChart * chart, QColor color = Qt::red);
    ~ChartLine();

    void clear();


    double yMin() const;
    void setYMin(int yMin);

    double yMax() const;
    void setYMax(int yMax);

    double xStep() const;
    void setXStep(double xStep);

    double yStep() const;
    void setYStep(double yStep);

    QList<ChartData *> showDatas() const;
    void setShowDatas(const QList<ChartData *> &datas);

    QColor color() const;
    void setColor(const QColor &color);

protected:
    void getPoints();
    void getStep();
    bool haveData();
    void addData(ChartData * data);
private:
    QList<ChartData *> m_showDatas;     //数据列表
    QList<QPoint *> m_points;           //只代表当前需要绘制的点
    QColor m_color;
    double m_yMin;  //代表了最小测量值
    double m_yMax;  //代表了最大测量值
    double m_xStep; //同chart
    double m_yStep; //同chart
    DMMChart * m_chart;

    friend class DMMChart;
};
/**
 * @brief The DMMChart class
 * 图表类
 */
class DMMChart : public QFrame
{
    Q_OBJECT
public:
    explicit DMMChart(QWidget *parent = 0);
    ~DMMChart();

    void addData(ChartData * data, int lineIndex = 0);
    void clear();
    void saveToPiture(const QString &fileName) const;


    QColor background() const;
    void setBackground(const QColor &background);

    bool yAuto() const;
    void setYAuto(bool yAuto);

    double yMin() const;
    void setYMin(int yMin);

    double yMax() const;
    void setYMax(int yMax);

    QList<ChartLine *> lines() const;
    void setLines(const QList<ChartLine *> &lines);

    bool xIsTime() const;
    void setXIsTime(bool xIsTime);

    int zoomStartIdx() const;
    void setZoomStartIdx(int zoomStartIdx);

    int zoomEndIdx() const;
    void setZoomEndIdx(int zoomEndIdx);

    bool allowZoom() const;
    void setAllowZoom(bool allowZoom);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

    void drawScale(QPainter *painter);
    void drawData(QPainter *painter,ChartLine * line);
    void drawSelArea(QPainter *painter);
    void drawIndicator(QPainter *painter);

    void getPoints();
    void getStep();

    int getXIntervalMul(double step,double intervalMin);
    double getYIntervalMul(double step,double intervalMin);
    bool haveData();
    int maxDataCnt();
    void windowTransfer(QPainter * painter);
    void calDrawVar();
    int getIndexFromX(int x);


private:
    int m_left;
    int m_right;
    int m_top;
    int m_bottom;
    QRect m_drawRect;//绘制区域
    QList<ChartLine *> m_lines;//曲线列表(一个chart可以有多条数据曲线)
    QPixmap m_pixmapScale;
    QPixmap m_pixmapLine;
    QPixmap m_pixmapIndicator;
    QPixmap m_pixmapSelArea;
    QTimer *m_tmrIndicator;         //用于隐藏指示器
    bool m_updateScale;             //是否重绘刻度
    bool m_updateLine;              //是否重绘代表值得曲线
    bool m_updateIndicator;         //是否重绘指示器
    bool m_updateSelectArea;        //是否重绘缩放框选线
    QColor m_background;
    bool m_yAuto;                   //y轴的值是否根据value来自动调整
    double m_yMin;                  //y轴的最小测量值
    double m_yMax;                  //y轴的最大测量值
//    double m_xMin;
//    double m_xMax;
    double m_xStep;                 //x轴上一个点单位需要的像素量 宽度像素/1point
    double m_yStep;                 //y轴上一个值单位需要的像素量 高度像素/1value
    double m_xScaleInterval;        //每个大刻度的间隔(pixel)
    double m_yScaleInterval;        //每个大刻度的间隔(pixel)
    int m_xScaleNum;                //x轴需要几个大刻度
    int m_yScaleMaxNum;             //y轴正值需要几个大刻度
    int m_yScaleMinNum;             //y轴负值需要几个大刻度
    bool m_xIsTime;
    int m_xMul;                     //x轴多少个点代表每个大刻度
    double m_yMul;                  //y轴多少个点代表每个大刻度
    bool m_IndicatorEnable;         //是否显示指示器
    bool m_showIndicator;           //是否重绘指示器
    int m_indicatorIdx;             //指示器显示的数据的index
    int m_tmrID;
    bool m_allowZoom;
    bool m_zooming;                 //处于放大过程(选取放大区域时)
    bool m_zoomed;                  //处于已放大状态
    int m_zoomStartIdx;             //放大时的开始点的数据index
    int m_zoomEndIdx;               //放大时的开始点的数据index
    QPoint m_zoomStartPt;           //放大框选区域的开始点
    QPoint m_zoomEndPt;             //放大框选区域的结束点
    QMutex m_mutex;
    QColor m_indicatorClr;          //指示器的颜色
signals:

public slots:
    void slotTmrIndicatorTimeout();
};

#endif // DMMCHART_H
