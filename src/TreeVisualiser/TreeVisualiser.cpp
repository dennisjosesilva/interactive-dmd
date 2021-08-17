#include "TreeVisualiser/TreeVisualiser.hpp"
#include "TreeVisualiser/RecNodeButton.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QIcon>

TreeVisualiser::TreeVisualiser(QWidget *parent)
  :QWidget(parent)
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

  setLayout(mainLayout);  
}

void TreeVisualiser::loadImage(Box domain, const std::vector<morphotree::uint8> &f, 
  std::shared_ptr<TreeSimplification> treeSimplification)
{
  treeWidget_->loadImage(domain, f, treeSimplification);
}

