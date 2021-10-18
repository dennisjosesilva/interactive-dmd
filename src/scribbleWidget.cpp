/*
#include "ImageViewerWidget/ImageViewerWidget.hpp"

#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QImage>
#include <QMouseEvent>
#include <QLayout>
#include <QVBoxLayout>
#include <QImageWriter>
#include <QImageReader>

#include <QPainter>

namespace ImageViewerWidget
{
  ImageViewerWidget::ImageViewerWidget(QWidget *parent)
    : QWidget{parent},
      imageLabel_{new QLabel},
      scrollArea_{new QScrollArea},
      scaleFactor_{1}      
  {
    QLayout *mainLayout = new QVBoxLayout;

    imageLabel_->setBackgroundRole(QPalette::Base);
    imageLabel_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel_->setMargin(0);
    
    imageLabel_->setFrameStyle(QFrame::NoFrame);        
    imageLabel_->setScaledContents(true);

    scrollArea_->setBackgroundRole(QPalette::Dark);
    scrollArea_->setFrameStyle(QFrame::NoFrame);
    scrollArea_->setWidget(imageLabel_);    
    scrollArea_->setVisible(false);
    
    mainLayout->setContentsMargins(0, 0, 0, 0);    
    mainLayout->setSpacing(0);    

    mainLayout->addWidget(scrollArea_);
    setLayout(mainLayout);
  }

  bool ImageViewerWidget::saveImage(const QString &fileName)
  {
    QImageWriter writer{fileName};
    
    if (writer.write(image_))
      return true;
    
    return false;
  }

  bool ImageViewerWidget::loadImage(const QString &fileName)
  {
    QImageReader reader{fileName};
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    
    if (newImage.isNull()) 
      return false;

    setImage(newImage);

    return true;
  }

  void ImageViewerWidget::setImage(const QImage &newImage)
  {
    image_ = newImage;
    resultImage_ = newImage;
    overlayImage_ = QImage{};

    imageLabel_->setPixmap(QPixmap::fromImage(image_));
    scaleFactor_ = 1.0;
    normalImageSize_ = QSize{newImage.size()};

    normalSize();

    scrollArea_->setVisible(true);
    scrollArea_->setEnabled(true);

    imageLabel_->update();
  }

  void ImageViewerWidget::setOverlayImage(const QImage &secondImage)
  {
    overlayImage_ = secondImage;
    performImageComposition();
  }

  void ImageViewerWidget::removeOverlay()
  {
    overlayImage_ = QImage{};
    resultImage_ = image_;
    imageLabel_->setPixmap(QPixmap::fromImage(resultImage_));
    imageLabel_->update();
  }

  void ImageViewerWidget::performImageComposition()
  {
    resultImage_ = QImage{image_.size(), QImage::Format_ARGB32_Premultiplied};
    QPainter painter{&resultImage_};
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(imageLabel_->rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, image_); // destination image
    painter.drawImage(0, 0, overlayImage_);  // source image
    painter.end();

    imageLabel_->setPixmap(QPixmap::fromImage(resultImage_));
    imageLabel_->update();
  }

  void ImageViewerWidget::fitToWidget()
  {
    scrollArea_->setWidgetResizable(true);
  }

  void ImageViewerWidget::unfitToWidget()
  {
    scrollArea_->setWidgetResizable(false);
  }

  void ImageViewerWidget::normalSize()
  {
    imageLabel_->adjustSize();
    scaleFactor_ = 1.0;
  }

  void ImageViewerWidget::zoomIn()
  {
    scaleImage(1.25);
  }

  void ImageViewerWidget::zoomOut()
  {
    scaleImage(0.8);
  }

  void ImageViewerWidget::scaleImage(double factor)
  {
    scaleFactor_ *= factor;
    imageLabel_->resize(scaleFactor_ * imageLabel_->pixmap()->size());

    adjustScrollBar(scrollArea_->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea_->verticalScrollBar(), factor);    
  } 

  void ImageViewerWidget::adjustScrollBar(QScrollBar *scrollBar, double factor)
  {
    scrollBar->setValue( static_cast<int>(factor * scrollBar->value()) +
      ((factor - 1) * scrollBar->pageStep() / 2));
  }

  QPointF ImageViewerWidget::scrollAreaPointMap(const QPointF &p) const
  {
    QPointF scrollOffset = QPointF(scrollArea_->horizontalScrollBar()->value(), 
       scrollArea_->verticalScrollBar()->value());

    return p + scrollOffset;
  }

  QPointF ImageViewerWidget::normalisePoint(const QPointF &p) const
  {
    return QPointF{ p.x() / imageLabel_->size().width(), 
      p.y() / imageLabel_->size().height() };
  }

  QPointF ImageViewerWidget::mapScrollPointToImagePoint(
    const QPointF &p_normalised) const
  {
    return QPointF{ p_normalised.x() * normalImageWidth(), 
      p_normalised.y() * normalImageHeight() };
  }


  void ImageViewerWidget::mousePressEvent(QMouseEvent *e)
  {
    QPointF p = scrollAreaPointMap(e->localPos()) - QPointF{ 2, 4 };
    QPointF p_normalised = normalisePoint(p);

    if ( 0 <= p_normalised.x() && p_normalised.x() < 1.0 && 
         0 <= p_normalised.y() && p_normalised.y() < 1.0) {      

      QPointF p_transformed = mapScrollPointToImagePoint(p_normalised);
      emit imageMousePress(p_transformed);
    }

    update();
  }

  void ImageViewerWidget::mouseReleaseEvent(QMouseEvent *e)
  {
    QPointF p = scrollAreaPointMap(e->localPos()) - QPointF{ 2, 4 };
    QPointF p_normalised = normalisePoint(p);

    if ( 0 <= p_normalised.x() && p_normalised.x() < 1.0 && 
         0 <= p_normalised.y() && p_normalised.y() < 1.0) {      

      QPointF p_transformed = mapScrollPointToImagePoint(p_normalised);
      emit imageMouseRelease(p_transformed);
    }

    update();   
  }

  void ImageViewerWidget::mouseDoubleClickEvent(QMouseEvent *e)
  {
    QPointF p = scrollAreaPointMap(e->localPos()) - QPointF{ 2, 4 };
    QPointF p_normalised = normalisePoint(p);

    if ( 0 <= p_normalised.x() && p_normalised.x() < 1.0 && 
         0 <= p_normalised.y() && p_normalised.y() < 1.0) {      

      QPointF p_transformed = mapScrollPointToImagePoint(p_normalised);
      emit imageMouseDoubleClick(p_transformed);
    }

    update();
  }
}
*/

