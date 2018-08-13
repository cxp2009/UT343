#include "tableprinter.h"

#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrinterInfo>
#include <QDebug>
#include <QPainter>
#include <QtPrintSupport/QPageSetupDialog>
#include <QtPrintSupport/QPrintPreviewWidget>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QTextEdit>
#include <QAbstractTableModel>
#include <QDateTime>

TablePrinter::TablePrinter(DMMModel *models, QObject *parent) : QObject(parent)
{
    m_model = models;


}

TablePrinter::~TablePrinter()
{

}

void TablePrinter::print()
{

    QPrinter *printer = new QPrinter;
//    QPrintPreviewWidget previewWidget(printer);

    QPrintPreviewDialog previewDialog(printer);
    previewDialog.setWindowFlags(previewDialog.windowFlags()
                                 | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
    previewDialog.setWindowState(Qt::WindowMaximized);
    connect(&previewDialog, SIGNAL(paintRequested(QPrinter *)),this,SLOT(slotPaintRequested(QPrinter *)));

//    if(previewDialog.exec() == QDialog::Accepted){
    previewDialog.exec();
//    }
}
QString TablePrinter::title() const
{
    return m_title;
}

void TablePrinter::setTitle(const QString &title)
{
    m_title = title;
}
QList<Column *> TablePrinter::cols() const
{
    return m_cols;
}

void TablePrinter::setCols(const QList<Column *> &cols)
{
    m_cols = cols;
}


#define FONT_SIZE 12    //字体大小
void TablePrinter::slotPaintRequested(QPrinter *printer)
{
    int heightPerRow=0;                     //行高
    int rowsPerPage=0;                      //每页行数
    int x=0;
    int y=0;
    int pageCnt=0;                          //总页数
    int recW = printer->pageRect().width(); //可绘制区域宽度
    int recH = printer->pageRect().height();//可绘制区域高度
    int mar = recW/20;
    int w;
    QString str;
    int rowCnt = 0;
    int mr = 0;
    QAbstractTableModel * model = m_model;
    rowCnt = model->rowCount();

    int rows = rowCnt;         //总行数
    int cols = m_model->columnCount();      //列数
    double colfactor = 0.0;
    QVector<QLine > lines;

    QPainter painter;
    painter.begin(printer);

    QFont font = painter.font();
    font.setPointSize(FONT_SIZE);
    painter.setFont(font);//设置字体
    QFontMetrics fm(painter.font());
    heightPerRow = fm.height();
    heightPerRow += heightPerRow /2;        //1.5 font‘s height
    int titleHeight = heightPerRow * 3;
    int footerHeight = heightPerRow * 1;
    int headerHeight = heightPerRow;
    rowsPerPage = (recH - titleHeight - headerHeight - footerHeight)/heightPerRow;
    pageCnt = rows/rowsPerPage;
    pageCnt += (rows - rowsPerPage * pageCnt) > 0?1:0;

    for(int page = 0;page < pageCnt;page++){
        //title
        w = fm.width(m_title);
        x = (recW - w)/2;
        y = 0;
        painter.drawText(QRect(x,y,w,titleHeight),Qt::AlignCenter,m_title);
        /*header*/
        font = painter.font();
        font.setPointSize(10);
        painter.setFont(font);
        QFontMetrics fms(painter.font());
        //time
        str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        w = fms.width(str);
        x = recW - mar - w;
        y = 0;
        painter.drawText(QRect(x,y,w,titleHeight),Qt::AlignBottom,str);
        //page num
        str = QString("%1/%2").arg(page+1).arg(pageCnt);
        w = fms.width(str);
        x = recW - mar - w;
        y = titleHeight + headerHeight + heightPerRow * rowsPerPage;
        painter.drawText(QRect(x,y,w,footerHeight),Qt::AlignRight | Qt::AlignBottom,str);

        font = painter.font();
        font.setPointSize(FONT_SIZE);
        painter.setFont(font);

        painter.drawLine(mar,titleHeight,recW - mar,titleHeight );//最上面横线
        painter.drawLine(mar,titleHeight + headerHeight,
                         recW - mar,titleHeight + headerHeight );//header下面横线

        /*column*/
        colfactor = 0.0;
        for(int col = 0;col < cols;col++)
            colfactor += m_cols.at(col)->m_factor;
        colfactor = (recW - mar*2)/colfactor;
        //header
        x = mar;
        lines.clear();
        int r = qMin(rowsPerPage,rows - rowsPerPage * page);
        for(int col = 0;col < cols;col++){
            w = m_cols.at(col)->m_factor * colfactor;
            y = titleHeight;
            lines.append(QLine(x,y,x,y + headerHeight + heightPerRow * r));
            painter.drawText(QRect(x,y,w,heightPerRow ),Qt::AlignCenter,m_cols.at(col)->m_title);
            x += w;

        }
        lines << QLine(recW - mar,titleHeight,recW - mar,//最右边的竖线
                       titleHeight + headerHeight + heightPerRow * r);
        painter.drawLines(lines);

        //data
        lines.clear();
        for(int row=0;row < rowsPerPage;row++){
            if(!(rowsPerPage * page + row < rows))
                break;

            x = mar;

            for(int col = 0;col < cols;col++){
               y = titleHeight + headerHeight + heightPerRow * row;
               w = m_cols.at(col)->m_factor * colfactor;

               QVariant v = model->data(model->index(mr,col));

               if(v.type() > QVariant::Bool && v.type() < QVariant::Map)
                   str = QString::number( v.toInt());
               else if(v.type() == QVariant::String)
                   str = v.toString();
               else if(v.type() == QVariant::DateTime){
                   str = v.toDateTime().toString("yyyy-MM-dd hh:mm:ss");
               }
               else{
                   str = v.toString();
               }
               painter.drawText(QRect(x,y,w,heightPerRow),
                                Qt::AlignBottom | Qt::AlignHCenter,str);

               lines.append( QLine(x,y + heightPerRow,recW - mar,y + heightPerRow));
               x += w;
            }
            mr++;
        }
        painter.drawLines(lines);

        if(page < pageCnt - 1)
            printer->newPage();
    }
    painter.end();
}

