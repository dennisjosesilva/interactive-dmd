#include "TreeVisualiser/TreeVisualiser.hpp"
#include "TreeVisualiser/TreeVisualiserStylePanel.hpp"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QDebug>


TreeVisualiserStylePanel::TreeVisualiserStylePanel(TreeVisualiser *treeVis, 
  QWidget *parent): QFrame{parent}, treeVis_{treeVis}
{
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addItem(createTitle());
  layout->addWidget(createRenderStyleSection());
  layout->addStretch();
  setLayout(layout);
}

QLayout* TreeVisualiserStylePanel::createTitle()
{
  
  QLayout *layout = new QVBoxLayout;
  
  QLabel *titleLabel = new QLabel{tr("MorphoTree Style"), this};
  titleLabel->setStyleSheet(
    "font-weight: bold;"
  );
  layout->addWidget(titleLabel);  
  layout->addWidget(createHLine());

  return layout;
}

QWidget *TreeVisualiserStylePanel::createHLine()
{
  QFrame *line = new QFrame{this};
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  return line;
}

QGroupBox *TreeVisualiserStylePanel::createRenderStyleSection()
{
  QLayout *layout = new QVBoxLayout;

  QGroupBox *groupBox = new QGroupBox{tr("Render Style")};
  
  flatRadioButton_ = new QRadioButton{tr("Flat")};  
  connect(flatRadioButton_, &QRadioButton::toggled, this, 
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);
  
  gradientRadioButton_ = new QRadioButton{tr("Gradient")};
  gradientRadioButton_->setChecked(true);
  connect(gradientRadioButton_, &QRadioButton::toggled, this,
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);

  layout->addWidget(flatRadioButton_);
  layout->addWidget(gradientRadioButton_);

  groupBox->setLayout(layout);
  return groupBox;
}

void TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle(bool checked)
{
  if (checked) {
    QRadioButton *checkedBtn = static_cast<QRadioButton *>(sender());
    treeVis_->resetCache();
    if (checkedBtn == flatRadioButton_) {
      treeVis_->useFixedColorGNodeStyle();      
    }
    else if (checkedBtn == gradientRadioButton_) {
      treeVis_->useGradientGNodeStyle();            
    }
  }
}