#include <QtWidgets>

#include "scribbleWidget.hpp"

//! [0]
scribbleWidget::scribbleWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    modified = false;
    scribbling = false;
    myPenWidth = 50;
    myPenColor = Qt::blue;
    //myPenColor = QColor(0,0,255,20);
}
//! [0]

//! [1]
bool scribbleWidget::openImage(const QString &fileName)
//! [1] //! [2]
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;
    //QSize newSize = loadedImage.size().expandedTo(size());
    //QSize newSize = loadedImage.size();
    //printf("height: %d, width: %d. \n",newSize.height(),newSize.width());
    //resizeImage(&loadedImage, newSize);
    Image = loadedImage;
   
    //modified = false;
    update();
    return true;
}
void scribbleWidget::setImage(const QImage &newImage) 
{ 
  Image = newImage; 
  setFixedSize(Image.width(),Image.height());
  update();
}
//! [2]

//! [3]
bool scribbleWidget::saveImage(const QString &fileName, const char *fileFormat)
//! [3] //! [4]
{
    /*
    int w=image.width();
    int h=image.height();
    QImage diffImage(w, h, QImage::Format_Grayscale8);
    //QImage diffImage = image;
    
    for(int i=0;i<h;i++){
        QRgb *rgb1=(QRgb*)image.constScanLine(i);
        QRgb *rgb2=(QRgb*)originalImage.constScanLine(i);
        for(int j=0;j<w;j++){
            //diffImage.setPixel(i,j,qRgb(0, 0, 0));
            if(rgb1[j]!=rgb2[j])
                diffImage.setPixel(i,j,qRgb(255, 255, 255));
        }
    }
*/
    QImage visibleImage = Image;
    //resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}
//! [4]

//! [5]
void scribbleWidget::setPenColor(const QColor &newColor)
//! [5] //! [6]
{
    myPenColor = newColor;
}
//! [6]

//! [7]
void scribbleWidget::setPenWidth(int newWidth)
//! [7] //! [8]
{
    myPenWidth = newWidth;
}
//! [8]

//! [9]
void scribbleWidget::clearImage()
//! [9] //! [10]
{
    Image.fill(qRgb(255, 255, 255));
    //modified = true;
    update();
}
//! [10]

//! [11]
void scribbleWidget::mousePressEvent(QMouseEvent *event)
//! [11] //! [12]
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        scribbling = true;
    }
}

void scribbleWidget::mouseMoveEvent(QMouseEvent *event)
{
  if(!darwEllipseMode)
    if ((event->buttons() & Qt::LeftButton) && scribbling)
        drawLineTo(event->pos());
}

void scribbleWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
      if(darwEllipseMode){
        QPoint BottomRightPoint = event->pos();
        //update();
        QPainter painter(&Image);
        QRect rect(lastPoint, BottomRightPoint);
        painter.setPen(QPen(myPenColor));
        painter.setBrush(QBrush(myPenColor));
        painter.drawEllipse(rect);

        int rad = 2;
        update(QRect(lastPoint, BottomRightPoint).normalized()
                                        .adjusted(-rad, -rad, +rad, +rad));

      }
      else
        drawLineTo(event->pos());
        
      scribbling = false;
    }
}

//! [12] //! [13]
void scribbleWidget::paintEvent(QPaintEvent *event)
//! [13] //! [14]
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, Image, dirtyRect);

}
//! [14]

//! [15]

void scribbleWidget::resizeEvent(QResizeEvent *event)
//! [15] //! [16]
{
    /*if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }*/
    //printf("height: %d, width: %d. \n",Image.height(),Image.width());
    printf("height: %d, width: %d. \n",height(), width());
    QWidget::resizeEvent(event);
}
//! [16]

//! [17]
void scribbleWidget::drawLineTo(const QPoint &endPoint)
//! [17] //! [18]
{
    QPainter painter(&Image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    
    painter.drawLine(lastPoint, endPoint);
    modified = true;

    int rad = (myPenWidth / 2) + 2;
    update(QRect(lastPoint, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}
//! [18]

//! [19]
void scribbleWidget::resizeImage(QImage *image, const QSize &newSize)
//! [19] //! [20]
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}
//! [20]
