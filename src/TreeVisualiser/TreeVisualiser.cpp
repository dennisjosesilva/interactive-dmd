#include <MorphotreeWidget/Graphics/GNodeEventHandler.hpp>
#include <MainWidget.hpp>

#include "MainWidget.hpp"
#include "TreeVisualiser/TreeVisualiser.hpp"

#include <morphotree/tree/mtree.hpp>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QIcon>
#include <QDockWidget>
#include <QAction>
#include <QLabel>
#include <QSlider>
#include <QSpacerItem>
#include <QSizePolicy>

#include <QDebug>
#include <QMessageBox>
#include <QPushButton>

MyDockWidget::MyDockWidget(const QString &title, QWidget *mainwindow)
  :QDockWidget{title, mainwindow}  
{}

void MyDockWidget::closeEvent(QCloseEvent *e)
{
  emit closed(this);
}

QSize MyDockWidget::sizeHint() const 
{
  if (widget())
    return widget()->sizeHint();
  else 
    return QDockWidget::sizeHint();
}

TreeVisualiser::TreeVisualiser(MainWidget *mainWidget)
  : mainWidget_{mainWidget},
    curNodeSelection_{nullptr},
    binRecDock_{nullptr},
    greyRecDock_{nullptr},
    skelRecDock_{nullptr},
    removeSkelDock_{nullptr},
    curColorBar_{nullptr}
{
  namespace mw = MorphotreeWidget;
   
  QLayout *mainLayout = new QVBoxLayout;
  QLayout *btnLayout = createButtons();
  QLayout *treeSimplificationLayout = createTreeSimplificationControls();

  mainLayout->addItem(btnLayout);
  mainLayout->addItem(treeSimplificationLayout);
  
  treeWidget_ = new mw::MorphotreeWidget{mw::TreeLayout::TreeLayoutType::GraphvizWithLevel};
  mainLayout->addWidget(treeWidget_);

  connect(mw::GNodeEventHandler::Singleton(), &mw::GNodeEventHandler::mousePress,
    this, &TreeVisualiser::nodeMousePress);

  treeSimplification_ = 
    std::make_shared<mw::TreeSimplificationProgressiveAreaDifferenceFilter>(6, 50, 180);

  setLayout(mainLayout);
}

QLayout *TreeVisualiser::createButtons()
{
  QLayout *btnLayout = new QHBoxLayout;

  QPushButton *binRecBtn = new QPushButton{ QIcon{":/images/binrec_icon.png"}, 
    tr(""), this};
  binRecBtn->setIconSize(QSize{32, 32});
  connect(binRecBtn, &QPushButton::clicked, this, &TreeVisualiser::binRecBtn_press);

  QPushButton *binRecPlusBtn = new QPushButton { QIcon{":/images/binrec_plus_icon.png"},
    tr(""), this};  
  binRecPlusBtn->setIconSize(QSize{32, 32});
  connect(binRecPlusBtn, &QPushButton::clicked, this, &TreeVisualiser::binRecPlusBtn_press);

  QPushButton *greyRecBtn = new QPushButton{ QIcon{":/images/greyrec_icon.png"}, 
    tr(""), this};
  greyRecBtn->setIconSize(QSize{32, 32});
  connect(greyRecBtn, &QPushButton::clicked, this, &TreeVisualiser::greyRecBtn_press);

  QPushButton *greyRecPlusBtn = new QPushButton{ QIcon{":/images/greyrec_plus_icon.png"},
    "", this};
  greyRecPlusBtn->setIconSize(QSize{32, 32});
  connect(greyRecPlusBtn, &QPushButton::clicked, this, &TreeVisualiser::greyRecPlusBtn_press);
  
  QPushButton *inspectNodePlusBtn = new QPushButton{ QIcon{":/images/inspect_node_plus.png"},
    "", this};
  inspectNodePlusBtn->setIconSize(QSize{32, 32});
  connect(inspectNodePlusBtn, &QPushButton::clicked, this, &TreeVisualiser::inspectNodePlusBtn_press);

  QPushButton *inspectNodeMinusBtn = new QPushButton{ QIcon{":/images/inspect_node_minus.png"},
    "", this};
  inspectNodeMinusBtn->setIconSize(QSize{32, 32});
  connect(inspectNodeMinusBtn, &QPushButton::clicked, this, &TreeVisualiser::inspectNodeMinusBtn_press);

  QPushButton *skelRecBtn = new QPushButton{ QIcon{":/images/Skel_icon.png"}, "", this};
  skelRecBtn->setIconSize(QSize{32, 32});
  connect(skelRecBtn, &QPushButton::clicked, this, &TreeVisualiser::skelRecBtn_press);
  
  QPushButton *removeSkelBtn = new QPushButton { QIcon{":/images/Remove_Skel_icon.png"}, "", this};
  removeSkelBtn->setIconSize(QSize{32, 32});
  connect(removeSkelBtn, &QPushButton::clicked, this, &TreeVisualiser::removeSkelBtn_press);


  btnLayout->addWidget(binRecBtn);
  btnLayout->addWidget(binRecPlusBtn);
  btnLayout->addWidget(greyRecBtn);
  btnLayout->addWidget(greyRecPlusBtn);
  btnLayout->addWidget(inspectNodePlusBtn);
  btnLayout->addWidget(inspectNodeMinusBtn);
  btnLayout->addWidget(skelRecBtn);
  btnLayout->addWidget(removeSkelBtn);
  

  return btnLayout;
}

