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
#include <QAction>
#include <ImageViewerWidget/ImageViewerWidget.hpp>

#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>

#include <QDockWidget>

#include <QDebug>

#include "MainWindow.hpp"

MainWidget::MainWidget(MainWindow *mainWindow, QWidget *parent)
  :QWidget{parent},
   needTreeVisualiserUpdate_{false},
   nodeSelectionByClickActivated_{false},
   ctrlDown_{false},
   reconMode_{ReconMode::MorphoTree},
   mainWindow_{mainWindow}
{ 
  namespace iv = ImageViewerWidget;  
  namespace mw = IcicleMorphotreeWidget;

  QLayout *mainLayout = new QVBoxLayout;

  imageViewer_ = new iv::ImageViewerWidget{this};
  ThresCtl = new ThresholdControl{this};

  treeVis_ = new TreeVisualiser{this};
  Interactive_sdmd = new InteractiveSdmd();

  createDockTreeVisualiser();
  createDockWidgetSdmd();

  mainLayout->addWidget(ThresCtl);
  mainLayout->addWidget(imageViewer_);

  connect(imageViewer_, &iv::ImageViewerWidget::imageMousePress, 
    this, &MainWidget::imageMousePress);

  connect(treeVis_, &TreeVisualiser::nodeSelectionChanged, this, &MainWidget::treeVis_NodeSelected);
  // connect(treeVis_, &TreeVisualiser::nodeUnselected, this, &MainWidget::treeVis_NodeUnselected);
  
  connect(ThresCtl, &ThresholdControl::ImageHasBeenReconstructed, this, &MainWidget::ChangeDisplayImg);
  connect(ThresCtl, &ThresholdControl::LayerHasBeenSelected, this, &MainWidget::DisplaySelectedNodes);
  connect(ThresCtl, &ThresholdControl::DisplayOriginalImg, this, &MainWidget::DisplayOrigImg);

  imageViewer_->scrollAreaWidget()->viewport()->installEventFilter(this);
  

  setLayout(mainLayout);
}

bool MainWidget::loadImage(const QString &filename)
{
  namespace iv = ImageViewerWidget; 
  
  QImageReader reader{filename};
  reader.setAutoTransform(true);
  newImage = reader.read();
  
  if (newImage.isNull()) {
    QMessageBox::information(this, QGuiApplication::applicationDisplayName(), 
      tr("Cannot load %1: %2").arg(QDir::toNativeSeparators(filename), reader.errorString()));
    return false;
  }
  
  imageViewer_->setImage(newImage);
  needTreeVisualiserUpdate_ = true;

  Interactive_sdmd->setImage(newImage);
  const char *c_str = filename.toLocal8Bit().data();
  Interactive_sdmd->readIntoSdmd(c_str);

  ThresCtl->readImgIntoSdmd(c_str);
  
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
  namespace mw = IcicleMorphotreeWidget;

  const QImage &image = imageViewer_->image();
  mt::Box domain = mt::Box::fromSize(mt::UI32Point{ 
    static_cast<mt::uint32>(image.width()), 
    static_cast<mt::uint32>(image.height())});

  const uchar *bits = image.bits();
  std::vector<mt::uint8> f{ bits, bits + domain.numberOfPoints() };

  treeVis_->loadImage(domain, f);
  
  needTreeVisualiserUpdate_ = false;

  
  mainWindow_->setMinMaxProgressBar(0, treeVis_->numberOfNodesOfTree()-1);
  mainWindow_->statusBar()->clearMessage();
  mainWindow_->statusBar()->showMessage(tr("Loading Skeleton (DMD)"));
  treeVis_->registerDMDSkeletons();  
  HasUpdatedTreeVisualiser = true;
  reconMode_ = ReconMode::MorphoTree;
}

void MainWidget::zoomOut()
{
  imageViewer_->zoomOut();
}

void MainWidget::zoomIn()
{
  imageViewer_->zoomIn();
}

void MainWidget::setHighlightNodeActivated(bool checked)
{
  using Box = morphotree::Box;
  highlightNode_ = checked;

  if (highlightNode_ && treeVis_->numberOfSelectedNodes() > 0) {
    highlightNodes();
  }
  else {    
    imageViewer_->removeOverlay();
  }
}

void MainWidget::setImage(const QImage &image)
{
  imageViewer_->removeOverlay();
  imageViewer_->setImage(image);
}

void MainWidget::createDockTreeVisualiser()
{  
  dockTreeVisualiser_ = new ClosableDockWidget{"Morphotree", mainWindow_};

  dockTreeVisualiser_->setVisible(false);
  //dockTreeVisualiser_->setFloating(true);  
  mainWindow_->addDockWidget(Qt::RightDockWidgetArea, dockTreeVisualiser_);
  dockTreeVisualiser_->setWidget(treeVis_);

  connect(dockTreeVisualiser_, &ClosableDockWidget::closed, 
    [this](ClosableDockWidget *dock) { mainWindow_->showTreeVisAct()->setChecked(false); }
  );
}

