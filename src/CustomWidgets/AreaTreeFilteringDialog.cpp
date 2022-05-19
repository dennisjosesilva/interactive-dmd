#include "CustomWidgets/AreaTreeFilteringDialog.hpp"
#include <morphotree/adjacency/adjacency8c.hpp>
#include <QSpinBox>

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>


AreaTreeFilteringDialog::AreaTreeFilteringDialog(const Box &domain,
  const std::vector<uint8> &f, QWidget *parent)
  : QDialog{parent}, 
    tree_{MTreeType::MaxTree},
    domain_{domain},
    filtering_{std::make_unique<IcicleMorphotreeWidget::AreaTreeFiltering>(0)}
{
  using morphotree::buildMaxTree;
  using morphotree::Adjacency8C;

  tree_ = buildMaxTree(f, std::make_unique<Adjacency8C>(domain));

  QVBoxLayout *layout = new QVBoxLayout;
  QHBoxLayout *sliderLayout = new QHBoxLayout;

  sliderLayout->addWidget(new QLabel{tr("area threshold: "), this});
  spinBoxArea_ = new QSpinBox{this};
  spinBoxArea_->setRange(0, domain.numberOfPoints());
  connect(spinBoxArea_, QOverload<int>::of(&QSpinBox::valueChanged), this, 
    &AreaTreeFilteringDialog::spinBoxArea_onValueChanged);
  sliderLayout->addWidget(spinBoxArea_);

  layout->addItem(sliderLayout);
  
  numberOfNodesLabel_ = new QLabel{tr("#Kept nodes: %1 out of %2")
    .arg(tree_.numberOfNodes()).arg(tree_.numberOfNodes())};
  layout->addWidget(numberOfNodesLabel_);

  imageViewer_ = new ImageViewerWidget{this};
  QImage img{f.data(), 
    static_cast<int>(domain.width()), static_cast<int>(domain.height()), 
    QImage::Format_Grayscale8};
  imageViewer_->setImage(img);

  layout->addWidget(new QLabel{"preview", this});
  layout->addWidget(imageViewer_);

  QHBoxLayout *btnLayout = new QHBoxLayout;
  QPushButton *applyBtn = new QPushButton{tr("apply filter"), this};
  connect(applyBtn, &QPushButton::clicked, this, &AreaTreeFilteringDialog::btnApply_onClick);
  btnLayout->addWidget(applyBtn);

  QPushButton *cancelBtn = new QPushButton{tr("cancel"), this};
  connect(cancelBtn, &QPushButton::clicked, this, &AreaTreeFilteringDialog::btnCancel_onClick);
  btnLayout->addWidget(cancelBtn);

  layout->addItem(btnLayout);

  setLayout(layout);
}

void AreaTreeFilteringDialog::spinBoxArea_onValueChanged(int val)
{
  filtering_->areaThres(val);

  MTree ftree = filtering_->filter(tree_);
  std::vector<uint8> f_filtered = ftree.reconstructImage();

  numberOfNodesLabel_->setText(
    tr("#Kept nodes: %1 out of %2").arg(ftree.numberOfNodes()).arg(tree_.numberOfNodes()));

  QImage img{f_filtered.data(), 
    static_cast<int>(domain_.width()), static_cast<int>(domain_.height()), 
    QImage::Format_Grayscale8};
  imageViewer_->setImage(img);
}


void AreaTreeFilteringDialog::btnApply_onClick()
{
  resultImage_ = imageViewer_->image();
  accept();
}

void AreaTreeFilteringDialog::btnCancel_onClick()
{
  reject();
}