QLayout *TreeVisualiser::createTreeSimplificationControls()
{  
  QLayout *controlsLayout = new QVBoxLayout;
  QHBoxLayout *nleavesLayout = new QHBoxLayout;  
  QHBoxLayout *areaLayout = new QHBoxLayout;
  QHBoxLayout *areaDiffLayout = new QHBoxLayout;

  // -------------------- number of leaves (extinction filter) ------------------------
  nleavesLayout->addWidget(new QLabel{"# leaves: ", this});
  numberLeavesSlider_ = new QSlider{Qt::Horizontal, this};
  numberLeavesSlider_->setRange(1, 50);
  numberLeavesSlider_->setValue(6);
  numberLeavesValueLabel_ = new QLabel("6", this);
  numberLeavesValueLabel_->setFixedWidth(25);
  numberLeavesValueLabel_->setAlignment(Qt::AlignRight);
  connect(numberLeavesSlider_, &QSlider::sliderMoved, this,
    &TreeVisualiser::numberLeavesSlider_onValueChange);
  
  nleavesLayout->addWidget(numberLeavesSlider_);  
  nleavesLayout->addWidget(numberLeavesValueLabel_);

  controlsLayout->addItem(nleavesLayout);

  // ------------------------- area filter ------------------------------------------------
  areaLayout->addWidget(new QLabel{"area:         ", this});
  areaSlider_ = new QSlider{Qt::Horizontal, this};
  areaSlider_->setRange(0, 1000);
  areaSlider_->setValue(50);
  areaValueLabel_ = new QLabel{"50", this};
  areaValueLabel_->setFixedWidth(35);
  areaValueLabel_->setAlignment(Qt::AlignRight);
  connect(areaSlider_, &QSlider::sliderMoved, this, 
    &TreeVisualiser::areaSlider_onValueChange);

  areaLayout->addWidget(areaSlider_);
  areaLayout->addWidget(areaValueLabel_);

  controlsLayout->addItem(areaLayout);

  // --------------- progressive area diff filter -----------------------------------------
  areaDiffLayout->addWidget(new QLabel{"area diff: ", this});
  areaDiffSlider_ = new QSlider{Qt::Horizontal, this};
  areaDiffSlider_->setRange(0, 1000);
  areaDiffSlider_->setValue(180);
  areaDiffValueLabel_ = new QLabel{"180", this};
  areaDiffValueLabel_->setFixedWidth(35);
  areaDiffValueLabel_->setAlignment(Qt::AlignRight);
  connect(areaDiffSlider_, &QSlider::sliderMoved, this,
    &TreeVisualiser::areaDiffSlider_onValueChange);

  areaDiffLayout->addWidget(areaDiffSlider_);
  areaDiffLayout->addWidget(areaDiffValueLabel_);

  controlsLayout->addItem(areaDiffLayout);

  return controlsLayout;
}

