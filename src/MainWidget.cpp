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

#include <MorphotreeWidget/TreeLayout/TreeLayout.hpp>
#include <MorphotreeWidget/TreeSimplification/TreeSimplification.hpp>
#include <MorphotreeWidget/Graphics/GNodeEventHandler.hpp>

#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>

#include <QDockWidget>

#include <QDebug>


MainWidget::MainWidget(QWidget *parent)
  :QWidget{parent},
   needMorphotreeWidgetUpdate_{false}
{ 
  namespace iv = ImageViewerWidget;
  namespace mw = MorphotreeWidget;

  QLayout *mainLayout = new QVBoxLayout;

  imageViewer_ = new iv::ImageViewerWidget{this};
  morphotreeWidget_ = new mw::MorphotreeWidget{mw::TreeLayout::TreeLayoutType::GraphvizWithLevel};

  createDockMorphotreeWidget();

  connect(mw::GNodeEventHandler::Singleton(), &mw::GNodeEventHandler::mousePress,
    this, &MainWidget::morphotreeWidget_onNodeMousePress); 

  connect(mw::GNodeEventHandler::Singleton(), &mw::GNodeEventHandler::mouseRelease,
    this, &MainWidget::morphotreeWidget_onNodeMouseRelease); 

  connect(mw::GNodeEventHandler::Singleton(), &mw::GNodeEventHandler::mouseDoubleClick,
    this, &MainWidget::morphotreeWidget_onNodeMouseDoubleClick); 

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
  needMorphotreeWidgetUpdate_ = true;

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

void MainWidget::updateMorphotreeWidget()
{
  namespace mt = morphotree;
  namespace mw = MorphotreeWidget;

  const QImage &image = imageViewer_->image();
  mt::Box domain = mt::Box::fromSize(mt::UI32Point{ 
    static_cast<mt::uint32>(image.width()), 
    static_cast<mt::uint32>(image.height())});

  const uchar *bits = image.bits();
  std::vector<mt::uint8> f{ bits, bits + domain.numberOfPoints() };

  // morphotreeWidget_->loadImage(domain, f, 
  //   std::make_shared<mw::TreeSimplificationProgressiveAreaDifferenceFilter>(6, 50, 180));

  morphotreeWidget_->loadImage(domain, f, 
    std::make_shared<mw::TreeSimplificationProgressiveAreaDifferenceFilter>(6, 1000, 180));

  needMorphotreeWidgetUpdate_ = false;
}

void MainWidget::createDockMorphotreeWidget()
{
  dockMorphotreeWidget_ = new QDockWidget{"Morphotree", 
    qobject_cast<QMainWindow*>(parent())};

  dockMorphotreeWidget_->setVisible(false);
  dockMorphotreeWidget_->setFloating(true);
  dockMorphotreeWidget_->setWidget(morphotreeWidget_);
}

QDockWidget *MainWidget::morphotreeDockWidget()
{
  if (needMorphotreeWidgetUpdate_) 
    updateMorphotreeWidget();
  
  return dockMorphotreeWidget_;
}

void MainWidget::morphotreeWidget_onNodeMousePress(MorphotreeWidget::GNode *node)
{
  qDebug() << "Node Press";
}

void MainWidget::morphotreeWidget_onNodeMouseRelease(MorphotreeWidget::GNode *node)
{
  qDebug() << "Node Release";
}

void MainWidget::morphotreeWidget_onNodeMouseDoubleClick(MorphotreeWidget::GNode *node)
{
  qDebug() << "Node Double Click";
}