// #include <MorphotreeWidget/Graphics/GNodeEventHandler.hpp>
#include <IcicleMorphotreeWidget/Graphics/Node/GNodeEventHandler.hpp>
#include <IcicleMorphotreeWidget/Graphics/Node/GNodeFactory.hpp>
#include <MainWidget.hpp>

#include "MainWidget.hpp"
#include "TreeVisualiser/TreeVisualiser.hpp"

#include <morphotree/tree/mtree.hpp>
#include <algorithm>

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

#include "CustomWidgets/CollapsableWidget.hpp"
#include "TreeVisualiser/TreeVisualiserStylePanel.hpp"

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
    SplineManipDock_{nullptr},
    skelRecDock_{nullptr},
    removeSkelDock_{nullptr},
    curColorBar_{nullptr},
    maxValue_{0}
{  
  using GNodeEventHandler = IcicleMorphotreeWidget::GNodeEventHandler;
  using FixedHeightTreeLayout = IcicleMorphotreeWidget::FixedHeightTreeLayout;    
  using GrayscaleBasedHeightTreeLayout = IcicleMorphotreeWidget::GrayscaleBasedHeightTreeLayout;
  using GradientGNodeFactory = IcicleMorphotreeWidget::GradientGNodeFactory;                                                       
  using IcicleMorphotreeWidget = IcicleMorphotreeWidget::IcicleMorphotreeWidget;

  QLayout *mainLayout = new QVBoxLayout;
  QLayout *controlsLayout = new QHBoxLayout;
  QLayout *btnLayout = createButtons();

  mainLayout->addItem(btnLayout);  
  
  treeWidget_ = new IcicleMorphotreeWidget{this, 
    std::make_unique<GrayscaleBasedHeightTreeLayout>(
      std::make_unique<GradientGNodeFactory>(),
      20.f, 20.f, 10.f)};

  controlsLayout->addWidget(treeWidget_);  

  connect(GNodeEventHandler::Singleton(), &GNodeEventHandler::mousePress, 
    this, &TreeVisualiser::nodeMousePress);
  
  TreeVisualiserStylePanel *tl = new TreeVisualiserStylePanel{this, nullptr};  
  CollapsableWidget *cw = new CollapsableWidget{"Style", tl, this};  
  controlsLayout->addWidget(cw);    

  mainLayout->addItem(controlsLayout);
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
  
  QPushButton *SplineManipulateBtn = new QPushButton{ QIcon{":/images/Spline_CPs_icon.png"}, "", this};
  SplineManipulateBtn->setIconSize(QSize{32, 32});
  connect(SplineManipulateBtn, &QPushButton::clicked, this, &TreeVisualiser::SplineManipulateBtn_press);
  
  QPushButton *skelRecBtn = new QPushButton{ QIcon{":/images/Skel_icon.png"}, "", this};
  skelRecBtn->setIconSize(QSize{32, 32});
  connect(skelRecBtn, &QPushButton::clicked, this, &TreeVisualiser::skelRecBtn_press);
  
  QPushButton *removeSkelBtn = new QPushButton{ QIcon{":/images/Remove_Skel_icon.png"}, "", this};
  removeSkelBtn->setIconSize(QSize{32, 32});
  connect(removeSkelBtn, &QPushButton::clicked, this, &TreeVisualiser::removeSkelBtn_press);

  QPushButton *remNodeToReconBtn = new QPushButton { QIcon{":/images/icicle_node_removal_icon.png"},
    "", this};
  remNodeToReconBtn->setIconSize(QSize{32, 32});
  connect(remNodeToReconBtn, &QPushButton::clicked, this, &TreeVisualiser::remNodeReconBtn_press);

  QPushButton *incNodeToReconBtn = new QPushButton{ QIcon{":/images/icicle_node_inclusion_icon.png"}, 
    "", this};
  incNodeToReconBtn->setIconSize(QSize{32, 32});
  connect(incNodeToReconBtn, &QPushButton::clicked, this, &TreeVisualiser::incNodeReconBtn_press);

  btnLayout->addWidget(binRecBtn);
  btnLayout->addWidget(binRecPlusBtn);
  btnLayout->addWidget(greyRecBtn);
  btnLayout->addWidget(greyRecPlusBtn); 
  btnLayout->addWidget(SplineManipulateBtn); 
  btnLayout->addWidget(skelRecBtn);
  btnLayout->addWidget(removeSkelBtn);
  btnLayout->addWidget(remNodeToReconBtn);
  btnLayout->addWidget(incNodeToReconBtn);
  
  return btnLayout;
}

