#include "MainWindow.hpp"

#include <QStatusBar>
#include <QMenuBar>
#include <QAction>

#include <QFileDialog>
#include <QStandardPaths>
#include <QByteArrayList>
#include <QImageReader>
#include <QImageWriter>
#include <QDockWidget>
#include <QImage>
#include <QToolBar>
#include <QIcon>

#include <QDebug>

#include "dmdReconstruct.hpp"

MainWindow::MainWindow()
{
  setWindowTitle("Interactive DMD"); 
  //const QString filename = "../../images/Zuckerberg.pgm";
  const QString filename = "../images/art_deco.pgm";

  mainWidget_ = new MainWidget{this};
  mainWidget_->loadImage(filename);
  setCentralWidget(mainWidget_);

  dmd = new dmdProcess();
  const char *c_str = filename.toLocal8Bit().data();
  dmd->set_filename(c_str);
  dmd->readImage();
  printf("Read image from: %s \n",c_str);

  createMenus();
  createToolBar();
  
  statusBar()->showMessage(tr("Ready"), 3000);
}

void MainWindow::createMenus()
{
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
  
  QAction *openAct = fileMenu->addAction(tr("&Open"), this, &MainWindow::open);
  openAct->setToolTip(tr("Choose a image file from disk to be opened"));
  openAct->setShortcut(QKeySequence::Open);

  QAction *saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &MainWindow::saveAs);
  openAct->setToolTip(tr("Save the current image in disk."));
  saveAsAct->setShortcut(QKeySequence::SaveAs);

  fileMenu->addSeparator();

  QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
  exitAct->setToolTip("Finalise the application");
  exitAct->setShortcut(tr("Ctrl+Q"));
}

void MainWindow::createToolBar()
{
  QToolBar *toolbar = addToolBar(tr("Main"));
  toolbar->setIconSize(QSize{32, 32});

  const QIcon showTreeIcon = QIcon(":/images/morphotree_icon.png");  
  showTreeVisAct_ = new QAction(showTreeIcon, tr("Show morphotree"), this);
  showTreeVisAct_->setStatusTip(tr("Show Morphotree Widget for the current image"));
  showTreeVisAct_->setCheckable(true);
  connect(showTreeVisAct_, &QAction::toggled, this, &MainWindow::treeVisAct_onToggled);
  toolbar->addAction(showTreeVisAct_);

  const QIcon nodeSelectionClickIcon = QIcon(":/images/node_selection_pixel_icon.png");
  nodeSelectionClickAct_ = new QAction(nodeSelectionClickIcon, 
    tr("Node selection by pixel click"), this);
  nodeSelectionClickAct_->setStatusTip(tr("Toogle node selection by pixel click."));
  nodeSelectionClickAct_->setCheckable(true);
  connect(nodeSelectionClickAct_, &QAction::toggled, this, &MainWindow::nodeSelectionClickAct_onToggled);
  toolbar->addAction(nodeSelectionClickAct_);

  // ===== Image Tool bar ========
  QToolBar *imageToolBar = addToolBar("Image Toolbar");
  imageToolBar->setIconSize(QSize(32, 32));

  const QIcon imageZoomInIcon = QIcon(":/images/image_zoom_in_icon.png");
  imageZoomInAct_ = new QAction(imageZoomInIcon, tr("Image zoom in"), this);
  imageZoomInAct_->setStatusTip("Zoom in image");
  connect(imageZoomInAct_, &QAction::triggered, this, 
    &MainWindow::imageZoomInAct_onTrigged);
  imageToolBar->addAction(imageZoomInAct_);

  const QIcon imageZoomOutIcon = QIcon(":/images/image_zoom_out_icon.png");
  imageZoomOutAct_ = new QAction(imageZoomOutIcon, tr("Image zoom out"), this);
  imageZoomOutAct_->setStatusTip("Zoom out image");
  connect(imageZoomOutAct_, &QAction::triggered, this, 
    &MainWindow::imageZoomOutAct_onTrigged);
  imageToolBar->addAction(imageZoomOutAct_);

  // ========= DMD tool bar ==============
  QToolBar *dmdToolBar = addToolBar("DMD Toolbar");
  dmdToolBar->setIconSize(QSize{ 32, 32});

  const QIcon dmdProcessIcon = QIcon{":/images/Skel_icon.png"};
  dmdProcessAct_ = new QAction{dmdProcessIcon, tr("DMD process"), this};
  dmdProcessAct_->setStatusTip("DMD process");
  connect(dmdProcessAct_, &QAction::triggered, this, &MainWindow::dmdProcessAct_onTrigged);
  dmdToolBar->addAction(dmdProcessAct_);
}