void TreeVisualiser::loadImage(Box domain, const std::vector<uint8> &f)
{  
  namespace mw = MorphotreeWidget;

  if (treeWidget_->hasAttributes()) 
    clearAttributes();

  curNodeSelection_ = nullptr;
  if (treeWidget_->treeSimplification() == nullptr) 
    treeWidget_->loadImage(domain, f, 
      std::make_shared<mw::TreeSimplificationProgressiveAreaDifferenceFilter>(6, 50, 180));
  else 
    treeWidget_->loadImage(domain, f, treeWidget_->treeSimplification());
  
  
  domain_ = domain;
  dmd_.setProcessedImage(greyImageToField(f));  
  dmdrecon_ = new dmdReconstruct();
}

std::vector<morphotree::uint8> TreeVisualiser::bool2UInt8(
  const std::vector<bool> &binimg) const
{
  std::vector<uint8> f(binimg.size());
  for (uint32 p = 0; p < f.size(); ++p) {
    f[p] = binimg[p] ? 0 : 255;
  }

  return f;
}

void TreeVisualiser::reconstructBinaryImage(SimpleImageViewer *iv, NodePtr node)
{  
  std::vector<uint8> bimg = bool2UInt8(node->reconstruct(domain_));  

  QImage img{bimg.data(), static_cast<int>(domain_.width()), static_cast<int>(domain_.height()),
      QImage::Format::Format_Grayscale8};
    
  iv->setImage(img);
}

void TreeVisualiser::reconstructGreyImage(SimpleImageViewer *iv, NodePtr node)
{
  std::vector<uint8> fimg = node->reconstructGrey(domain_);
  QImage img{fimg.data(), static_cast<int>(domain_.width()), static_cast<int>(domain_.height()),
      QImage::Format::Format_Grayscale8};
  
  iv->setImage(img);
}

std::shared_ptr<MorphotreeWidget::TreeSimplification> TreeVisualiser::duplicateTreeSimplification()
{
  namespace mw = MorphotreeWidget;
  using TSType = mw::TreeSimplificationProgressiveAreaDifferenceFilter;

  std::shared_ptr<TSType> ts = std::dynamic_pointer_cast<TSType>(
    treeWidget_->treeSimplification());

  return std::make_shared<TSType>(ts->numberOfLeavesToKeep(), 
    ts->areaThresholdToKeep(), ts->progDiffThresholdToKeep());
}

FIELD<float> *TreeVisualiser::binImageToField(const std::vector<bool> &bimg) const
{
  FIELD<float> *fimg = new FIELD<float>{
    static_cast<int>(domain_.width()), static_cast<int>(domain_.height()) };

  for (int y = domain_.top(); y <= domain_.bottom(); y++) {
    for (int x = domain_.left(); x <= domain_.right(); x++) {
      if (bimg[ domain_.pointToIndex(x, y)]) 
        fimg->set(x, y, 0.f);   // foreground pixel
      else 
        fimg->set(x, y, 1.f);   // background pixel
    }
  }

  return fimg;
}

FIELD<float> *TreeVisualiser::greyImageToField(const std::vector<uint8> &img) const 
{
  FIELD<float> *fimg = new FIELD<float>{ 
    static_cast<int>(domain_.width()), static_cast<int>(domain_.height()) };

  for (int y = domain_.top(); y <= domain_.bottom(); y++) {
    for (int x = domain_.left(); x <= domain_.right(); x++) {
      fimg->set(x, y, static_cast<float>(img[domain_.pointToIndex(x, y)]));
    }
  }
  
  return fimg;
}

