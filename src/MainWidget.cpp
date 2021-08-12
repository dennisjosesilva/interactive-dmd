#include "MainWidget.hpp"

#include <QMessageBox>
#include <QImageReader>
#include <QImageWriter>
#include <QImage>
#include <QGuiApplication>
#include <QDir>
#include <QStatusBar>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <ImageViewerWidget/ImageViewerWidget.hpp>

MainWidget::MainWidget(QWidget *parent)
  :QWidget{parent},
   imageViewer_{new ImageViewerWidget::ImageViewerWidget{this}}
{ 
  QLayout *mainLayout = new QVBoxLayout;

  

  mainLayout->addWidget(imageViewer_);

  setLayout(mainLayout);
}

bool MainWidget::loadImage(const QString &filename)
{
  namespace iv = ImageViewerWidget; 
  
  QImageReader reader{filename};
  reader.setAutoTransform(true);
  const QImage newImage = reader.read();
  
  if (newImage.isNull()) {
    QMessageBox::information(this, QGuiApplication::applicationDisplayName(), 
      tr("Cannot load %1: %2").arg(QDir::toNativeSeparators(filename), reader.errorString()));
    return false;
  }
  
  imageViewer_->setImage(newImage);

  return true;
}

bool MainWidget::saveImage(const QString &filename)
{
  QImageWriter writer(filename);

  if (!writer.write(imageViewer_->image())) {
    QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
      tr("Cannot write %1: %2").arg(QDir::toNativeSeparators(filename), writer.errorString()));
    return false;
  }

  return true;
}