#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QImage>
#include <QPoint>
#include <QRect>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QDebug>
#include <QVector>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QLabel>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //打开图像
    void OpenImage();
    //保存图像
    void SaveImage();

    //画线
    void DrawLine();
    //新建
    void AddNew();
    //橡皮
    void DrawEraser();
    //矩形选区
    void DrawRect();

    //模糊函数
    QImage Blur(QImage origin);
    //模糊槽函数
    void DrawBlur();

    //锐化函数
    QImage  Sharpen(QImage  origin);
    //锐化槽函数
    void DrawSharpen();


    //选区
    void Select();
    void DeleteSelect();
    QPoint GetBeginPoint(QPoint begin);
    QPoint GetEndPoint(QPoint end);
    QPoint m_begin;
    QPoint m_end;
    bool m_isselect;
    QPixmap m_selectpix;
    QImage m_selectimg;
    bool m_selectpress;
    bool m_isoverselect;
    QImage Copy(QImage img);
    QVector<QRect> m_selectrect;


    //明天写movecopy函数，进行m_pix的更改 完成移动功能
    //移动
    void Move();
    bool m_ismove;
    QImage MoveCopy();
    QPoint m_beginmove;
    QPoint m_endmove;
    bool m_ismoveover;


    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *);



    QVector<QVector <QPoint> > m_penline; //集条集合，多段线条
    QVector<QVector <QColor> > m_pencolor;//颜色集合，跟线条配合使用
    QVector<QVector <int> > m_penwidth;//笔刷大小集合，跟线条和颜色配合使用

    QVector<QVector <int> >m_eraserwidth;//橡皮笔刷
    QVector<QVector <QPoint> > m_eraserline;//橡皮线条

    QVector<int>  m_shape;//种类

    QVector<QRect> m_rect;//矩形集合
    QVector<QVector <QColor> > m_rectcolor;//矩形颜色集合
    QVector<QVector <int> > m_rectwidth;//矩形笔刷大小集合



private:
    Ui::MainWindow *ui;

    int m_drawType;//描绘类型
    QPixmap m_pix;//画布图片
    bool m_leftpress;//左键按下标志
    bool m_isopen;//打开图片



};
#endif // MAINWINDOW_H