QImage TreeVisualiser::fieldToQImage(FIELD<float> *fimg) const
{
  QImage img{fimg->dimX(), fimg->dimY(), QImage::Format_Grayscale8};
  float *fimg_data = fimg->data();
  uchar *img_data = img.bits();

  int N = fimg->dimX() * fimg->dimY();
  for (int i = 0; i < N; ++i)
    img_data[i] = static_cast<uchar>(fimg_data[i]);
 
  return img;
}

void TreeVisualiser::registerDMDSkeletons()
{
  const MTree &tree = treeWidget_->tree();
  
  dmd_.Init_indexingSkeletons();
  NumberOfSkeletonPointCache nskelCache;
  
  FIELD<float> *fnode = nullptr;
  uint32 nodeSequence = 0;
  
  nskelCache.openFile();
  tree.tranverse([&fnode, &nodeSequence, &nskelCache, this](NodePtr node) {
    fnode = binImageToField(node->reconstruct(domain_));    
    int nskelPt = dmd_.indexingSkeletons(fnode, node->level(), node->id());
    nskelCache.store(node->id(), nskelPt);

    emit associateNodeToSkeleton(nodeSequence);
    nodeSequence++;
        
    delete fnode;
  });
  nskelCache.closeFile();
  dmdrecon_->readIndexingControlPoints(domain_.width(), domain_.height(), 
    dmd_.clear_color, dmd_.getInty_Node()); // pre-upload
}

void TreeVisualiser::showArea()
{
  NormalisedAttributeMeta areaInfo = attrCompueter_.computeArea(domain_, treeWidget_->tree());

  treeWidget_->loadAttributes(std::move(areaInfo.nattr_));
  
  if (curColorBar_ == nullptr) {
    ColorBar *colorBar = treeWidget_->createHColorBar(this);
    curColorBar_ = new TitleColorBar{colorBar, this};
  }
  
  curColorBar_->setMaxValue(areaInfo.maxValue);
  curColorBar_->setMinValue(areaInfo.minValue);
  curColorBar_->setShowNumbers(true);
  curColorBar_->setTitle("Area");
  curColorBar_->update();
  layout()->addWidget(curColorBar_);
}

void TreeVisualiser::showPerimeter()
{
  NormalisedAttributeMeta perimeterInfo = attrCompueter_.computePerimeter(
    domain_, treeWidget_->tree());

  treeWidget_->loadAttributes(std::move(perimeterInfo.nattr_));

  if (curColorBar_ == nullptr) {
    ColorBar *colorBar = treeWidget_->createHColorBar(this);
    curColorBar_ = new TitleColorBar{colorBar, this};
  }

  curColorBar_->setMaxValue(perimeterInfo.maxValue);
  curColorBar_->setMinValue(perimeterInfo.minValue);
  curColorBar_->setShowNumbers(true);
  curColorBar_->setTitle("Perimeter");
  curColorBar_->update();
  layout()->addWidget(curColorBar_);
}

void TreeVisualiser::showVolume()
{
  NormalisedAttributeMeta volumeInfo = attrCompueter_.computeVolume(
    domain_, treeWidget_->tree());

  treeWidget_->loadAttributes(std::move(volumeInfo.nattr_));

  if (curColorBar_ == nullptr) {
    ColorBar *colorBar = treeWidget_->createHColorBar(this);
    curColorBar_ = new TitleColorBar{colorBar, this};
  }

  curColorBar_->setMaxValue(volumeInfo.maxValue);
  curColorBar_->setMinValue(volumeInfo.minValue);
  curColorBar_->setShowNumbers(true);
  curColorBar_->setTitle("Volume");
  curColorBar_->update();
  layout()->addWidget(curColorBar_);
}

