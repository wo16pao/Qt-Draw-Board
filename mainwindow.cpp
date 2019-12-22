#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始化
    m_leftpress = false;
    m_isopen = false;
    m_drawType = 0;
    m_isselect = false;
    m_selectpress = false;
    m_isoverselect = false;
    m_ismove = false;

    //画线
    connect(ui->actionPen,&QAction::triggered,this,&MainWindow::DrawLine);
    //橡皮
    connect(ui->actionEraser,&QAction::triggered,this,&MainWindow::DrawEraser);
    //矩形
    connect(ui->actionRect,&QAction::triggered,this,&MainWindow::DrawRect);
    //打开文件
    connect(ui->actionOpen,&QAction::triggered,this,&MainWindow::OpenImage);
    //保存文件
    connect(ui->actionSave,&QAction::triggered,this,&MainWindow::SaveImage);
    //新建
    connect(ui->actionNew,&QAction::triggered,this,&MainWindow::AddNew);
    //模糊
    connect(ui->actionBlur,&QAction::triggered,this,&MainWindow::DrawBlur);
    //锐化
    connect(ui->actionSharpen,&QAction::triggered,this,&MainWindow::DrawSharpen);
    //创建选区
    connect(ui->actionSelect,&QAction::triggered,this,&MainWindow::Select);
    //删除选区
    connect(ui->actionDeleteSelect,&QAction::triggered,this,&MainWindow::DeleteSelect);
    //移动选区
    connect(ui->actionMove,&QAction::triggered,this,&MainWindow::Move);

}

//打开图像
void MainWindow::OpenImage()
{
    //默认路径为C盘，限制图片格式为png和jpg
    QString filename = QFileDialog::getOpenFileName(this,tr("选择图片"),"C:",tr("Image(*.png *.jpg)"));
    //若打开失败则返回
    if(filename.isEmpty())
    {
        return;
    }
    else
    {
        QImage img;
        if(!(img.load(filename))) //判断是否成功加载图像
        {
            QMessageBox::information(this, tr("打开图像失败"),tr("打开图像失败!"));
            return;
        }

        //计算图片的适宜大小
        QPixmap pixmag = QPixmap::fromImage(img);
        int width = ui->label->width();
        int height = ui->label->height();
        QPixmap fitpixmag = pixmag.scaled(width,height,Qt ::KeepAspectRatio,Qt::SmoothTransformation);
        //ui->label->setPixmap(fitpixmag);
        m_pix = fitpixmag;
        update();//重绘画图事件
        m_isopen = true;//标记打开图片

    }
}

//保存图像
void MainWindow::SaveImage()
{
    //选择路径，默认路径为C盘
    QString filename1 = QFileDialog::getSaveFileName(this,tr("保存图片"),"C:",tr("Image(*.png *.jpg)"));
    QScreen *screen = QGuiApplication::primaryScreen();
    screen->grabWindow(ui->label->winId()).save(filename1);
}

//新建
void MainWindow::AddNew()
{
    m_leftpress = false;
    m_isopen = false;
    m_drawType = 0;
    m_rect.clear();
    m_penline.clear();
    m_pencolor.clear();
    m_penwidth.clear();
    m_eraserwidth.clear();
    m_eraserline.clear();
    m_shape.clear();
    m_isselect = false;
    m_ismove = false;
    DeleteSelect();
}

//画线
void MainWindow::DrawLine()
{
    m_drawType = 1;
}

//橡皮
void MainWindow::DrawEraser()
{
    m_drawType = 2;
}

//矩形
void MainWindow::DrawRect()
{
    m_drawType = 3;
}


