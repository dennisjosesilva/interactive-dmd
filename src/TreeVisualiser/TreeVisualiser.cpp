  // #include <MorphotreeWidget/Graphics/GNodeEventHandler.hpp>
#include <IcicleMorphotreeWidget/Graphics/Node/GNodeEventHandler.hpp>
#include <IcicleMorphotreeWidget/Graphics/Node/GNodeFactory.hpp>
#include <IcicleMorphotreeWidget/TreeLayout/AutoSizeTreeLayout.hpp>
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
#include <QTime>
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
    binRecDock_{nullptr},
    SplineManipDock_{nullptr},
    skelRecDock_{nullptr},
    removeSkelDock_{nullptr},
    curColorBar_{nullptr},
    maxValue_{0},
    gradientRenderStyle_{true},
    shouldUpdateCustomTreeRedraw_{false}
{  
  using GNodeEventHandler = IcicleMorphotreeWidget::GNodeEventHandler;
  using FixedHeightTreeLayout = IcicleMorphotreeWidget::FixedHeightTreeLayout;    
  using GrayscaleBasedHeightTreeLayout = IcicleMorphotreeWidget::GrayscaleBasedHeightTreeLayout;
  using GradientGNodeFactory = IcicleMorphotreeWidget::GradientGNodeFactory; 
  using HGradientGNodeFactory = IcicleMorphotreeWidget::HGradientGNodeFactory;                                                      
  using AutoSizeTreeLayout = IcicleMorphotreeWidget::AutoSizeTreeLayout;
  using IcicleMorphotreeWidget = IcicleMorphotreeWidget::IcicleMorphotreeWidget;
  
  QLayout *mainLayout = new QVBoxLayout;
  QLayout *controlsLayout = new QHBoxLayout;
  QLayout *btnLayout = createButtons();

  mainLayout->addItem(btnLayout);  
  
  // treeWidget_ = new IcicleMorphotreeWidget{this, 
  //   std::make_unique<GrayscaleBasedHeightTreeLayout>(
  //     std::make_unique<GradientGNodeFactory>(),
  //     20.f, 20.f, 10.f)};

  treeWidget_ = new IcicleMorphotreeWidget{this, 
    std::make_unique<AutoSizeTreeLayout>(
      std::make_unique<HGradientGNodeFactory>(), 20.f, 20.f)};

  treeWidget_->setNodeSelectionColor(Qt::red);
  controlsLayout->addWidget(treeWidget_);  

  connect(GNodeEventHandler::Singleton(), &GNodeEventHandler::mousePress, 
    this, &TreeVisualiser::nodeMousePress);
  
  connect(treeWidget_, &IcicleMorphotreeWidget::treeAboutToBeRedrawn, this, 
    &TreeVisualiser::updateCustomTreeVisualisationWhenRedraw);

  TreeVisualiserStylePanel *tl = new TreeVisualiserStylePanel{this, nullptr};  
  CollapsableWidget *cw = new CollapsableWidget{"Style", tl, this};  
  controlsLayout->addWidget(cw);    

  mainLayout->addItem(controlsLayout);
  setLayout(mainLayout);
}

