#include <MorphotreeWidget/Graphics/GNodeEventHandler.hpp>
#include <MainWidget.hpp>

#include "MainWidget.hpp"
#include "TreeVisualiser/TreeVisualiser.hpp"

#include <morphotree/tree/mtree.hpp>

#include <QImage>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QIcon>
#include <QDockWidget>
#include <QAction>
#include <QLabel>
#include <QSlider>
#include <QSpacerItem>

#include <QDebug>

#include <QPushButton>

MyDockWidget::MyDockWidget(const QString &title, QWidget *mainwindow)
  :QDockWidget{title, mainwindow}  
{}

void MyDockWidget::closeEvent(QCloseEvent *e)
{
  emit closed(this);
}

TreeVisualiser::TreeVisualiser(MainWidget *mainWidget)
  : mainWidget_{mainWidget},
    curNodeSelection_{nullptr},
    binRecDock_{nullptr},
    greyRecDock_{nullptr}
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

  btnLayout->addWidget(binRecBtn);
  btnLayout->addWidget(binRecPlusBtn);
  btnLayout->addWidget(greyRecBtn);
  btnLayout->addWidget(greyRecPlusBtn);
  btnLayout->addWidget(inspectNodePlusBtn);
  btnLayout->addWidget(inspectNodeMinusBtn);

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
  connect(numberLeavesSlider_, &QSlider::valueChanged, this,
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
  connect(areaSlider_, &QSlider::valueChanged, this, 
    &TreeVisualiser::areaDiffSlider_onValueChange);

  areaLayout->addWidget(areaSlider_);
  areaLayout->addWidget(areaValueLabel_);

  controlsLayout->addItem(areaLayout);

  // --------------- progressive area diff filter -----------------------------------------
  areaDiffLayout->addWidget(new QLabel{"area diff: ", this});
  areaDiffSlider_ = new QSlider{Qt::Horizontal, this};
  areaDiffSlider_->setRange(0, 1000);
  areaDiffSlider_->setValue(200);
  areaDiffValueLabel_ = new QLabel{"200", this};
  areaDiffValueLabel_->setFixedWidth(35);
  areaDiffValueLabel_->setAlignment(Qt::AlignRight);
  connect(areaDiffSlider_, &QSlider::valueChanged, this,
    &TreeVisualiser::areaDiffSlider_onValueChange);

  areaDiffLayout->addWidget(areaDiffSlider_);
  areaDiffLayout->addWidget(areaDiffValueLabel_);

  controlsLayout->addItem(areaDiffLayout);

  return controlsLayout;
}

void TreeVisualiser::loadImage(Box domain, const std::vector<uint8> &f)
{  
  treeWidget_->loadImage(domain, f, treeSimplification_);
  domain_ = domain;
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
    }
    else {
      iv = qobject_cast<SimpleImageViewer *>(binRecDock_->widget());
    }
    
    binRecDock_->setFixedSize(static_cast<int>(domain_.width()), static_cast<int>(domain_.height()));
    reconstructBinaryImage(iv, curNodeSelection_->mtreeNode());
  }
}

void TreeVisualiser::binRecPlusBtn_press()
{
  SimpleImageViewer *iv = new SimpleImageViewer;
  MyDockWidget *dock = mainWidget_->createDockWidget(
    tr("binary node reconstruction"), iv);
  dock->setGNode(curNodeSelection_);

  dock->setFixedSize(static_cast<int>(domain_.width()), static_cast<int>(domain_.height()));
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
    }
    else {
      iv = qobject_cast<SimpleImageViewer *>(greyRecDock_->widget());
    }

    greyRecDock_->setFixedSize(static_cast<int>(domain_.width()), 
      static_cast<int>(domain_.height()));
    reconstructGreyImage(iv, curNodeSelection_->mtreeNode());    
  }
}

void TreeVisualiser::greyRecPlusBtn_press()
{
  SimpleImageViewer *iv = new SimpleImageViewer;
  MyDockWidget *dock = mainWidget_->createDockWidget(
    tr("greyscale node reconstruction"), iv);
  dock->setGNode(curNodeSelection_);

  dock->setFixedSize(static_cast<int>(domain_.width()), static_cast<int>(domain_.height()));
  reconstructGreyImage(iv, curNodeSelection_->mtreeNode());  
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
    treeWidget_->inspectNode(node->id(), treeSimplification_);
    curNodeSelection_ = nullptr;
  }
}

void TreeVisualiser::inspectNodeMinusBtn_press()
{
  if (treeWidget_->numberOfUndoNodeInspection() > 0) {
    treeWidget_->undoNodeInspection();
    curNodeSelection_ = nullptr;
  }
}

void TreeVisualiser::numberLeavesSlider_onValueChange(int val)
{  
  namespace mw = MorphotreeWidget;
  using AreaProgSimplification = mw::TreeSimplificationProgressiveAreaDifferenceFilter;

  numberLeavesValueLabel_->setText(QString::number(val));

  std::dynamic_pointer_cast<AreaProgSimplification>(treeSimplification_)
    ->numberOfLeavesToKeep(val);

  treeWidget_->simplifyTree(treeSimplification_);
}

void TreeVisualiser::areaSlider_onValueChange(int val)
{
  namespace mw = MorphotreeWidget;
  using AreaProgSimplification = mw::TreeSimplificationProgressiveAreaDifferenceFilter;

  areaValueLabel_->setText(QString::number(val));
  
  std::dynamic_pointer_cast<AreaProgSimplification>(treeSimplification_)
    ->areaThresholdToKeep(val);

  treeWidget_->simplifyTree(treeSimplification_);
}

void TreeVisualiser::areaDiffSlider_onValueChange(int val)
{
  namespace mw = MorphotreeWidget;
  using AreaProgSimplification = mw::TreeSimplificationProgressiveAreaDifferenceFilter;

  areaDiffValueLabel_->setText(QString::number(val));

  std::dynamic_pointer_cast<AreaProgSimplification>(treeSimplification_)
    ->progDifferenceThreholdToKeep(val);

  treeWidget_->simplifyTree(treeSimplification_);
}

void TreeVisualiser::binRecDock_onClose(MyDockWidget *dock)
{
  binRecDock_ = nullptr;  
}

void TreeVisualiser::greyRecDock_onClose(MyDockWidget *dock)
{
  greyRecDock_ = nullptr;
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