//模糊
void MainWindow::DrawBlur()
{
    if(m_isoverselect)
    {
        QPixmap pix = this->grab(QRect(m_begin,m_end));
        QImage img = pix.toImage();
        m_pix =QPixmap::fromImage(Copy(Blur(img)));
        update();
    }
    if(!m_isoverselect)
    {
        m_pix = this->grab(QRect(0,0,ui->label->width(),ui->label->height()));
        QImage img = m_pix.toImage();
        m_pix = QPixmap::fromImage(Blur(img));
        update();
    }
}
//模糊函数
QImage MainWindow::Blur(QImage origin)
{
  QImage * newImage = new QImage(origin);

  int kernel [5][5]= {{0,0,1,0,0},
                      {0,1,3,1,0},
                      {1,3,7,3,1},
                      {0,1,3,1,0},
                      {0,0,1,0,0}};
  int kernelSize = 5;
  int sumKernel = 27;
  int r,g,b;
  QColor color;

  for(int x=kernelSize/2; x<newImage->width()-(kernelSize/2); x++)
  {
    for(int y=kernelSize/2; y<newImage->height()-(kernelSize/2); y++)
    {
      r = 0;
      g = 0;
      b = 0;

      for(int i = -kernelSize/2; i<= kernelSize/2; i++)
      {
        for(int j = -kernelSize/2; j<= kernelSize/2; j++)
        {
          color = QColor(origin.pixel(x+i, y+j));
          r += color.red()*kernel[kernelSize/2+i][kernelSize/2+j];
          g += color.green()*kernel[kernelSize/2+i][kernelSize/2+j];
          b += color.blue()*kernel[kernelSize/2+i][kernelSize/2+j];
        }
      }

      r = qBound(0, r/sumKernel, 255);
      g = qBound(0, g/sumKernel, 255);
      b = qBound(0, b/sumKernel, 255);

      newImage->setPixel(x,y, qRgb(r,g,b));
      }
   }
   return *newImage;
}


//锐化槽函数
void MainWindow::DrawSharpen()
{
    if(m_isoverselect)
    {
        QPixmap pix = this->grab(QRect(m_begin,m_end));
        QImage img = pix.toImage();
        m_pix =QPixmap::fromImage(Copy(Sharpen(img)));
        update();
    }
    if(!m_isoverselect)
    {
        m_pix = this->grab(QRect(0,0,ui->label->width(),ui->label->height()));
        QImage img = m_pix.toImage();
        m_pix = QPixmap::fromImage(Sharpen(img));
        update();
    }
}
//锐化函数
QImage  MainWindow::Sharpen(QImage  origin)
{
  QImage  *newImage = new QImage( origin);
  int kernel [3][3]= {{0,-1,0},
                      {-1,5,-1},
                      {0,-1,0}};
  int kernelSize = 3;
  int sumKernel = 1;
  int r,g,b;
  QColor color;

  for(int x=kernelSize/2; x<newImage->width()-(kernelSize/2); x++)
  {
    for(int y=kernelSize/2; y<newImage->height()-(kernelSize/2); y++)
    {
      r = 0;
      g = 0;
      b = 0;

      for(int i = -kernelSize/2; i<= kernelSize/2; i++)
      {
        for(int j = -kernelSize/2; j<= kernelSize/2; j++)
        {
          color = QColor(origin.pixel(x+i, y+j));
          r += color.red()*kernel[kernelSize/2+i][kernelSize/2+j];
          g += color.green()*kernel[kernelSize/2+i][kernelSize/2+j];
          b += color.blue()*kernel[kernelSize/2+i][kernelSize/2+j];
        }
      }

      r = qBound(0, r/sumKernel, 255);
      g = qBound(0, g/sumKernel, 255);
      b = qBound(0, b/sumKernel, 255);
      newImage->setPixel(x,y, qRgb(r,g,b));
      }
    }
    return *newImage;
}



//选区
void MainWindow::Select()
{

    m_drawType = 4;
    m_isselect = true;
}
void MainWindow::DeleteSelect()
{
    m_isoverselect = false;
    //m_selectrect.insert(0,QRect(0,0,0,0));
    m_drawType = 0;
    update();
}
QImage MainWindow::Copy(QImage img)
{
    QImage newImage = m_pix.toImage();
    for(int x = m_begin.x() , i = 0; x<=m_end.x() && i<=m_end.x()-m_begin.x() ; x++ , i++)
    {
        for(int y = m_begin.y() , j = 0 ; y<=m_end.y() && j <=m_end.y()-m_begin.y(); y++,j++)
        {
            newImage.setPixel(x,y,img.pixel(i,j));
        }

    }
    return newImage;

}
QPoint MainWindow::GetBeginPoint(QPoint begin)
{
    return begin;
}
QPoint MainWindow::GetEndPoint(QPoint end)
{
    return end;
}