void TreeVisualiser::showCircularity()
{
  NormalisedAttributeMeta circularityInfo = attrCompueter_.computeComplexity(
    domain_, treeWidget_->tree());

  treeWidget_->loadAttributes(std::move(circularityInfo.nattr_));

  if (curColorBar_ == nullptr) {
    ColorBar *colorBar = treeWidget_->createHColorBar(this);
    curColorBar_ = new TitleColorBar{colorBar, this};
  }

  curColorBar_->setMaxValue(circularityInfo.maxValue);
  curColorBar_->setMinValue(circularityInfo.minValue);
  curColorBar_->setShowNumbers(true);
  curColorBar_->setTitle("Circularity");
  curColorBar_->update();
  layout()->addWidget(curColorBar_);
}

void TreeVisualiser::showComplexity()
{
  NormalisedAttributeMeta complexityInfo = attrCompueter_.computeComplexity(
    domain_, treeWidget_->tree());

  treeWidget_->loadAttributes(std::move(complexityInfo.nattr_));

  if (curColorBar_ == nullptr) {
    ColorBar *colorBar = treeWidget_->createHColorBar(this);
    curColorBar_ = new TitleColorBar{colorBar, this};
  }

  curColorBar_->setMaxValue(complexityInfo.maxValue);
  curColorBar_->setMinValue(complexityInfo.minValue);
  curColorBar_->setShowNumbers(true);
  curColorBar_->setTitle("Complexity");
  curColorBar_->update();
  layout()->addWidget(curColorBar_);
}

void TreeVisualiser::showNumberOfSkeletonPoints()
{
  NormalisedAttributeMeta nskelPtInfo = 
    attrCompueter_.compueteNumberOfSkeletonPoints(treeWidget_->tree());
  
  treeWidget_->loadAttributes(std::move(nskelPtInfo.nattr_));

  if (curColorBar_ == nullptr) {
    ColorBar *colorBar = treeWidget_->createHColorBar(this);
    curColorBar_ = new TitleColorBar{colorBar, this};
  }

  curColorBar_->setMaxValue(nskelPtInfo.maxValue);
  curColorBar_->setMinValue(nskelPtInfo.minValue);
  curColorBar_->setShowNumbers(true);
  curColorBar_->setTitle("Number of skeleton points");
  curColorBar_->update();
  layout()->addWidget(curColorBar_);
}

void TreeVisualiser::clearAttributes()
{
  layout()->removeWidget(curColorBar_);
  curColorBar_->deleteLater();
  curColorBar_ = nullptr;
  treeWidget_->clearAttributes();
  update();
}

void TreeVisualiser::binRecBtn_press()
{  
  if (curNodeSelection_ != nullptr) {
    SimpleImageViewer *iv = nullptr;
    if (binRecDock_ == nullptr) {
      iv = new SimpleImageViewer;
      binRecDock_ = mainWidget_->createDockWidget(
        tr("binary node reconstruction"), iv);
      binRecDock_->setGNode(curNodeSelection_);
      connect(binRecDock_, &MyDockWidget::closed, this, &TreeVisualiser::binRecDock_onClose);
      binRecDock_->resize(domain_.width()+22, domain_.height()+84);    
    }
    else {
      iv = qobject_cast<SimpleImageViewer *>(binRecDock_->widget());
    }
    
    reconstructBinaryImage(iv, curNodeSelection_->mtreeNode());    
  }
}

void TreeVisualiser::binRecPlusBtn_press()
{
  SimpleImageViewer *iv = new SimpleImageViewer;
  MyDockWidget *dock = mainWidget_->createDockWidget(
    tr("binary node reconstruction"), iv);
  dock->setGNode(curNodeSelection_);

  dock->resize(domain_.width() + 22, domain_.height() + 84);
  reconstructBinaryImage(iv, curNodeSelection_->mtreeNode());  
}