void TreeVisualiser::updateTransparencyOfTheNodes()
{
  for (unsigned int i = 0; i < selectedNodesForRec_.size(); i++) {
    if (!selectedNodesForRec_[i])
      treeWidget_->gnodes()[i]->setOpacity(0.35f);
  }
}

void TreeVisualiser::useGradientGNodeStyle()
{  
  using GradientNodeGNodeFactory = IcicleMorphotreeWidget::GradientGNodeFactory; 
  curNodeSelection_ = nullptr;
  float unitHeight = unitHeightNode();
  treeWidget_->removeGrayScaleBar();
  treeWidget_->setGNodeFactory(std::make_unique<GradientNodeGNodeFactory>());
  treeWidget_->addGrayScaleBar(maxValue_+1, 10.f, unitHeight);  
  treeWidget_->updateTreeRendering();

  updateTransparencyOfTheNodes();
}

void TreeVisualiser::useFixedColorGNodeStyle()
{
  using FixedColorGNodeFactory = IcicleMorphotreeWidget::FixedColorGNodeFactory;
  curNodeSelection_ = nullptr;
  float unitHeight = unitHeightNode();
  treeWidget_->removeGrayScaleBar();
  treeWidget_->setGNodeFactory(std::make_unique<FixedColorGNodeFactory>());
  treeWidget_->addGrayScaleBar(maxValue_+1, 10.f, unitHeight);
  treeWidget_->updateTreeRendering();   

  updateTransparencyOfTheNodes();
}

float TreeVisualiser::unitHeightNode() const
{
  using GrayScaleBasedHeightTreeLayout = IcicleMorphotreeWidget::GrayscaleBasedHeightTreeLayout;
  using GrayScaleBasedHeightTreeLayoutPtr = std::shared_ptr<GrayScaleBasedHeightTreeLayout>;

  GrayScaleBasedHeightTreeLayoutPtr treeLayout = 
    std::dynamic_pointer_cast<GrayScaleBasedHeightTreeLayout>(treeWidget_->treeLayout());
  
  return treeLayout->uniHeight();
}

void TreeVisualiser::setUnitHeightNode(float val)
{
  using GrayScaleBasedHeightTreeLayout = IcicleMorphotreeWidget::GrayscaleBasedHeightTreeLayout;
  using GrayScaleBasedHeightTreeLayoutPtr = std::shared_ptr<GrayScaleBasedHeightTreeLayout>;

  curNodeSelection_ = nullptr;
  GrayScaleBasedHeightTreeLayoutPtr treeLayout = 
    std::dynamic_pointer_cast<GrayScaleBasedHeightTreeLayout>(treeWidget_->treeLayout());

  treeLayout->setUniHeight(val);

  if (treeWidget_->grayscaleBar() != nullptr) 
    treeWidget_->grayscaleBar()->setUnitHeight(val);

  treeWidget_->updateTreeRendering();
  updateTransparencyOfTheNodes();
}