//选区移动
void MainWindow::Move()
{
    m_ismove = true;
    m_drawType = 0;
}
QImage MainWindow::MoveCopy()
{

    //m_pix = this->grab(QRect(0,0,ui->label->width(),ui->label->height()));
    QImage newImage = m_pix.toImage();
    QImage whiteimg(ui->label->width(),ui->label->height(),newImage.format());
    whiteimg.fill(Qt::white);
    for(int x = m_begin.x(); x<=m_end.x(); x++ )
    {
        for(int y = m_begin.y() ; y<=m_end.y() ; y++)
        {
            newImage.setPixel(x,y,whiteimg.pixel(x,y));
        }

    }
    QImage img = m_pix.toImage();
    for(int x1 = m_begin.x() , x2 = m_begin.x()+m_endmove.x()-m_beginmove.x();x1<=m_end.x();x1++,x2++)
    {
        for(int y1 = m_begin.y() , y2 = m_begin.y()+m_endmove.y()-m_beginmove.y();y1<=m_end.y();y1++,y2++)
        {
            newImage.setPixel(x2,y2,img.pixel(x1,y1));
        }
    }
    for(int i = 0;i < m_penline.size() ; i++)
    {
        for(int j = 0 ; j<m_penline[i].size();j++)
        {
            if(m_penline[i][j].x()>=m_begin.x()&&m_penline[i][j].x()<=m_end.x()&&m_penline[i][j].y()>=m_begin.y()&&m_penline[i][j].y()<=m_end.y())
            {
                m_penline[i][j] += m_endmove-m_beginmove;
            }
        }
    }
    for(int i = 0;i < m_eraserline.size() ; i++)
    {
        for(int j = 0 ; j<m_eraserline[i].size();j++)
        {
            if(m_eraserline[i][j].x()>=m_begin.x()&&m_eraserline[i][j].x()<=m_end.x()&&m_eraserline[i][j].y()>=m_begin.y()&&m_eraserline[i][j].y()<=m_end.y())
            {
                m_eraserline[i][j] += m_endmove-m_beginmove;
            }
        }
    }
    for(int i = 0;i < m_rect.size() ; i++)
    {
        if(m_rect[i].topLeft().x()>=m_begin.x()&&m_rect[i].topLeft().x()<=m_end.x()&&m_rect[i].bottomRight().x()>=m_begin.x()&&m_rect[i].bottomRight().x()<=m_end.x()
                &&m_rect[i].topLeft().y()>=m_begin.y()&&m_rect[i].topLeft().y()<=m_end.y()&&m_rect[i].bottomRight().y()>=m_begin.y()&&m_rect[i].bottomRight().y()<=m_end.y())
        {
//            m_rect[i].topLeft() += m_endmove-m_beginmove;
//            m_rect[i].bottomRight() += m_endmove-m_beginmove;
            m_rect[i].moveTopLeft(m_rect[i].topLeft()+m_endmove-m_beginmove);
        }
    }
//    m_selectrect[0].topLeft() += m_endmove-m_beginmove;
//    m_selectrect[0].bottomRight() += m_endmove-m_beginmove;
    m_selectrect[0].moveTopLeft(m_selectrect[0].topLeft()+m_endmove-m_beginmove);

    return newImage;
}



