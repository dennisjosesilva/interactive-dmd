#include <MorphotreeWidget/Graphics/GNodeEventHandler.hpp>
#include <ImageViewerWidget/ImageViewerWidget.hpp>
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

  mainLayout->addItem(btnLayout);
  
  treeWidget_ = new mw::MorphotreeWidget{mw::TreeLayout::TreeLayoutType::GraphvizWithLevel};
  mainLayout->addWidget(treeWidget_);

  connect(mw::GNodeEventHandler::Singleton(), &mw::GNodeEventHandler::mousePress,
    this, &TreeVisualiser::nodeMousePress);

  setLayout(mainLayout);
}

void TreeVisualiser::loadImage(Box domain, const std::vector<uint8> &f, 
  std::shared_ptr<TreeSimplification> treeSimplification)
{
  treeSimplification_ = treeSimplification;
  treeWidget_->loadImage(domain, f, treeSimplification);
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

void TreeVisualiser::reconstructBinaryImage(ImageViewerWidget *iv, NodePtr node)
{  
  std::vector<uint8> bimg = bool2UInt8(node->reconstruct(domain_));
  qDebug() << node->id();

  QImage img{bimg.data(), static_cast<int>(domain_.width()), static_cast<int>(domain_.height()),
      QImage::Format::Format_Grayscale8};
    
  iv->setImage(img);
}

void TreeVisualiser::reconstructGreyImage(ImageViewerWidget *iv, NodePtr node)
{
  std::vector<uint8> fimg = node->reconstructGrey(domain_);
  QImage img{fimg.data(), static_cast<int>(domain_.width()), static_cast<int>(domain_.height()),
      QImage::Format::Format_Grayscale8};
  
  iv->setImage(img);
}

void TreeVisualiser::binRecBtn_press()
{  
  if (curNodeSelection_ != nullptr) {
    ImageViewerWidget *iv = nullptr;
    if (binRecDock_ == nullptr) {
      iv = new ImageViewerWidget;
      binRecDock_ = mainWidget_->createDockWidget(
        tr("binary node reconstruction"), iv);
      binRecDock_->setGNode(curNodeSelection_);
      connect(binRecDock_, &MyDockWidget::closed, this, &TreeVisualiser::binRecDock_onClose);
    }
    else {
      iv = qobject_cast<ImageViewerWidget *>(binRecDock_->widget());
    }
    
    binRecDock_->setFixedSize(static_cast<int>(domain_.width()), static_cast<int>(domain_.height()));
    reconstructBinaryImage(iv, curNodeSelection_->mtreeNode());
  }
}

void TreeVisualiser::binRecPlusBtn_press()
{
  ImageViewerWidget *iv = new ImageViewerWidget;
  MyDockWidget *dock = mainWidget_->createDockWidget(
    tr("binary node reconstruction"), iv);
  dock->setGNode(curNodeSelection_);

  dock->setFixedSize(static_cast<int>(domain_.width()), static_cast<int>(domain_.height()));
  reconstructBinaryImage(iv, curNodeSelection_->mtreeNode());  
}

void TreeVisualiser::greyRecBtn_press()
{  
  if (curNodeSelection_ != nullptr) {
    ImageViewerWidget *iv = nullptr;
    if (greyRecDock_ == nullptr) {
      iv = new ImageViewerWidget;
      greyRecDock_ = mainWidget_->createDockWidget(
        tr("grey node reconstruction"), iv);      
      greyRecDock_->setGNode(curNodeSelection_);
      connect(greyRecDock_, &MyDockWidget::closed, this, &TreeVisualiser::greyRecDock_onClose);
    }
    else {
      iv = qobject_cast<ImageViewerWidget *>(greyRecDock_->widget());
    }

    greyRecDock_->setFixedSize(static_cast<int>(domain_.width()), 
      static_cast<int>(domain_.height()));
    reconstructGreyImage(iv, curNodeSelection_->mtreeNode());    
  }
}

void TreeVisualiser::greyRecPlusBtn_press()
{
  ImageViewerWidget *iv = new ImageViewerWidget;
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

void TreeVisualiser::binRecDock_onClose(MyDockWidget *dock)
{
  binRecDock_ = nullptr;  
}

void TreeVisualiser::greyRecDock_onClose(MyDockWidget *dock)
{
  greyRecDock_ = nullptr;
}