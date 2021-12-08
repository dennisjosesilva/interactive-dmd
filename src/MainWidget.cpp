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

MainWidget::MainWidget(QWidget *parent)
  :QWidget{parent},
   needTreeVisualiserUpdate_{false},
   nodeSelectionByClickActivated_{false},
   ctrlDown_{false},
   reconMode_{ReconMode::MorphoTree}
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

  connect(treeVis_, &TreeVisualiser::nodeSelected, this, &MainWidget::treeVis_NodeSelected);
  connect(treeVis_, &TreeVisualiser::nodeUnselected, this, &MainWidget::treeVis_NodeUnselected);
  
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

  MainWindow *mainWindow = qobject_cast<MainWindow*>(parent());
  mainWindow->setMinMaxProgressBar(0, treeVis_->numberOfNodesOfTree()-1);
  mainWindow->statusBar()->clearMessage();
  mainWindow->statusBar()->showMessage(tr("Loading Skeleton (DMD)"));
  treeVis_->registerDMDSkeletons();  
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

  if (highlightNode_ && treeVis_->hasNodeSelected()) {
    highlightNode(treeVis_->curSelectedNode());
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
  MainWindow *mainWindow = qobject_cast<MainWindow*>(parent());
  dockTreeVisualiser_ = new ClosableDockWidget{"Morphotree", mainWindow};

  dockTreeVisualiser_->setVisible(false);
  dockTreeVisualiser_->setFloating(true);  
  dockTreeVisualiser_->setWidget(treeVis_);

  connect(dockTreeVisualiser_, &ClosableDockWidget::closed, 
    [mainWindow](ClosableDockWidget *dock) { mainWindow->showTreeVisAct()->setChecked(false); }
  );
}

void MainWidget::createDockWidgetSdmd()
{
  dockWidgetSdmd_ = new QDockWidget{"SDMD processing", 
    qobject_cast<QMainWindow*>(parent())};
    
  dockWidgetSdmd_->setVisible(false);
  dockWidgetSdmd_->setFloating(true);  
  dockWidgetSdmd_->setWidget(Interactive_sdmd);
 
}

void MainWidget::highlightNode(GNode *node)
{
  using morphotree::Box;

  if (reconMode_ == ReconMode::MorphoTree) {
    Box domain = treeVis_->domain();
    //std::vector<bool> nodeImg = node->simplifiedMTreeNode()->reconstruct(domain);
    std::vector<bool> nodeImg = node->mnode()->reconstruct(domain);
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
    FIELD<float> *nodeImg = treeVis_->SDMDReconstruction(node->mnode()->id());
    QImage bimg { nodeImg->dimX(), nodeImg->dimY(), QImage::Format_ARGB32 };

    for (int l = 0; l < nodeImg->dimY(); l++) {
      QRgb *line = reinterpret_cast<QRgb*>(bimg.scanLine(l));
      for (int c = 0; c < nodeImg->dimX(); c++) {
        if ((*nodeImg)(c, l) > 0) 
          line[c] = qRgba(255, 0, 0, 200);
        else 
          line[c] = qRgba(0, 0, 0, 0);
      }
    }

    imageViewer_->setOverlayImage(bimg);
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

void MainWidget::treeVis_NodeSelected(GNode *node)
{
  if (highlightNode_)
    highlightNode(node);
}

void MainWidget::treeVis_NodeUnselected(GNode *node) 
{
  imageViewer_->removeOverlay();
}

void MainWidget::ChangeDisplayImg(QImage img)
{
  imageViewer_->setImage(img);
}

void MainWidget::DisplaySelectedNodes(vector<int> selectedIntensity)
{  
  treeVis_->selectNodesForRecBasedOnIntensities(selectedIntensity);
  setReconMode(ReconMode::SDMD);
}

void MainWidget::DisplayOrigImg()
{
  imageViewer_->setImage(newImage);
}