//绘图事件
void MainWindow::paintEvent(QPaintEvent *)
{

    QPainter painter;
    if(!m_isopen)//若没有打开图片则新建一个白色背景
    {
        m_pix = QPixmap(ui->label->width(),ui->label->height());
        m_pix.fill(Qt::white);
    }

    QPixmap pix;
    pix = m_pix;
    painter.begin(&pix);

    int i1 = 0 , i2 = 0 , i3 = 0 , i4 = 0;//索引
    for(int c = 0;c<m_shape.size();++c)
    {
        if(m_shape.at(c) == 1)//画线
        {
            //设置笔刷
            const QVector<QColor>& pencolor = m_pencolor.at(i1);
            const QVector<int>& penwidth = m_penwidth.at(i1);
            for(int k = 0 ; k<pencolor.size();k++)//一条线对应一个笔刷和颜色
            {
                QPen pen;
                pen.setColor(pencolor.at(k));
                pen.setWidth(penwidth.at(k));
                painter.setPen(pen);
            }
            //画线
            const QVector<QPoint>& penline = m_penline.at(i1++);
            for(int j = 0 ; j < penline.size()-1 ; j++)//将每两个点依次连起来
            {
                painter.drawLine(penline.at(j),penline.at(j+1));
            }
        }
        if(m_shape.at(c) == 2)//橡皮
        {
            QPen pen;
            pen.setColor(Qt::white);//橡皮为白色，与画笔类似
            const QVector<int>& eraserwidth = m_eraserwidth.at(i2);
            for(int k = 0 ; k<eraserwidth.size() ; k++)
            {
                pen.setWidth(eraserwidth.at(k));
                painter.setPen(pen);
            }
            const QVector<QPoint>& eraserline = m_eraserline.at(i2++);
            for(int j = 0 ; j < eraserline.size()-1 ; j++)
            {
                painter.drawLine(eraserline.at(j),eraserline.at(j+1));
            }
        }
        if(m_shape.at(c) == 3)//矩形
        {

            const QVector<QColor>& rectcolor = m_rectcolor.at(i3);
            const QVector<int>& rectwidth = m_rectwidth.at(i3);
            for(int k = 0 ; k<rectcolor.size();k++)//一条线对应一个笔刷和颜色
            {
                QPen pen;
                pen.setColor(rectcolor.at(k));
                pen.setWidth(rectwidth.at(k));
                painter.setPen(pen);
            }
            painter.drawRect(m_rect.at(i3++));
        }
        if(m_shape.at(c) == 4)
        {
            if(m_isoverselect||m_isselect)
            {
                QPen pen;
                pen.setColor(Qt::black);
                pen.setStyle(Qt::DotLine);
                pen.setWidth(2);
                painter.setPen(pen);
                painter.drawRect(m_selectrect.at(i4++));
            }
        }
    }


    painter.end();
    //m_pix = pix;
    painter.begin(this);//将当前窗体作为画布
    painter.drawPixmap(0,0,pix);//将pixmap画到窗体


}

