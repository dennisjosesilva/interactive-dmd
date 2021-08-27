#include "MainWidget.hpp"

#include <QMainWindow>

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

#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>

#include <QDockWidget>

#include <QDebug>

#include "TreeVisualiser/TreeVisualiser.hpp"


MainWidget::MainWidget(QWidget *parent)
  :QWidget{parent},
   needTreeVisualiserUpdate_{false},
   nodeSelectionByClickActivated_{false}
{ 
  namespace iv = ImageViewerWidget;  
  namespace mw = MorphotreeWidget;

  QLayout *mainLayout = new QVBoxLayout;

  imageViewer_ = new iv::ImageViewerWidget{this};
  treeVis_ = new TreeVisualiser{this};

  createDockTreeVisualiser();

  mainLayout->addWidget(imageViewer_);

  connect(imageViewer_, &iv::ImageViewerWidget::imageMousePress, 
    this, &MainWidget::imageMousePress);

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
  needTreeVisualiserUpdate_ = true;

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

void MainWidget::updateTreeVisualiser()
{
  namespace mt = morphotree;
  namespace mw = MorphotreeWidget;

  const QImage &image = imageViewer_->image();
  mt::Box domain = mt::Box::fromSize(mt::UI32Point{ 
    static_cast<mt::uint32>(image.width()), 
    static_cast<mt::uint32>(image.height())});

  const uchar *bits = image.bits();
  std::vector<mt::uint8> f{ bits, bits + domain.numberOfPoints() };

  // treeVis_->loadImage(domain, f, 
  //   std::make_shared<mw::TreeSimplificationProgressiveAreaDifferenceFilter>(6, 50, 180));

  treeVis_->loadImage(domain, f, 
    std::make_shared<mw::TreeSimplificationProgressiveAreaDifferenceFilter>(6, 1000, 180));

  needTreeVisualiserUpdate_ = false;
}

void MainWidget::createDockTreeVisualiser()
{
  dockTreeVisualiser_ = new QDockWidget{"Morphotree", 
    qobject_cast<QMainWindow*>(parent())};

  dockTreeVisualiser_->setVisible(false);
  dockTreeVisualiser_->setFloating(true);  
  dockTreeVisualiser_->setWidget(treeVis_);
}

QDockWidget *MainWidget::morphotreeDockWidget()
{
  if (needTreeVisualiserUpdate_) 
    updateTreeVisualiser();
  
  return dockTreeVisualiser_;
}


MyDockWidget *MainWidget::createDockWidget(const QString &title, QWidget *widget)
{  
  MyDockWidget *dock = 
    new MyDockWidget{title, qobject_cast<QMainWindow *>(parent())};

  dock->setVisible(true);
  dock->setFloating(true);
  dock->setWidget(widget);  
  dock->setAttribute(Qt::WA_DeleteOnClose, true);
  return dock;
}

void MainWidget::imageMousePress(const QPointF &p)
{
  if (nodeSelectionByClickActivated_) {
    treeVis_->selectNodeByPixel(static_cast<int>(p.x()), static_cast<int>(p.y()));
  }
}