QLayout *TreeVisualiser::createButtons()
{
  QLayout *btnLayout = new QHBoxLayout;

  // Image Reconstruction button
  // ---------------------------
  QPushButton *binRecBtn = new QPushButton{ QIcon{":/images/binrec_icon.png"}, 
    tr(""), this};
  binRecBtn->setIconSize(QSize{32, 32});
  connect(binRecBtn, &QPushButton::clicked, this, &TreeVisualiser::binRecBtn_press);
  btnLayout->addWidget(binRecBtn);

  // Zoom Controls buttons
  // ----------------------
  QPushButton *fitToWindowBtn = new QPushButton{ QIcon{":/images/fit_to_widget_icon.png"}, "", this};
  fitToWindowBtn->setIconSize(QSize{32, 32});
  connect(fitToWindowBtn, &QPushButton::clicked, this, &TreeVisualiser::fitToWindowBtn_press);
  btnLayout->addWidget(fitToWindowBtn);

  QPushButton *zoomInBtn = new QPushButton{ QIcon{":/images/image_zoom_in_icon.png"}, "", this};
  zoomInBtn->setIconSize(QSize{32, 32});
  connect(zoomInBtn, &QPushButton::clicked, this, &TreeVisualiser::zoomInBtn_press);
  btnLayout->addWidget(zoomInBtn);

  QPushButton *zoomOutBtn = new QPushButton{ QIcon{":/images/image_zoom_out_icon.png"}, "", this};
  zoomOutBtn->setIconSize(QSize{32, 32});
  connect(zoomOutBtn, &QPushButton::clicked, this, &TreeVisualiser::zoomOutBtn_press);
  btnLayout->addWidget(zoomOutBtn);
  
  // Rotate Widget button
  QPushButton *rotateWidgetBtn = new QPushButton{ QIcon{":/images/rotate_icon.png"}, "", this};
  rotateWidgetBtn->setIconSize(QSize{32, 32});
  connect(rotateWidgetBtn, &QPushButton::clicked, this, &TreeVisualiser::rotateWidgetBtn_press);
  btnLayout->addWidget(rotateWidgetBtn);

  // Node selection for reconstruction buttons
  // -----------------------------------------
  QPushButton *remNodeToReconBtn = new QPushButton { QIcon{":/images/icicle_node_removal_icon.png"},
    "", this};
  remNodeToReconBtn->setIconSize(QSize{32, 32});
  connect(remNodeToReconBtn, &QPushButton::clicked, this, &TreeVisualiser::remNodeReconBtn_press);
  btnLayout->addWidget(remNodeToReconBtn);

  QPushButton *incNodeToReconBtn = new QPushButton{ QIcon{":/images/icicle_node_inclusion_icon.png"}, 
    "", this};
  incNodeToReconBtn->setIconSize(QSize{32, 32});
  connect(incNodeToReconBtn, &QPushButton::clicked, this, &TreeVisualiser::incNodeReconBtn_press);
  btnLayout->addWidget(incNodeToReconBtn);

  // SDMD Manipulation and reconstruction buttons 
  // --------------------------------------------
  QPushButton *SplineManipulateBtn = new QPushButton{ QIcon{":/images/Spline_CPs_icon.png"}, "", this};
  SplineManipulateBtn->setIconSize(QSize{32, 32});
  connect(SplineManipulateBtn, &QPushButton::clicked, this, &TreeVisualiser::SplineManipulateBtn_press);
  btnLayout->addWidget(SplineManipulateBtn);
  
  QPushButton *skelRecBtn = new QPushButton{ QIcon{":/images/Skel_icon.png"}, "", this};
  skelRecBtn->setIconSize(QSize{32, 32});
  connect(skelRecBtn, &QPushButton::clicked, this, &TreeVisualiser::skelRecBtn_press);
  btnLayout->addWidget(skelRecBtn);

  QPushButton *removeSkelBtn = new QPushButton{ QIcon{":/images/Remove_Skel_icon.png"}, "", this};
  removeSkelBtn->setIconSize(QSize{32, 32});
  connect(removeSkelBtn, &QPushButton::clicked, this, &TreeVisualiser::removeSkelBtn_press);  
  btnLayout->addWidget(removeSkelBtn);
 
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
  using HGradientNodeGNodeFactory = IcicleMorphotreeWidget::HGradientGNodeFactory;   
  treeWidget_->setGNodeFactory(std::make_unique<HGradientNodeGNodeFactory>());
  treeWidget_->updateTreeRendering();
  treeWidget_->grayscaleBar()->setShowBorders(false);
  gradientRenderStyle_ = true;  
}

void TreeVisualiser::useFixedColorGNodeStyle()
{
  using FixedColorGNodeFactory = IcicleMorphotreeWidget::FixedColorGNodeFactory;      
  treeWidget_->setGNodeFactory(std::make_unique<FixedColorGNodeFactory>());
  treeWidget_->updateTreeRendering();  
  treeWidget_->grayscaleBar()->setShowBorders(true);
  gradientRenderStyle_ = false;
}