//鼠标按下事件
void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)//当鼠标左键按下
    {
        if(m_drawType == 1) //画线
        {

           m_leftpress = true;

           //设置点集

           QVector<QPoint> penline;//鼠标按下，新的线条开始
           m_penline.append(penline);//将新线条添加到线条集合
           QVector<QPoint>& penlastLine = m_penline.last();//拿到新线条
           if(m_isoverselect == true)
           {
               if(e->pos().x()>=m_begin.x()&&e->pos().x()<=m_end.x()&&e->pos().y()>=m_begin.y()&&e->pos().y()<=m_end.y())
               {
                   penlastLine.append(e->pos());//记录鼠标的坐标(新线条的开始坐标)
               }
           }
           if(m_isoverselect == false)
           {
               penlastLine.append(e->pos());//记录鼠标的坐标(新线条的开始坐标)
           }



           //设置颜色集
           QVector<QColor> pencolor;
           m_pencolor.append(pencolor);
           QVector<QColor>& lastColor = m_pencolor.last();
           lastColor.append(QColor(ui->Red->value(),ui->Green->value(),ui->Blue->value(),ui->Alpha->value()));

           //设置笔刷集
           QVector<int> penwidth;
           m_penwidth.append(penwidth);
           QVector<int>& penlastWidth = m_penwidth.last();
           penlastWidth.append(int(ui->width->value()));


           m_shape.append(1);
        }
        if(m_drawType == 2)
        {
            m_leftpress = true;

            //设置点集
            QVector<QPoint> eraserline;//鼠标按下，新的线条开始
            m_eraserline.append(eraserline);//将新线条添加到线条集合
            QVector<QPoint>& eraserlastLine = m_eraserline.last();//拿到新线条
            //eraserlastLine.append(e->pos());//记录鼠标的坐标(新线条的开始坐标)
            if(m_isoverselect == true)
            {
                if(e->pos().x()>=m_begin.x()&&e->pos().x()<=m_end.x()&&e->pos().y()>=m_begin.y()&&e->pos().y()<=m_end.y())
                {
                    eraserlastLine.append(e->pos());//记录鼠标的坐标(新线条的开始坐标)
                }
            }
            if(m_isoverselect == false)
            {
                eraserlastLine.append(e->pos());//记录鼠标的坐标(新线条的开始坐标)
            }


            //设置笔刷集
            QVector<int> eraserwidth;
            m_eraserwidth.append(eraserwidth);
            QVector<int>& eraserlastWidth = m_eraserwidth.last();
            eraserlastWidth.append(int(ui->width->value()));


            m_shape.append(2);
        }
        if(m_drawType == 3)
        {
            m_leftpress = true;


            QRect rect;//鼠标按下，矩形开始
            m_rect.append(rect);//将新矩形添加到矩形集合
            QRect& rectlastRect = m_rect.last();//拿到新矩形
//            lastRect.setTopLeft(e->pos());//记录鼠标的坐标(新矩形的左上角坐标)
            if(m_isoverselect == true)
            {
                if(e->pos().x()>=m_begin.x()&&e->pos().x()<=m_end.x()&&e->pos().y()>=m_begin.y()&&e->pos().y()<=m_end.y())
                {
                    rectlastRect.setTopLeft(e->pos());//记录鼠标的坐标(新矩形的左上角坐标)
                }
            }
            if(m_isoverselect == false)
            {
                rectlastRect.setTopLeft(e->pos());//记录鼠标的坐标(新矩形的左上角坐标)
            }


            //设置颜色集
            QVector<QColor> rectcolor;
            m_rectcolor.append(rectcolor);
            QVector<QColor>& rectlastColor = m_rectcolor.last();
            rectlastColor.append(QColor(ui->Red->value(),ui->Green->value(),ui->Blue->value(),ui->Alpha->value()));

            //设置笔刷集
            QVector<int> rectwidth;
            m_rectwidth.append(rectwidth);
            QVector<int>& rectlastWidth = m_rectwidth.last();
            rectlastWidth.append(int(ui->width->value()));

            m_shape.append(3);
        }
        if(m_drawType == 4)
        {
            m_leftpress = true;
            m_isselect = true;

            QRect rect;//鼠标按下，矩形开始

            m_selectrect.insert(0,rect);//将新矩形添加到矩形集合

            QRect& lastRect = m_selectrect.last();//拿到新矩形
            lastRect.setTopLeft(e->pos());//记录鼠标的坐标(新矩形的左上角坐标)

            m_shape.append(4);
        }
        if(m_isselect)
        {
            m_leftpress = true;
            m_begin = GetBeginPoint(e->pos());
        }
        if(m_ismove)
        {
            if(e->pos().x()>=m_begin.x()&&e->pos().x()<=m_end.x()&&e->pos().y()>=m_begin.y()&&e->pos().y()<=m_end.y())
            {
                m_leftpress = true;
                m_beginmove = GetBeginPoint(e->pos());
            }
            else
            {
                m_ismove = false;
            }
        }
    }
}


//鼠标释放事件
void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_leftpress)
    {
        if(m_drawType == 1)//画线
        {
            QVector<QPoint>& penlastLine = m_penline.last();//最后添加的线条，就是最新画的
            //penlastLine.append(e->pos());//记录线条的结束坐标
            if(m_isoverselect == true)
            {
                if(e->pos().x()>=m_begin.x()&&e->pos().x()<=m_end.x()&&e->pos().y()>=m_begin.y()&&e->pos().y()<=m_end.y())
                {
                    penlastLine.append(e->pos());//记录鼠标的坐标(新线条的开始坐标)
                }
            }
            if(m_isoverselect == false)
            {
                penlastLine.append(e->pos());//记录鼠标的坐标(新线条的开始坐标)
            }
            m_leftpress = false;
        }
        if(m_drawType == 2)//橡皮
        {
            QVector<QPoint>& eraserlastLine = m_eraserline.last();//最后添加的线条，就是最新画的
            //eraserlastLine.append(e->pos());//记录线条的结束坐标
            if(m_isoverselect == true)
            {
                if(e->pos().x()>=m_begin.x()&&e->pos().x()<=m_end.x()&&e->pos().y()>=m_begin.y()&&e->pos().y()<=m_end.y())
                {
                   eraserlastLine.append(e->pos());//记录线条的结束坐标
                }
            }
            if(m_isoverselect == false)
            {
                eraserlastLine.append(e->pos());//记录线条的结束坐标
            }
            m_leftpress = false;
        }
        if(m_drawType == 3)
        {
            QRect& rectlastRect = m_rect.last();
//            rectlastRect.setBottomRight(e->pos());//设置右下角
            if(m_isoverselect == true)
            {
                if(e->pos().x()>=m_begin.x()&&e->pos().x()<=m_end.x()&&e->pos().y()>=m_begin.y()&&e->pos().y()<=m_end.y())
                {
                   rectlastRect.setBottomRight(e->pos());//设置右下角
                }
            }
            if(m_isoverselect == false)
            {
                rectlastRect.setBottomRight(e->pos());//设置右下角
            }

            m_leftpress = false;
        }
        if(m_drawType == 4)
        {
            QRect& lastRect = m_selectrect.last();
            lastRect.setBottomRight(e->pos());//设置右下角
            m_leftpress = false;
        }
        if(m_isselect)
        {
            m_end = GetEndPoint(e->pos());
            m_selectpress = false;
            m_isselect = false;
            m_leftpress = false;
            //m_selectimg = Copy();
            //m_selectpix = QPixmap::fromImage(m_selectimg);
            //m_selectpix = QPixmap(m_end.x()-m_begin.x(),m_end.y()-m_begin.y());
            m_isoverselect = true;
            update();
        }
        if(m_ismove)
        {
            m_leftpress = false;
            m_ismove = false;


            m_endmove = GetEndPoint(e->pos());
            m_pix = QPixmap::fromImage(MoveCopy());
            m_begin = m_begin+m_endmove-m_beginmove;
            m_end = m_end+ m_endmove-m_beginmove;
            update();
        }
    }


}

//鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(m_leftpress)
    {
        if(m_drawType == 1)//画线
        {
            if(m_penline.size()<=0) return;//线条集合为空，不画线
            QVector<QPoint>& penlastLine = m_penline.last();//最后添加的线条，就是最新画的
            //penlastLine.append(e->pos());//记录鼠标的坐标(线条的轨迹)
            if(m_isoverselect == true)
            {
                if(e->pos().x()>=m_begin.x()&&e->pos().x()<=m_end.x()&&e->pos().y()>=m_begin.y()&&e->pos().y()<=m_end.y())
                {
                    penlastLine.append(e->pos());//记录鼠标的坐标(新线条的开始坐标)
                }
            }
            if(m_isoverselect == false)
            {
                penlastLine.append(e->pos());//记录鼠标的坐标(新线条的开始坐标)
            }
            update();//触发窗体重绘
        }
        if(m_drawType == 2)//橡皮
        {
            if(m_eraserline.size()<=0) return;//线条集合为空，不画线
            QVector<QPoint>& eraserlastLine = m_eraserline.last();//最后添加的线条，就是最新画的
            //eraserlastLine.append(e->pos());//记录鼠标的坐标(线条的轨迹)
            if(m_isoverselect == true)
            {
                if(e->pos().x()>=m_begin.x()&&e->pos().x()<=m_end.x()&&e->pos().y()>=m_begin.y()&&e->pos().y()<=m_end.y())
                {
                    eraserlastLine.append(e->pos());//记录鼠标的坐标(线条的轨迹)
                }
            }
            if(m_isoverselect == false)
            {
                eraserlastLine.append(e->pos());//记录鼠标的坐标(线条的轨迹)
            }
            update();//触发窗体重绘
        }
        if(m_drawType == 3)
        {
            if(m_rect.size()<=0) return;
            QRect& rectlastRect = m_rect.last();//拿到新矩形
//            rectlastRect.setBottomRight(e->pos());//更新矩形的右下角坐标
            if(m_isoverselect == true)
            {
                if(e->pos().x()>=m_begin.x()&&e->pos().x()<=m_end.x()&&e->pos().y()>=m_begin.y()&&e->pos().y()<=m_end.y())
                {
                    rectlastRect.setBottomRight(e->pos());//更新矩形的右下角坐标
                }
            }
            if(m_isoverselect == false)
            {
               rectlastRect.setBottomRight(e->pos());//更新矩形的右下角坐标
            }
            update();//触发窗体重绘
        }
        if(m_drawType == 4)
        {
            if(m_selectrect.size()<=0) return;
            QRect& lastRect = m_selectrect.last();//拿到新矩形
            lastRect.setBottomRight(e->pos());//更新矩形的右下角坐标
            update();//触发窗体重绘
        }

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