static void initialiseImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
  static bool firstDialog = true;

  if (firstDialog) {
    firstDialog = false;
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
  }

  QStringList mimeTypeFilters;
  const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
    ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
  
  for (const QByteArray &mimeTypeName : supportedMimeTypes)
    mimeTypeFilters.append(mimeTypeName);

  mimeTypeFilters.sort();
  dialog.setMimeTypeFilters(mimeTypeFilters);
  dialog.selectMimeTypeFilter("image/jpeg");
  dialog.setAcceptMode(acceptMode);
  if (acceptMode == QFileDialog::AcceptSave)
    dialog.setDefaultSuffix("jpg");
}

void MainWindow::open()
{  
  QFileDialog dialog(this, tr("Open File"));
  initialiseImageFileDialog(dialog, QFileDialog::AcceptOpen);

  bool accepted = dialog.exec() == QDialog::Accepted;
  
  if (accepted && dialog.selectedFiles().count() > 0) {    
    const QString filename = dialog.selectedFiles().constFirst();
    if (mainWidget_->loadImage(filename)) {
      const QImage &image = mainWidget_->image();
      statusBar()->showMessage(tr("loaded %1 : dim: (%2 x %3)").arg(filename) 
        .arg(image.width()).arg(image.height()), 3000); 
      
      if (showTreeVisAct_->isChecked())
        mainWidget_->updateTreeVisualiser();

    }

    //for DMD 
     ///Read the filename
    
    const char *c_str = filename.toLocal8Bit().data();
    dmd->set_filename(c_str);
    dmd->readImage();
    printf("Read image from: %s \n",c_str);
  }
  else {
    statusBar()->showMessage(tr("Image open has been canceled"), 3000);
  }    
}

void MainWindow::saveAs()
{
  QFileDialog dialog(this, tr("Save File As"));
  initialiseImageFileDialog(dialog, QFileDialog::AcceptSave);

  bool accepted = dialog.exec();
  
  if (accepted && dialog.selectedFiles().count() > 0) {
    const QString filename = dialog.selectedFiles().constFirst();
    if (mainWidget_->saveImage(filename))
      statusBar()->showMessage(tr("Wrote image to %1").arg(filename), 3000);  
    else
      statusBar()->showMessage(tr("Image has not been written due an error"), 3000);  
  }
  else {
    statusBar()->showMessage(tr("Image save operation has been canceled."), 3000);  
  }  
}

void MainWindow::dmdProcessAct_onTrigged()
{
 // dmd->curImage()->NewwritePGM("dmd01.pgm");
/* 
  // remove island
  dmd->removeIslands(0.1);

  dmd->LayerSelection(false, 2);
  
  dmd->computeSkeletons(0.4);

  dmdReconstruct recon;
  recon.readControlPoints();
  recon.ReconstructImage(true); // true for interpolation method.
 
  */
 dmd->Init_indexingSkeletons();

 const QString filename = "../images/l146.pgm";
 const char *c_str = filename.toLocal8Bit().data();
 FIELD<float> *cc = FIELD<float>::read(c_str);
 dmd->indexingSkeletons(cc, 146, 0);

 const QString filename_ = "../images/l240.pgm";
 c_str = filename_.toLocal8Bit().data();
 cc = FIELD<float>::read(c_str);

 dmd->indexingSkeletons(cc, 240, 1);

 dmdReconstruct recon;
 recon.readIndexingControlPoints();
 int nodeID = 0;
 // int action = 0; //delete
 int action = 0; //highlight
 recon.ReconstructIndexingImage(true, nodeID, action); //true for interpolation method.
 
}

void MainWindow::treeVisAct_onToggled(bool checked)
{  
  QDockWidget *dockMorphotreeWidget = mainWidget_->morphotreeDockWidget();
  if (checked) 
    dockMorphotreeWidget->setVisible(true);
  else 
    dockMorphotreeWidget->setVisible(false);
}

void MainWindow::nodeSelectionClickAct_onToggled(bool checked)
{
  mainWidget_->setNodeSelectionByClickActivated(checked);
}

void MainWindow::imageZoomInAct_onTrigged()
{
  mainWidget_->zoomIn();
}

void MainWindow::imageZoomOutAct_onTrigged()
{
  mainWidget_->zoomOut();
}