FIELD<float> *TreeVisualiser::SDMDReconstruction(unsigned int id)
{
  dmdrecon_->ReconstructIndexingImage(id);
  return dmdrecon_->getOutput();
}

void TreeVisualiser::selectNodesForRecBasedOnIntensities(
  const std::vector<int> &sIntensities)
{
  // type aliases
  using uint32 = unsigned int;

  // Considering only 8-bit images.
  const int MAX_LEVEL = 255;

  // auxilariaries variables
  const MTree &tree = treeWidget_->mtree();
  std::vector<std::vector<uint32>> levelNodeMap(MAX_LEVEL);

  // maps each node to its associated level
  tree.tranverse([&levelNodeMap](NodePtr node) {
    levelNodeMap[node->level()].push_back(node->id());
  });

  // make all nodes unselected for reconstruction but the root node.
  selectedNodesForRec_.fill(false);
  selectedNodesForRec_[0] = true;

  // selected all nodes with level at the sIntensity list.
  for (int level : sIntensities)  {
    for (uint32 nodeId : levelNodeMap[level]) 
      selectedNodesForRec_[nodeId] = true;
  }

  // update node transparency 
  updateTransparencyOfTheNodes();
}

void TreeVisualiser::loadImage(Box domain, const std::vector<uint8> &f)
{  
  // namespace mw = MorphotreeWidget;
  using AutoSizeTreeLayout = IcicleMorphotreeWidget::AutoSizeTreeLayout;
  
  shouldUpdateCustomTreeRedraw_ = false;
  if (treeWidget_->hasAttributes()) 
    clearAttributes();

  maxValue_ = static_cast<uint32>(*std::max_element(f.begin(), f.end()));
  curSelectedNodeIndex_ = InvalidNodeIndex;
  
  treeWidget_->loadImage(domain, f);  

  std::shared_ptr<AutoSizeTreeLayout> treeLayout = 
    dynamic_pointer_cast<AutoSizeTreeLayout>(treeWidget_->treeLayout());

  treeWidget_->removeGrayScaleBar();    
  treeWidget_->addGrayScaleBar(maxValue_+1, 10.f);
  
  selectedNodesForRec_.clear();
  selectedNodesForRec_.resize(treeWidget_->mtree().numberOfNodes());
  selectedNodesForRec_.fill(true);
  
  if (gradientRenderStyle_)
    treeWidget_->grayscaleBar()->setShowBorders(false);
  else 
    treeWidget_->grayscaleBar()->setShowBorders(true);  

  shouldUpdateCustomTreeRedraw_ = true;

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

IcicleMorphotreeWidget::GNode *TreeVisualiser::curSelectedNode()
{
  if (curSelectedNodeIndex_ == InvalidNodeIndex)
    return nullptr;
  return treeWidget_->gnodes()[curSelectedNodeIndex_];
}

IcicleMorphotreeWidget::GNode *TreeVisualiser::curSelectedNode() const
{
  if (curSelectedNodeIndex_ == InvalidNodeIndex)
    return nullptr;
  return treeWidget_->gnodes()[curSelectedNodeIndex_];
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
  if (hasNodeSelected())  
    return curSelectedNode()->mnode()->reconstruct(domain_);  
  else
    return std::vector<bool>();
}

std::vector<bool> TreeVisualiser::recFullNode() const 
{
  if (hasNodeSelected()) {   
    return curSelectedNode()->mnode()->reconstruct(domain_);
  }
  else
    return std::vector<bool>();
}

void TreeVisualiser::binRecBtn_press()
{  
  if (hasNodeSelected()) {
    SimpleImageViewer *iv = nullptr;
    if (binRecDock_ == nullptr) {
      iv = new SimpleImageViewer;
      binRecDock_ = mainWidget_->createDockWidget(
        tr("binary node reconstruction"), iv);
      binRecDock_->setGNode(curSelectedNode());
      connect(binRecDock_, &MyDockWidget::closed, this, &TreeVisualiser::binRecDock_onClose);
      binRecDock_->resize(domain_.width()+22, domain_.height()+84);    
    }
    else {
      iv = qobject_cast<SimpleImageViewer *>(binRecDock_->widget());
    }
    
    // reconstructBinaryImage(iv, curNodeSelection_->mtreeNode());    
    reconstructBinaryImage(iv, curSelectedNode()->mnode());
  }
}

void TreeVisualiser::SplineManipulateBtn_press()
{
  if (hasNodeSelected()) {
    //CpViewer *cv = nullptr;
    if (SplineManipDock_ == nullptr) {
      
      cv = new CpViewer(static_cast<int>(domain_.width()), static_cast<int>(domain_.height()));
      
      SplineManipDock_ = mainWidget_->createDockWidget(tr("Spline-based shape manipulation"), cv);
      
      connect(SplineManipDock_, &MyDockWidget::closed, this, &TreeVisualiser::SplineManipDock_onClose);
      SplineManipDock_->resize(domain_.width() + 32, domain_.height() + 100);
    }
    else {
      cv = qobject_cast<CpViewer *>(SplineManipDock_->widget());
      cv->Update();
    }
 
    // NodePtr mnode = curNodeSelection_->mtreeNode();
    NodePtr mnode = curSelectedNode()->mnode();
    dmdrecon_->ReconstructIndexingImage(mnode->id());
  
   if(mnode->id() == 0){
     QMessageBox::information(0, "For your information",
        "Node_0 corresponds to the background layer, if you \n"
        "want to manipulate CPs, please select other nodes.");
   }
   else{
     vector<vector<Vector3<float>>> storeCPs = dmdrecon_->GetCPs(mnode->id());
     if(storeCPs.empty()){
       QMessageBox::information(0, "For your information",
        "The node is too small to generate any control points.");
     }
     else cv->transData(mnode->level(), storeCPs, dmdrecon_);
   }
    
  }
  else{
    QMessageBox::information(0, "For your information",
        "Please select a node.");
  }

}
void TreeVisualiser::skelRecBtn_press()
{
  // SimpleImageViewer *iv = nullptr;
  // if (skelRecDock_ == nullptr) {
  //   iv = new SimpleImageViewer;
  //   skelRecDock_ = mainWidget_->createDockWidget(tr("SDMD reconstruction of the selected nodes"), iv);
  //   skelRecDock_->setGNode(curNodeSelection_);//?
  //   connect(skelRecDock_, &MyDockWidget::closed, this, &TreeVisualiser::skelRecDock_onClose);
  //   skelRecDock_->resize(domain_.width() + 22, domain_.height() + 84);
  // }
  // else {
  //   iv = qobject_cast<SimpleImageViewer *>(skelRecDock_->widget());
  // }

  vector<int> keptNodes;
  for (int i = 0; i < selectedNodesForRec_.size(); ++i) {
    if (selectedNodesForRec_[i]) 
      keptNodes.push_back(i);
  }

  QTime time;
  time.start();
    
  QImage img = dmdrecon_->ReconstructMultiNode(mainWidget_->GetInterpState(), keptNodes, 1);

  cout<<time.elapsed()<<" ms."<<endl;
  //QImage img = fieldToQImage(dmdrecon_->getOutput());   
  mainWidget_->setReconMode(ReconMode::SDMD);
  mainWidget_->setImage(img);
  // iv->setImage(img);    
}

void TreeVisualiser::removeSkelBtn_press()
{
  
  //SimpleImageViewer *iv = nullptr;
  // if (removeSkelDock_ == nullptr) {
  //   iv = new SimpleImageViewer;
  //   removeSkelDock_ = mainWidget_->createDockWidget(tr("SDMD remove skeleton reconstruction"), iv);
  //   removeSkelDock_->setGNode(curNodeSelection_);
  //   connect(removeSkelDock_, &MyDockWidget::closed, this, &TreeVisualiser::removeSkelDock_onClose);
  //   removeSkelDock_->resize(domain_.width() + 22, domain_.height() + 84);
  // }
  // else {
  //   iv = qobject_cast<SimpleImageViewer *>(removeSkelDock_->widget());
  // }

  // NodePtr mnode = curNodeSelection_->mtreeNode();
  //NodePtr mnode = curNodeSelection_->mnode();
  //dmdrecon_->ReconstructIndexingImage(false, mnode->id(), 0);
  
  // collect nodes that should be used in the reconstruction
  vector<int> keptNodes;
  for (int i=0; i < selectedNodesForRec_.size(); ++i) {
    if (selectedNodesForRec_[i])
      keptNodes.push_back(i);
      
  }
      
  QImage img = dmdrecon_->ReconstructMultiNode(mainWidget_->GetInterpState(), keptNodes, 0);
  //QImage img = fieldToQImage(dmdrecon_->getOutput());    
  mainWidget_->setReconMode(ReconMode::SDMD);
  mainWidget_->setImage(img);
  // iv->setImage(img);  
}

void TreeVisualiser::incNodeReconBtn_press()
{
  if (hasNodeSelected()) {
    GNode *node = curSelectedNode();
    selectedNodesForRec_[node->mnode()->id()] = true;
    node->setOpacity(1.0f);
  }
}

void TreeVisualiser::remNodeReconBtn_press()
{
  if (hasNodeSelected()) {
    GNode* node = curSelectedNode();
    selectedNodesForRec_[node->mnode()->id()] = false;
    node->setOpacity(0.35f);
  }
}

void TreeVisualiser::nodeMousePress(GNode *node, 
  QGraphicsSceneMouseEvent *e)
{
  if (treeWidget_->dragMode() == QGraphicsView::NoDrag) {
    if (hasNodeSelected()) {
      GNode *selectedNode = curSelectedNode();
      selectedNode->setSelected(false);
      selectedNode->update();
    }
      
    if (node->isSelected()) {
      node->setSelected(false);
      curSelectedNodeIndex_ = InvalidNodeIndex;
      emit nodeUnselected(node);
    }
    else {
      emit nodeSelected(node);
      node->setSelected(true);
      curSelectedNodeIndex_ = node->mnode()->id();
    }

    node->update();
  }
}

void TreeVisualiser::binRecDock_onClose(MyDockWidget *dock)
{
  binRecDock_ = nullptr;  
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

void TreeVisualiser::fitToWindowBtn_press()
{
  treeWidget_->fitToWidget();
}

void TreeVisualiser::zoomInBtn_press()
{
  treeWidget_->visZoomIn();
}

void TreeVisualiser::zoomOutBtn_press()
{
  treeWidget_->visZoomOut();
}

void TreeVisualiser::rotateWidgetBtn_press()
{
  using TreeLayoutOrientation = IcicleMorphotreeWidget::TreeLayoutOrientation;

  if (treeWidget_->orientation() == TreeLayoutOrientation::Vertical)
    treeWidget_->setOrientation(TreeLayoutOrientation::Horizontal);
  else 
    treeWidget_->setOrientation(TreeLayoutOrientation::Vertical);
}

void TreeVisualiser::selectNodeByPixel(int x, int y)
{
  GNode *node = treeWidget_->gnode(x, y, selectedNodesForRec_);

  if (curSelectedNodeIndex_ != node->mnode()->id()) {
    if (hasNodeSelected()) {
      GNode *selectedNode = curSelectedNode();
      selectedNode->setSelected(false);
      selectedNode->update();
    }
    
    node->setSelected(true);
    emit nodeSelected(node);
    node->update();
    curSelectedNodeIndex_ = node->mnode()->id();
    treeWidget_->centerOn(node);
  }
}

void TreeVisualiser::updateCustomTreeVisualisationWhenRedraw()
{
  if (shouldUpdateCustomTreeRedraw_) {
    updateTransparencyOfTheNodes();
    if (hasNodeSelected())
      curSelectedNode()->setSelected(true);
  }
}