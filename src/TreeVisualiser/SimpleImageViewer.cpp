#include "TreeVisualiser/SimpleImageViewer.hpp"
#include <QPushButton>
#include <QScrollArea>

#include <QWheelEvent>

#include <cmath>

#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

SimpleImageViewer::SimpleImageViewer(QWidget *parent)
  :QWidget{parent}
{
  QLayout *mainLayout = new QVBoxLayout;

  QLayout *btnLayout = new QHBoxLayout; 

  QPushButton *zoomInBtn = new QPushButton{QIcon{":/images/image_zoom_in_icon.png"}, 
    tr(""), this};
  zoomInBtn->setIconSize(QSize{32, 32});
  connect(zoomInBtn, &QPushButton::clicked, this, &SimpleImageViewer::zoomIn_onClick);

  QPushButton *zoomOutBtn = new QPushButton{QIcon{":/images/image_zoom_out_icon.png"},
    tr(""), this};
  zoomOutBtn->setIconSize(QSize{32 ,32});
  connect(zoomOutBtn, &QPushButton::clicked, this, &SimpleImageViewer::zoomOut_onClick);

  btnLayout->addWidget(zoomInBtn);
  btnLayout->addWidget(zoomOutBtn);

  mainLayout->addItem(btnLayout);

  imageViewer_ = new ImageViewerWidget{this};
  imageViewer_->scrollAreaWidget()->viewport()->installEventFilter(this);
  mainLayout->addWidget(imageViewer_);

  setLayout(mainLayout);
}


void SimpleImageViewer::zoomIn_onClick()
{
  imageViewer_->zoomIn();
}

void SimpleImageViewer::zoomOut_onClick()
{
  imageViewer_->zoomOut();
}

void SimpleImageViewer::wheelEvent(QWheelEvent *e)
{
  if (isCtrlDown_) {
    imageViewer_->scaleImage(pow(2.0, -e->angleDelta().y() / 240.0f));
    e->accept();
  }
}

void SimpleImageViewer::keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Control) {
    isCtrlDown_ = true;
  }
}

void SimpleImageViewer::keyReleaseEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Control) {
    isCtrlDown_ = false;
  }
}

bool SimpleImageViewer::eventFilter(QObject *obj, QEvent *e)
{
  if (isCtrlDown_) {
    return true;
  }
  else {
    return QWidget::eventFilter(obj, e);
  }
}

QSize SimpleImageViewer::sizeHint() const
{
  const QImage img = imageViewer_->image();

  return QSize{ img.width(), img.height() + 32 };
}