void TreeVisualiser::loadImage(Box domain, const std::vector<uint8> &f)
{  
  // namespace mw = MorphotreeWidget;

  if (treeWidget_->hasAttributes()) 
    clearAttributes();

  maxValue_ = static_cast<uint32>(*std::max_element(f.begin(), f.end()));
  curNodeSelection_ = nullptr;
  // if (treeWidget_->treeSimplification() == nullptr) 
  //   treeWidget_->loadImage(domain, f, 
  //     std::make_shared<mw::TreeSimplificationProgressiveAreaDifferenceFilter>(6, 50, 180));
  // else 
  //   treeWidget_->loadImage(domain, f, treeWidget_->treeSimplification());
  treeWidget_->loadImage(domain, f);

  treeWidget_->removeGrayScaleBar();
    // TODO: Make it dynamic  
  treeWidget_->addGrayScaleBar(maxValue_+1, 10.f, 10.f);
  selectedNodesForRec_.clear();
  selectedNodesForRec_.resize(treeWidget_->mtree().numberOfNodes());
  selectedNodesForRec_.fill(true);

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

// std::shared_ptr<MorphotreeWidget::TreeSimplification> TreeVisualiser::duplicateTreeSimplification()
// {
//   namespace mw = MorphotreeWidget;
//   using TSType = mw::TreeSimplificationProgressiveAreaDifferenceFilter;

//   std::shared_ptr<TSType> ts = std::dynamic_pointer_cast<TSType>(
//     treeWidget_->treeSimplification());

//   return std::make_shared<TSType>(ts->numberOfLeavesToKeep(), 
//     ts->areaThresholdToKeep(), ts->progDiffThresholdToKeep());
// }

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
  // const MTree &tree = treeWidget_->tree();
  const MTree &tree = treeWidget_->mtree();
  
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
  // NormalisedAttributeMeta areaInfo = attrCompueter_.computeArea(domain_, treeWidget_->tree());
  NormalisedAttributeMeta areaInfo = attrCompueter_.computeArea(domain_,
    treeWidget_->mtree());

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
  // NormalisedAttributeMeta perimeterInfo = attrCompueter_.computePerimeter(
  //   domain_, treeWidget_->tree());
  NormalisedAttributeMeta perimeterInfo = attrCompueter_.computePerimeter(
    domain_, treeWidget_->mtree());

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
  // NormalisedAttributeMeta volumeInfo = attrCompueter_.computeVolume(
  //   domain_, treeWidget_->tree());
  NormalisedAttributeMeta volumeInfo = attrCompueter_.computeVolume(
    domain_, treeWidget_->mtree());

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
  // NormalisedAttributeMeta circularityInfo = attrCompueter_.computeComplexity(
  //   domain_, treeWidget_->tree());
  NormalisedAttributeMeta circularityInfo = attrCompueter_.computeComplexity(
    domain_, treeWidget_->mtree());

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
  // NormalisedAttributeMeta complexityInfo = attrCompueter_.computeComplexity(
  //   domain_, treeWidget_->tree());
  NormalisedAttributeMeta complexityInfo = attrCompueter_.computeCircularity(
      domain_, treeWidget_->mtree());

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
  // NormalisedAttributeMeta nskelPtInfo = 
  //   attrCompueter_.compueteNumberOfSkeletonPoints(treeWidget_->tree());
  NormalisedAttributeMeta nskelPtInfo =
    attrCompueter_.compueteNumberOfSkeletonPoints(treeWidget_->mtree());
  
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

std::vector<bool> TreeVisualiser::recSimpleNode() const 
{
  if (curNodeSelection_ != nullptr)
    // return curNodeSelection_->simplifiedMTreeNode()->reconstruct(domain_);
    return curNodeSelection_->mnode()->reconstruct(domain_);
  else
    return std::vector<bool>();
}

std::vector<bool> TreeVisualiser::recFullNode() const 
{
  if (curNodeSelection_ != nullptr)
    //return curNodeSelection_->mtreeNode()->reconstruct(domain_);
    return curNodeSelection_->mnode()->reconstruct(domain_);
  else
    return std::vector<bool>();
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
    
    // reconstructBinaryImage(iv, curNodeSelection_->mtreeNode());    
    reconstructBinaryImage(iv, curNodeSelection_->mnode());
  }
}

void TreeVisualiser::binRecPlusBtn_press()
{
  SimpleImageViewer *iv = new SimpleImageViewer;
  MyDockWidget *dock = mainWidget_->createDockWidget(
    tr("binary node reconstruction"), iv);
  dock->setGNode(curNodeSelection_);

  dock->resize(domain_.width() + 22, domain_.height() + 84);
  // reconstructBinaryImage(iv, curNodeSelection_->mtreeNode());  
  reconstructBinaryImage(iv, curNodeSelection_->mnode());
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
    
    // reconstructGreyImage(iv, curNodeSelection_->mtreeNode());    
    reconstructGreyImage(iv, curNodeSelection_->mnode());
  }
}

void TreeVisualiser::greyRecPlusBtn_press()
{
  SimpleImageViewer *iv = new SimpleImageViewer;
  MyDockWidget *dock = mainWidget_->createDockWidget(
    tr("greyscale node reconstruction"), iv);
  dock->setGNode(curNodeSelection_);

  dock->resize(domain_.width() + 22, domain_.height() + 84);
  // reconstructGreyImage(iv, curNodeSelection_->mtreeNode());  
  reconstructGreyImage(iv, curNodeSelection_->mnode());
}