void MainWidget::createDockWidgetSdmd()
{
  dockWidgetSdmd_ = new QDockWidget{"SDMD processing", mainWindow_};
    
  dockWidgetSdmd_->setVisible(false);
  dockWidgetSdmd_->setFloating(true);  
  dockWidgetSdmd_->setWidget(Interactive_sdmd);
 
}

void MainWidget::highlightNodes()
{
  using morphotree::Box;

  if (treeVis_->numberOfSelectedNodes() > 0) {
    if (reconMode_ == ReconMode::MorphoTree) {
      Box domain = treeVis_->domain();
      //std::vector<bool> nodeImg = node->simplifiedMTreeNode()->reconstruct(domain);
      std::vector<bool> nodeImg =  treeVis_->morphoRecSelectedNodes(); //node->mnode()->reconstruct(domain);
      QImage bimg{ static_cast<int>(domain.width()), static_cast<int>(domain.height()), 
        QImage::Format_ARGB32 };
      
      for (int l = 0; l < domain.height(); l++) {
        QRgb *line = reinterpret_cast<QRgb*>(bimg.scanLine(l));
        for (int c = 0; c < domain.width(); c++) {
          if (nodeImg[domain.pointToIndex(c, l)])
            line[c] = qRgba(255, 0, 0, 200);
          else 
            line[c] = qRgba(0, 0, 0, 0);
        }
      }

      imageViewer_->setOverlayImage(bimg);
    }
    else if (reconMode_ == ReconMode::SDMD) {
      Box domain = treeVis_->domain();
      std::vector<bool> nodeImg = //treeVis_->SDMDReconstruction(node->mnode()->id());
        treeVis_->SDMDRecontructionSelectedNodes();
      QImage bimg{ static_cast<int>(domain.width()), static_cast<int>(domain.height()), 
      QImage::Format_ARGB32 };
      
      for (int l = 0; l < domain.height(); l++) {
        QRgb *line = reinterpret_cast<QRgb*>(bimg.scanLine(l));
        for (int c = 0; c < domain.width(); c++) {
          if (nodeImg[domain.pointToIndex(c, l)])
            line[c] = qRgba(255, 0, 0, 200);
          else 
            line[c] = qRgba(0, 0, 0, 0);
        }
      }
      imageViewer_->setOverlayImage(bimg);
    }
  }
}

QDockWidget *MainWidget::morphotreeDockWidget()
{
  if (needTreeVisualiserUpdate_) 
    updateTreeVisualiser();
  
  return dockTreeVisualiser_;
}


MyDockWidget *MainWidget::createDockWidget(const QString &title, QWidget *widget)
{  
  MyDockWidget *dock = new MyDockWidget{title, mainWindow_};

  dock->setVisible(true);
  dock->setFloating(true);
  dock->setWidget(widget);  
  dock->setAttribute(Qt::WA_DeleteOnClose, true);
  return dock;
}

void MainWidget::imageMousePress(const QPointF &p, QMouseEvent *e)
{   
  if (nodeSelectionByClickActivated_) {
    if (e->modifiers() & Qt::ShiftModifier)    
      treeVis_->selectNodeByPixel(static_cast<int>(p.x()), static_cast<int>(p.y()), true);
    else 
      treeVis_->selectNodeByPixel(static_cast<int>(p.x()), static_cast<int>(p.y()), false);
  }
}

void MainWidget::wheelEvent(QWheelEvent *e)
{
  if (ctrlDown_) {
    imageViewer_->scaleImage(pow(2.0, -e->angleDelta().y() / 240.0));    
    e->accept();
  }
}

void MainWidget::keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Control) {  
    ctrlDown_ = true;
  }
}

void MainWidget::keyReleaseEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Control) {
    ctrlDown_ = false;
  }
}

bool MainWidget::eventFilter(QObject *obj, QEvent *evt)
{
  if (ctrlDown_) {
    return true;
  }
  else {
    return QWidget::eventFilter(obj, evt);
  }
}

void MainWidget::treeVis_NodeSelected()
{
  if (highlightNode_)
    highlightNodes();
}

void MainWidget::treeVis_NodeUnselected() 
{
  imageViewer_->removeOverlay();
}

void MainWidget::ChangeDisplayImg(QImage img)
{
  imageViewer_->setImage(img);
}

void MainWidget::DisplaySelectedNodes(vector<int> selectedIntensity)
{  
  if(HasUpdatedTreeVisualiser){
    treeVis_->selectNodesForRecBasedOnIntensities(selectedIntensity);
    setReconMode(ReconMode::SDMD);
  }
  
}

void MainWidget::DisplayOrigImg()
{
  imageViewer_->setImage(newImage);
}