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

  node->setSelected(true);

  // qDebug() << binRecDock_;

  if (binRecButton_->mode() == RecNodeButton::Mode::MonoDock) {
    ImageViewerWidget *iv = nullptr;
    if (binRecDock_ == nullptr) {            
      iv = new ImageViewerWidget;
      binRecDock_ = mainWidget_->createDockWidget(
        tr("binary node reconstruction"), iv);
      connect(binRecDock_, &QObject::destroyed, this, &TreeVisualiser::dock_onDestroy);          
    }
    else {
      iv = qobject_cast<ImageViewerWidget *>(binRecDock_->widget());
    }

    NodePtr mnode = node->mtreeNode();    
    std::vector<uint8> bimg = bool2UInt8(mnode->reconstruct(domain_));
    QImage fimg{bimg.data(), static_cast<int>(domain_.width()), 
      static_cast<int>(domain_.height()), QImage::Format::Format_Grayscale8};
    iv->setImage(fimg);            
  } 
  else if (binRecButton_->mode() == RecNodeButton::Mode::MultiDock) {
    ImageViewerWidget *iv = new ImageViewerWidget;    
    QDockWidget *dock = mainWidget_->createDockWidget(
      tr("bin node reconstruction "), iv);      
    
    NodePtr mnode = node->mtreeNode();    
    std::vector<uint8> bimg = bool2UInt8(mnode->reconstruct(domain_));
    QImage fimg{bimg.data(), static_cast<int>(domain_.width()), 
      static_cast<int>(domain_.height()), QImage::Format::Format_Grayscale8};
    iv->setImage(fimg);        
  }

} 

void TreeVisualiser::dock_onDestroy(QObject *dock)
{
  binRecDock_ = nullptr;
}