void TreeVisualiser::SplineManipulateBtn_press()
{
  if (curNodeSelection_ != nullptr) {
    //CpViewer *cv = nullptr;
    if (SplineManipDock_ == nullptr) {
      
      cv = new CpViewer(static_cast<int>(domain_.width()), static_cast<int>(domain_.height()));
      
      SplineManipDock_ = mainWidget_->createDockWidget(tr("Spline-based shape manipulation"), cv);
      
      connect(SplineManipDock_, &MyDockWidget::closed, this, &TreeVisualiser::SplineManipDock_onClose);
      SplineManipDock_->resize(domain_.width() + 32, domain_.height() + 170);
    }
    else {
      cv = qobject_cast<CpViewer *>(SplineManipDock_->widget());
      cv->Update();
    }
 
    // NodePtr mnode = curNodeSelection_->mtreeNode();
    NodePtr mnode = curNodeSelection_->mnode();
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
void TreeVisualiser::skelRecBtn_press()
{
  SimpleImageViewer *iv = nullptr;
  if (skelRecDock_ == nullptr) {
    iv = new SimpleImageViewer;
    skelRecDock_ = mainWidget_->createDockWidget(tr("SDMD reconstruction of the selected nodes"), iv);
    skelRecDock_->setGNode(curNodeSelection_);//?
    connect(skelRecDock_, &MyDockWidget::closed, this, &TreeVisualiser::skelRecDock_onClose);
    skelRecDock_->resize(domain_.width() + 22, domain_.height() + 84);
  }
  else {
    iv = qobject_cast<SimpleImageViewer *>(skelRecDock_->widget());
  }

  vector<int> keptNodes;
  for (int i = 0; i < selectedNodesForRec_.size(); ++i) {
    if (selectedNodesForRec_[i]) 
      keptNodes.push_back(i);
  }

  dmdrecon_->ReconstructMultiNode(false, keptNodes, 1);
  QImage img = fieldToQImage(dmdrecon_->getOutput());    
  iv->setImage(img);    
}

void TreeVisualiser::removeSkelBtn_press()
{
  SimpleImageViewer *iv = nullptr;
  if (removeSkelDock_ == nullptr) {
    iv = new SimpleImageViewer;
    removeSkelDock_ = mainWidget_->createDockWidget(tr("SDMD remove skeleton reconstruction"), iv);
    removeSkelDock_->setGNode(curNodeSelection_);
    connect(removeSkelDock_, &MyDockWidget::closed, this, &TreeVisualiser::removeSkelDock_onClose);
    removeSkelDock_->resize(domain_.width() + 22, domain_.height() + 84);
  }
  else {
    iv = qobject_cast<SimpleImageViewer *>(removeSkelDock_->widget());
  }

  // NodePtr mnode = curNodeSelection_->mtreeNode();
  NodePtr mnode = curNodeSelection_->mnode();
  //dmdrecon_->ReconstructIndexingImage(false, mnode->id(), 0);
  
  // collect nodes that should be used in the reconstruction
  vector<int> keptNodes;
  for (int i=0; i < selectedNodesForRec_.size(); ++i) {
    if (selectedNodesForRec_[i])
      keptNodes.push_back(i);
  }
      
  dmdrecon_->ReconstructMultiNode(false, keptNodes, 0);
  QImage img = fieldToQImage(dmdrecon_->getOutput());    
  iv->setImage(img);  
}

void TreeVisualiser::incNodeReconBtn_press()
{
  if (curNodeSelection_ != nullptr) {
    selectedNodesForRec_[curNodeSelection_->mnode()->id()] = true;
    curNodeSelection_->setOpacity(1.0f);
  }
}

void TreeVisualiser::remNodeReconBtn_press()
{
  if (curNodeSelection_ != nullptr) {
    selectedNodesForRec_[curNodeSelection_->mnode()->id()] = false;
    curNodeSelection_->setOpacity(0.35f);
  }
}

void TreeVisualiser::nodeMousePress(GNode *node, 
  QGraphicsSceneMouseEvent *e)
{
  if (treeWidget_->dragMode() == QGraphicsView::NoDrag) {
    if (curNodeSelection_ != nullptr) {
      curNodeSelection_ ->setSelected(false);
      curNodeSelection_->update();
    }
      
    if (node->isSelected()) {
      node->setSelected(false);
      curNodeSelection_ = nullptr;
      emit nodeUnselected(node);
    }
    else {
      emit nodeSelected(node);
      node->setSelected(true);
      curNodeSelection_ = node;
    }

    node->update();
  }
}

void TreeVisualiser::binRecDock_onClose(MyDockWidget *dock)
{
  binRecDock_ = nullptr;  
}

void TreeVisualiser::greyRecDock_onClose(MyDockWidget *dock)
{
  greyRecDock_ = nullptr;
}
void TreeVisualiser::SplineManipDock_onClose(MyDockWidget *dock)
{
  SplineManipDock_ = nullptr;
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
    emit nodeSelected(node);
    node->update();
    curNodeSelection_ = node;
    treeWidget_->centerOn(node);
  }
}