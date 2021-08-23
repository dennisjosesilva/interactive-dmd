#include <MorphotreeWidget/Graphics/GNodeEventHandler.hpp>
#include <ImageViewerWidget/ImageViewerWidget.hpp>
#include <MainWidget.hpp>

#include "MainWidget.hpp"
#include "TreeVisualiser/TreeVisualiser.hpp"
#include "TreeVisualiser/RecNodeButton.hpp"

#include <morphotree/tree/mtree.hpp>

#include <QImage>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QIcon>
#include <QDockWidget>
#include <QAction>

#include <QDebug>


MyDockWidget::MyDockWidget(const QString &title, QWidget *mainwindow)
  :QDockWidget{title, mainwindow}
{}

void MyDockWidget::closeEvent(QCloseEvent *e)
{
  emit closed(this);
}

TreeVisualiser::TreeVisualiser(MainWidget *mainWidget)
  : mainWidget_(mainWidget),
    binRecDock_{nullptr}
{
  namespace mw = MorphotreeWidget;

  QLayout *mainLayout = new QVBoxLayout;

  QLayout *btnLayout = new QHBoxLayout;

  binRecButton_ = new RecNodeButton{
    QIcon{":/images/binrec_unselect_icon.png"},
    QIcon{":/images/binrec_icon.png"},
    QIcon{":/images/binrec_plus_icon.png"}
  };  
  btnLayout->addWidget(binRecButton_);

  greyRecButton_ = new RecNodeButton{
    QIcon{":/images/greyrec_unselect_icon.png"},
    QIcon{":/images/greyrec_icon.png"},
    QIcon{":/images/greyrec_plus_icon.png"}
  };
  btnLayout->addWidget(greyRecButton_);

  mainLayout->addItem(btnLayout);

  treeWidget_ = new mw::MorphotreeWidget{mw::TreeLayout::TreeLayoutType::GraphvizWithLevel};
  mainLayout->addWidget(treeWidget_);

  connect(mw::GNodeEventHandler::Singleton(), &mw::GNodeEventHandler::mousePress, 
    this, &TreeVisualiser::nodeMousePress);

  setLayout(mainLayout);  
}

void TreeVisualiser::loadImage(Box domain, const std::vector<morphotree::uint8> &f, 
  std::shared_ptr<TreeSimplification> treeSimplification)
{
  treeWidget_->loadImage(domain, f, treeSimplification);
  domain_ = domain;
}

std::vector<morphotree::uint8> TreeVisualiser::bool2UInt8(
  const std::vector<bool> binimg) const
{
  using uint8 = morphotree::uint8;
  using uint32 = morphotree::uint32;

  std::vector<uint8> f(binimg.size());
  for (uint32 p = 0; p < f.size(); p++)
    f[p] = binimg[p] ? 0 : 255;
  
  return f;
}

void TreeVisualiser::nodeMousePress(GNode *node, QGraphicsSceneMouseEvent *e)
{  
  using NodePtr = typename GNode::MTreeNodePtr;
  using uint8 = morphotree::uint8;  


  // qDebug() << binRecDock_;

  if (binRecButton_->mode() == RecNodeButton::Mode::MonoDock) {
    ImageViewerWidget *iv = nullptr;
    if (binRecDock_ == nullptr && !binRecPlusDeck_.contains(node->simplifiedMTreeNode()->id())) {      
      iv = new ImageViewerWidget;
      binRecDock_  = mainWidget_->createDockWidget(
        tr("binary node reconstruction"), iv);
      binRecDock_->setGNode(node);
      binRecDock_->setFixedSize(domain_.width(), domain_.height());
      connect(binRecDock_, &MyDockWidget::closed, this, &TreeVisualiser::binRecDock_onClose);          

      NodePtr mnode = node->mtreeNode();    
      std::vector<uint8> bimg = bool2UInt8(mnode->reconstruct(domain_));
      QImage fimg{bimg.data(), static_cast<int>(domain_.width()), 
        static_cast<int>(domain_.height()), QImage::Format::Format_Grayscale8};
      iv->setImage(fimg);        
      node->setSelected(true);
    }
    else if (binRecDock_->gnode() == node) {
      binRecDock_->close();      
    }
    else if (!binRecPlusDeck_.contains(node->simplifiedMTreeNode()->id())) {
      binRecDock_->gnode()->setSelected(false);
      binRecDock_->gnode()->update();
      binRecDock_->setGNode(node);

      iv = qobject_cast<ImageViewerWidget *>(binRecDock_->widget());       
      NodePtr mnode = node->mtreeNode();    
      std::vector<uint8> bimg = bool2UInt8(mnode->reconstruct(domain_));
      QImage fimg{bimg.data(), static_cast<int>(domain_.width()), 
        static_cast<int>(domain_.height()), QImage::Format::Format_Grayscale8};
      iv->setImage(fimg);        
      node->setSelected(true);
    }    
  } 
  else if (binRecButton_->mode() == RecNodeButton::Mode::MultiDock) {    
    if (binRecPlusDeck_.contains(node->simplifiedMTreeNode()->id())) {
      binRecPlusDeck_[node->simplifiedMTreeNode()->id()]->close();      
    }
    else if (binRecDock_ != nullptr && binRecDock_->gnode() != node) {
      node->setSelected(true);
      ImageViewerWidget *iv = new ImageViewerWidget;    
      MyDockWidget *dock = mainWidget_->createDockWidget(
        tr("bin node reconstruction "), iv);          

      dock->setGNode(node);
      binRecPlusDeck_.insert(node->simplifiedMTreeNode()->id(), dock);
      connect(dock, &MyDockWidget::closed, this, &TreeVisualiser::binRecDockPlus_onClose);

      NodePtr mnode = node->mtreeNode();    
      std::vector<uint8> bimg = bool2UInt8(mnode->reconstruct(domain_));
      QImage fimg{bimg.data(), static_cast<int>(domain_.width()), 
        static_cast<int>(domain_.height()), QImage::Format::Format_Grayscale8};
      iv->setImage(fimg);        
    }
  }
} 

void TreeVisualiser::binRecDock_onClose(MyDockWidget *dock)
{
  binRecDock_->gnode()->setSelected(false);
  binRecDock_->gnode()->update();
  binRecDock_ = nullptr;
}

void TreeVisualiser::binRecDockPlus_onClose(MyDockWidget *dock)
{  
  dock->gnode()->setSelected(false);
  dock->gnode()->update();
  binRecPlusDeck_.remove(dock->gnode()->simplifiedMTreeNode()->id());
}