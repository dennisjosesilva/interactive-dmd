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
#include <QStackedWidget>
#include <QLabel>
#include <QComboBox>

#include <QStatusBar>
#include <QProgressBar>
#include <QApplication>

#include <QDebug>

#include "SDMD/dmdReconstruct.hpp"

MainWindow::MainWindow()
  :shouldUpdateProgressBar_{true}
{
  setWindowTitle("Interactive DMD"); 
  //const QString filename = "../../images/Zuckerberg.pgm";
  // const QString filename = "../images/bird.pgm";
  const QString filename = "../images/art1.pgm";
  
  mainWidget_ = new MainWidget{this};
 
  mainWidget_->loadImage(filename);
  centralWidget_ = new QStackedWidget{this};
  centralWidget_->addWidget(mainWidget_);
  centralWidget_->setCurrentIndex(0);
  setCentralWidget(centralWidget_);

  createMenus();
  createToolBar();  

  progressBar_ = new LabelWithProgressBar(statusBar());
  progressBar_->setLabelText(tr(""));
  progressBar_->hide();
  
  statusBar()->addPermanentWidget(progressBar_);
  
  connect(mainWidget_->treeVisualiser(), &TreeVisualiser::associateNodeToSkeleton,
    this, &MainWindow::treeVis_onNodeSkeletonAssociation);
  connect(progressBar_, &LabelWithProgressBar::fullProgressBar, 
    [this] {
      shouldUpdateProgressBar_ = false;
      statusBar()->clearMessage(); 
      progressBar_->hide(); 
  });
      
  statusBar()->showMessage(tr("Ready"), 3000);

  connect(mainWidget_->getTreeVis(), &TreeVisualiser::ChangeCentralWidget,
    this, &MainWindow::ChangeCentralWidget_);
}

void MainWindow::ChangeCentralWidget_(CpViewer *cv)
{
  if (centralWidget_->count() == 2)
      centralWidget_->removeWidget(centralWidget_->widget(1));
  
   if(cv == nullptr) {
     centralWidget_->setCurrentIndex(0);
   }
   else {     
    centralWidget_->addWidget(cv);
    centralWidget_->setCurrentIndex(1);
   }  
}
void MainWindow::setMinMaxProgressBar(int min, int max)
{
  progressBar_->setProgressBarRange(min, max);  
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

  createTreeAttributeVisualitionMenus();  
}

void MainWindow::createTreeAttributeVisualitionMenus()
{
  attrVisMenu_ = menuBar()->addMenu(tr("&Attribute Visualisation"));

  areaAct_ = attrVisMenu_->addAction(tr("area"), this, &MainWindow::areaAct_onTriggered);
  areaAct_->setCheckable(true);
  areaAct_->setToolTip("Show colormap for attribute area.");
  
  perimeterAct_ = attrVisMenu_->addAction(tr("perimeter"), this, &MainWindow::perimeterAct_onTriggered);
  perimeterAct_->setCheckable(true);
  perimeterAct_->setToolTip("Show colormap for attribute perimeter.");
  
  volumeAct_ = attrVisMenu_->addAction(tr("volume"), this, &MainWindow::volumeAct_onTriggered);
  volumeAct_->setCheckable(true);
  volumeAct_->setToolTip("Show colormap for attribute volume.");
  
  circularityAct_ = attrVisMenu_->addAction(tr("circularity"), this, &MainWindow::circularityAct_onTriggered);
  circularityAct_->setCheckable(true);
  circularityAct_->setToolTip("Show colormap for attribute circularity");
  
  complexityAct_ = attrVisMenu_->addAction(tr("complexity"), this, &MainWindow::complexityAct_onTriggered);
  complexityAct_->setCheckable(true);
  complexityAct_->setToolTip("Show colormap for attribute complexity.");  

  nskelptAct_ = attrVisMenu_->addAction(tr("number of skeleton points"), this, 
    &MainWindow::nskelptAct_onTriggered);
  nskelptAct_->setCheckable(true);
  nskelptAct_->setToolTip("Show colormap for attribute number of skeleton points.");

  attrVisMenu_->setEnabled(false);  
}

void MainWindow::uncheckAttrVisActs()
{
  areaAct_->setChecked(false);
  perimeterAct_->setChecked(false);
  volumeAct_->setChecked(false);
  circularityAct_->setChecked(false);
  complexityAct_->setChecked(false);
  nskelptAct_->setChecked(false);
}