void TreeVisualiser::greyRecBtn_press()
{  
  if (curNodeSelection_ != nullptr) {
    SimpleImageViewer *iv = nullptr;
    if (greyRecDock_ == nullptr) {
      iv = new SimpleImageViewer;
      greyRecDock_ = mainWidget_->createDockWidget(
        tr("grey node reconstruction"), iv);      
      greyRecDock_->setGNode(curNodeSelection_);
      connect(greyRecDock_, &MyDockWidget::closed, this, &TreeVisualiser::greyRecDock_onClose);
      greyRecDock_->resize(domain_.width() + 22, domain_.height() + 84);
    }
    else {
      iv = qobject_cast<SimpleImageViewer *>(greyRecDock_->widget());
    }
    
    reconstructGreyImage(iv, curNodeSelection_->mtreeNode());    
  }
}

void TreeVisualiser::greyRecPlusBtn_press()
{
  SimpleImageViewer *iv = new SimpleImageViewer;
  MyDockWidget *dock = mainWidget_->createDockWidget(
    tr("greyscale node reconstruction"), iv);
  dock->setGNode(curNodeSelection_);

  dock->resize(domain_.width() + 22, domain_.height() + 84);
  reconstructGreyImage(iv, curNodeSelection_->mtreeNode());  
}

void TreeVisualiser::skelRecBtn_press()
{
  if (curNodeSelection_ != nullptr) {
    //CpViewer *cv = nullptr;
    if (skelRecDock_ == nullptr) {
      
      cv = new CpViewer(static_cast<int>(domain_.width()), static_cast<int>(domain_.height()));
      
      skelRecDock_ = mainWidget_->createDockWidget(tr("Reconstruction of the selected node"), cv);
      
      //skelRecDock_->setGNode(curNodeSelection_);
      connect(skelRecDock_, &MyDockWidget::closed, this, &TreeVisualiser::skelRecDock_onClose);
      skelRecDock_->resize(domain_.width() + 22, domain_.height() + 84);
    }
    else {
      cv = qobject_cast<CpViewer *>(skelRecDock_->widget());
      cv->Update();
    }
 
    NodePtr mnode = curNodeSelection_->mtreeNode();
    dmdrecon_->ReconstructIndexingImage(false, mnode->id(), 1);
  
   if(mnode->id() == 0){
     QMessageBox::information(0, "For your information",
        "Node_0 corresponds to the background layer, if you \n"
        "want to manipulate CPs, please select other nodes.");
   }
   else{
     vector<vector<Vector3<float>>> storeCPs = dmdrecon_->GetCPs(mnode->id());
     cv->transData(mnode->level(), storeCPs, dmdrecon_);
   }
    
  }
  else{
    QMessageBox::information(0, "For your information",
        "Please select a node.");
  }
}

void TreeVisualiser::removeSkelBtn_press()
{
  if (curNodeSelection_ != nullptr) {
    SimpleImageViewer *iv = nullptr;
    if (removeSkelDock_ == nullptr) {
      iv = new SimpleImageViewer;
      removeSkelDock_ = mainWidget_->createDockWidget(tr("DMD remove skeleton reconstruction"), iv);
      removeSkelDock_->setGNode(curNodeSelection_);
      connect(removeSkelDock_, &MyDockWidget::closed, this, &TreeVisualiser::removeSkelDock_onClose);
      removeSkelDock_->resize(domain_.width() + 22, domain_.height() + 84);
    }
    else {
      iv = qobject_cast<SimpleImageViewer *>(removeSkelDock_->widget());
    }

    NodePtr mnode = curNodeSelection_->mtreeNode();
    dmdrecon_->ReconstructIndexingImage(false, mnode->id(), 0);
    QImage img = fieldToQImage(dmdrecon_->getOutput());    
    iv->setImage(img);
  }
}

void TreeVisualiser::nodeMousePress(GNode *node, 
  QGraphicsSceneMouseEvent *e)
{
  if (curNodeSelection_ != nullptr) {
    curNodeSelection_ ->setSelected(false);
    curNodeSelection_->update();
  }
    
  if (node->isSelected()) {
    node->setSelected(false);
    curNodeSelection_ = nullptr;
  }
  else {
    node->setSelected(true);
    curNodeSelection_ = node;
  }

  node->update();
}

