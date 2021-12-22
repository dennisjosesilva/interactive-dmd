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
  layout->addItem(createMeasuresSection());
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

QLayout *TreeVisualiserStylePanel::createMeasuresSection()
{
  QLayout *layout = new QVBoxLayout; 
  QLabel *label = new QLabel{tr("Measures"), this};
  label->setStyleSheet("font-weight: bold;");
  layout->addWidget(label);
  layout->addWidget(createHLine());

  QLayout *unitHeightLayout = new QHBoxLayout;
  QLabel *uniHeightLabel = new QLabel{tr("Unit Height: "), this};
  
  unitHeightSpinBox_ = new QDoubleSpinBox{this};
  unitHeightSpinBox_->setRange(0.1, 80);
  unitHeightSpinBox_->setSingleStep(0.5);  
  unitHeightLayout->addWidget(uniHeightLabel);
  unitHeightLayout->addWidget(unitHeightSpinBox_);
  connect(unitHeightSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
    &TreeVisualiserStylePanel::unitHeightSpinBox_onValueChanged);

  layout->addItem(unitHeightLayout);
  return layout;
}

void TreeVisualiserStylePanel::unitHeightSpinBox_onValueChanged(double val)
{  
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