void MainWindow::createToolBar()
{
  using IcicleMorphotreeWidget::MorphoTreeType;

  QToolBar *toolbar = addToolBar(tr("Main"));
  toolbar->setIconSize(QSize{32, 32});

  // ============ MORPHOTREE NODE ========
  const QIcon showTreeIcon = QIcon(":/images/iciclemorphotree_icon.png");  
  showTreeVisAct_ = new QAction(showTreeIcon, tr("Show morphotree"), this);
  showTreeVisAct_->setStatusTip(tr("Show Morphotree Widget for the current image"));
  showTreeVisAct_->setCheckable(true);
  connect(showTreeVisAct_, &QAction::toggled, this, &MainWindow::treeVisAct_onToggled);
  toolbar->addAction(showTreeVisAct_);

  QLabel* lblMTreeType = new QLabel{tr("Tree Type: "), this};
  toolbar->addWidget(lblMTreeType);

  mtreeTypeComboBox_ = new QComboBox{this};
  mtreeTypeComboBox_->addItem(tr("Max-Tree"), 
    static_cast<int>(MorphoTreeType::MAX_TREE_8C));
  mtreeTypeComboBox_->addItem(tr("Min-Tree"), 
    static_cast<int>(MorphoTreeType::MIN_TREE_8C));
  toolbar->addWidget(mtreeTypeComboBox_);
  connect(mtreeTypeComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), 
    this,  &MainWindow::mtreeTypeComboBox_onIndexChanged);

  toolbar->addSeparator();
  const QIcon nodeSelectionClickIcon = QIcon(":/images/icicle_node_selection_pixel_icon.png");
  nodeSelectionClickAct_ = new QAction(nodeSelectionClickIcon, 
    tr("Node selection by pixel click"), this);
  nodeSelectionClickAct_->setStatusTip(tr("Toogle node selection by pixel click."));
  nodeSelectionClickAct_->setCheckable(true);
  connect(nodeSelectionClickAct_, &QAction::toggled, this, &MainWindow::nodeSelectionClickAct_onToggled);
  toolbar->addAction(nodeSelectionClickAct_);

  const QIcon nodeHighlightIcon = QIcon{":/images/highlight_node_icon.png"};
  nodeHighlightAct_ = new QAction{nodeHighlightIcon, tr("Highlight node"), this};
  nodeHighlightAct_->setStatusTip(tr("Highlight morphotree node in the input image"));
  nodeHighlightAct_->setCheckable(true);
  connect(nodeHighlightAct_, &QAction::triggered, this, &MainWindow::nodeHighlightAct_onTriggered);
  toolbar->addAction(nodeHighlightAct_);

  const QIcon syncTreeIcon = QIcon{":/images/sync_morphotree_icon.png"};
  syncTreeAct_ = new QAction{syncTreeIcon, tr("Sychronize image and morphological tree"), this};
  syncTreeAct_->setStatusTip(tr("synchronize image and morphological tree"));
  connect(syncTreeAct_, &QAction::triggered, this, &MainWindow::syncTreeAct_onTrigged);
  toolbar->addAction(syncTreeAct_);

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
  /*QToolBar *dmdToolBar = addToolBar("DMD Toolbar");
  dmdToolBar->setIconSize(QSize{ 32, 32});

  const QIcon dmdProcessIcon = QIcon{":/images/Skel_icon.png"};
  dmdProcessAct_ = new QAction{dmdProcessIcon, tr("DMD process"), this};
  dmdProcessAct_->setStatusTip("DMD process");
  connect(dmdProcessAct_, &QAction::triggered, this, &MainWindow::dmdProcessAct_onTrigged);
  dmdToolBar->addAction(dmdProcessAct_);
 */
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
  // const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
  //   ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
  const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
    ? QByteArrayList{QByteArray{"x-portable-graymap"}} : QImageWriter::supportedMimeTypes();
  
  if (acceptMode == QFileDialog::AcceptOpen) 
    mimeTypeFilters.append("image/x-portable-graymap");
  else {
    for (const QByteArray &mimeTypeName : QImageWriter::supportedMimeTypes()) {
      mimeTypeFilters.append(mimeTypeName);
    }
  }

  mimeTypeFilters.sort();
  dialog.setMimeTypeFilters(mimeTypeFilters);
  dialog.selectMimeTypeFilter("image/x-portable-graymap");
  dialog.setAcceptMode(acceptMode);
  // if (acceptMode == QFileDialog::AcceptSave)
  //   dialog.setDefaultSuffix("jpg");
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
      
      shouldUpdateProgressBar_ = true;
      attrVisMenu_->setEnabled(false);      
      uncheckAttrVisActs();
      if (showTreeVisAct_->isChecked()) {
        using IcicleMorphotreeWidget::MorphoTreeType;

        MorphoTreeType mt = static_cast<MorphoTreeType>(
            mtreeTypeComboBox_->currentData().value<int>());
            
        showProgressBar();
        attrVisMenu_->setEnabled(true);
        // TODO: include mtree type here.
        mainWidget_->updateTreeVisualiser(mt);
      }
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

