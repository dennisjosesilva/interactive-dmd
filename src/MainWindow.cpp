#include "MainWindow.hpp"

#include <QStatusBar>
#include <QMenuBar>
#include <QAction>

#include <QFileDialog>
#include <QStandardPaths>
#include <QByteArrayList>
#include <QImageReader>
#include <QImageWriter>
#include <QImage>

#include <QDebug>

MainWindow::MainWindow()
{
  setWindowTitle("Interactive DMD");  

  mainWidget_ = new MainWidget{this};
  setCentralWidget(mainWidget_);

  createMenus();
  
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

  while (true) {
    bool accepted = dialog.exec() == QDialog::Accepted;
    const QString filename = dialog.selectedFiles().constFirst();
    
    if (accepted && mainWidget_->loadImage(filename)) {
      const QImage &image = mainWidget_->image();
      statusBar()->showMessage(tr("loaded %1 : dim: (%2 x %3)").arg(filename) 
        .arg(image.width()).arg(image.height()));
      break;
    }
  }  
}

void MainWindow::saveAs()
{
  statusBar()->showMessage(tr("Save as execution"), 2000);
}