void TreeVisualiser::inspectNodePlusBtn_press()
{
  if (curNodeSelection_ != nullptr) {
    NodePtr node = curNodeSelection_->simplifiedMTreeNode();
    treeWidget_->inspectNode(node->id(), duplicateTreeSimplification());
    curNodeSelection_ = nullptr;
  }
}

void TreeVisualiser::inspectNodeMinusBtn_press()
{
  namespace mw = MorphotreeWidget;
  using TSType = mw::TreeSimplificationProgressiveAreaDifferenceFilter;
  using TSTypePtr = std::shared_ptr<TSType>;

  if (treeWidget_->numberOfUndoNodeInspection() > 0) {
    treeWidget_->undoNodeInspection();
    curNodeSelection_ = nullptr;

    TSTypePtr ts = std::dynamic_pointer_cast<TSType>(treeWidget_->treeSimplification());

    numberLeavesSlider_->setValue(ts->numberOfLeavesToKeep());
    areaSlider_->setValue(ts->areaThresholdToKeep());
    areaDiffSlider_->setValue(ts->progDiffThresholdToKeep());

    numberLeavesValueLabel_->setText(QString::number(numberLeavesSlider_->value()));
    areaValueLabel_->setText(QString::number(areaSlider_->value()));
    areaDiffValueLabel_->setText(QString::number(areaDiffSlider_->value()));
  }
}

void TreeVisualiser::numberLeavesSlider_onValueChange(int val)
{  
  namespace mw = MorphotreeWidget;
  using AreaProgSimplification = mw::TreeSimplificationProgressiveAreaDifferenceFilter;

  numberLeavesValueLabel_->setText(QString::number(val));

  std::dynamic_pointer_cast<AreaProgSimplification>(treeWidget_->treeSimplification())
    ->numberOfLeavesToKeep(val);

  curNodeSelection_ = nullptr;
  treeWidget_->redrawTree();
}

void TreeVisualiser::areaSlider_onValueChange(int val)
{
  namespace mw = MorphotreeWidget;
  using AreaProgSimplification = mw::TreeSimplificationProgressiveAreaDifferenceFilter;

  areaValueLabel_->setText(QString::number(val));
  
  std::dynamic_pointer_cast<AreaProgSimplification>(treeWidget_->treeSimplification())
    ->areaThresholdToKeep(val);

  curNodeSelection_ = nullptr;
  treeWidget_->redrawTree();
}

void TreeVisualiser::areaDiffSlider_onValueChange(int val)
{
  namespace mw = MorphotreeWidget;
  using AreaProgSimplification = mw::TreeSimplificationProgressiveAreaDifferenceFilter;

  areaDiffValueLabel_->setText(QString::number(val));

  std::dynamic_pointer_cast<AreaProgSimplification>(treeWidget_->treeSimplification())
    ->progDifferenceThreholdToKeep(val);

  curNodeSelection_ = nullptr;
  treeWidget_->redrawTree();
}

void TreeVisualiser::binRecDock_onClose(MyDockWidget *dock)
{
  binRecDock_ = nullptr;  
}

void TreeVisualiser::greyRecDock_onClose(MyDockWidget *dock)
{
  greyRecDock_ = nullptr;
}

void TreeVisualiser::skelRecDock_onClose(MyDockWidget *dock)
{
  skelRecDock_ = nullptr;
}

void TreeVisualiser::removeSkelDock_onClose(MyDockWidget *dock)
{
  removeSkelDock_ = nullptr;
}

void TreeVisualiser::selectNodeByPixel(int x, int y)
{
  GNode *node = treeWidget_->gnode(x, y);

  if (curNodeSelection_ != node) {
    if (curNodeSelection_ != nullptr) {
      curNodeSelection_->setSelected(false);
      curNodeSelection_->update();
    }

    node->setSelected(true);
    node->update();
    curNodeSelection_ = node;
    treeWidget_->centerOn(node);
  }
}