void MainWindow::dmdProcessAct_onTrigged()
{
  QDockWidget *dockWidget_sdmd = mainWidget_->SdmdDockWidget();
  dockWidget_sdmd->setVisible(true); 
}

void MainWindow::treeVisAct_onToggled(bool checked)
{  
  using IcicleMorphotreeWidget::MorphoTreeType;

  if (shouldUpdateProgressBar_) {
    showProgressBar();
    attrVisMenu_->setEnabled(true);
  }

  MorphoTreeType mt =  static_cast<MorphoTreeType>(
    mtreeTypeComboBox_->currentData().value<int>());
  QDockWidget *dockMorphotreeWidget = mainWidget_->morphotreeDockWidget(mt);
  
  if (checked) {
    dockMorphotreeWidget->setVisible(true);    
  }
  else 
    dockMorphotreeWidget->setVisible(false);
}

void MainWindow::mtreeTypeComboBox_onIndexChanged(int index)
{
  using IcicleMorphotreeWidget::MorphoTreeType;

  uncheckAttrVisActs();
  mainWidget_->clearNodeHighlight();

  shouldUpdateProgressBar_ = true;
  if (showTreeVisAct_->isChecked()) {
    MorphoTreeType mt = static_cast<MorphoTreeType>(
      mtreeTypeComboBox_->currentData().value<int>());
    showProgressBar();
    attrVisMenu_->setEnabled(true);    
    mainWidget_->updateTreeVisualiser(mt);
  }  
}

void MainWindow::nodeSelectionClickAct_onToggled(bool checked)
{
  mainWidget_->setNodeSelectionByClickActivated(checked);
}

void MainWindow::syncTreeAct_onTrigged()
{
  shouldUpdateProgressBar_ = true;
  attrVisMenu_->setEnabled(false);
  mainWidget_->markTreeAsSynchronized();
  mainWidget_->setNeedTreeVisualiserUpdate(true);
  uncheckAttrVisActs();
  mainWidget_->clearNodeHighlight();
  
  if (showTreeVisAct_->isChecked()) {
    using IcicleMorphotreeWidget::MorphoTreeType;

    MorphoTreeType mtreeType = static_cast<MorphoTreeType>(
      mtreeTypeComboBox_->currentData().value<int>());

    showProgressBar();
    attrVisMenu_->setEnabled(true);
    mainWidget_->updateTreeVisualiser(mtreeType);    
  }
}

void MainWindow::imageZoomInAct_onTrigged()
{
  mainWidget_->zoomIn();
}

void MainWindow::imageZoomOutAct_onTrigged()
{
  mainWidget_->zoomOut();
}

void MainWindow::treeVis_onNodeSkeletonAssociation(int numberOfNodes)
{    
  progressBar_->setValue(numberOfNodes); 
}

void MainWindow::showProgressBar()
{
  statusBar()->clearMessage();
  progressBar_->show();  
}


void MainWindow::areaAct_onTriggered(bool checked)
{
  uncheckAttrVisActs();
  areaAct_->setChecked(checked);

  if (checked)
    mainWidget_->treeVisualiser()->showArea();
  else
    mainWidget_->treeVisualiser()->clearAttributes();
}

void MainWindow::perimeterAct_onTriggered(bool checked)
{
  uncheckAttrVisActs();
  perimeterAct_->setChecked(checked);

  if (checked)
    mainWidget_->treeVisualiser()->showPerimeter();
  else 
    mainWidget_->treeVisualiser()->clearAttributes();
}

void MainWindow::volumeAct_onTriggered(bool checked)
{
  uncheckAttrVisActs();
  volumeAct_->setChecked(checked);

  if (checked)
    mainWidget_->treeVisualiser()->showVolume();
  else
    mainWidget_->treeVisualiser()->clearAttributes();
}

void MainWindow::circularityAct_onTriggered(bool checked)
{
  uncheckAttrVisActs();
  circularityAct_->setChecked(checked);

  if (checked)
    mainWidget_->treeVisualiser()->showCircularity();
  else
    mainWidget_->treeVisualiser()->clearAttributes();
}

void MainWindow::complexityAct_onTriggered(bool checked)
{
  uncheckAttrVisActs();
  complexityAct_->setChecked(checked);

  if (checked)
    mainWidget_->treeVisualiser()->showComplexity();
  else
    mainWidget_->treeVisualiser()->clearAttributes();
}

void MainWindow::nskelptAct_onTriggered(bool checked)
{
  uncheckAttrVisActs();
  nskelptAct_->setChecked(checked);

  if (checked)
    mainWidget_->treeVisualiser()->showNumberOfSkeletonPoints();
  else
    mainWidget_->treeVisualiser()->clearAttributes();
}

void MainWindow::nodeHighlightAct_onTriggered(bool checked)
{
  mainWidget_->setHighlightNodeActivated(checked);
}