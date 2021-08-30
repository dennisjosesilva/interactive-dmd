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

#include "dmdProcess.hpp"
#include "dmdReconstruct.hpp"

MainWindow::MainWindow()
{
  const char *filename = "../images/panda.pgm";
  
  setWindowTitle("Interactive DMD");  


  mainWidget_ = new MainWidget{this};
  mainWidget_->loadImage("../../images/Zuckerberg.pgm");
  setCentralWidget(mainWidget_);

  
  dmdProcess dmd;
  dmd.set_filename(filename);
 
  // read image 
  dmd.readImage();
  //dmd.curImage()->writePGM("dmd01.pgm");

  // remove islands
  dmd.removeIslands(0.1);
  //dmd.curImage()->writePGM("dmd02.pgm");

  dmd.LayerSelection(true, 15);
  //dmd.curImage()->writePGM("dmd03.pgm");
  dmd.computeSkeletons();
  //dmd.Encoding();

  dmdReconstruct recon;
  recon.readControlPoints();
  recon.ReconstructImage();

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

void MainWindow::treeVisAct_onToggled(bool checked)
{  
  QDockWidget *dockMorphotreeWidget = mainWidget_->morphotreeDockWidget();
  if (checked) 
    dockMorphotreeWidget->setVisible(true);
  else 
    dockMorphotreeWidget->setVisible(false);
}

