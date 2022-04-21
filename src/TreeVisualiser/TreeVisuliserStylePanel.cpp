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
  

  bezierFuncLuminanceRadioButton_ = new QRadioButton{tr("Bezier Function Luminane Gradient")};
  bezierFuncLuminanceRadioButton_->setChecked(true);
  connect(bezierFuncLuminanceRadioButton_, &QRadioButton::toggled, this, 
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);

  glGradientDefaultRadioButton_ = new QRadioButton{tr("GL Default Bilinear Gradient")};
  connect(glGradientDefaultRadioButton_, &QRadioButton::toggled, this,
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);

  glGradientFlatRadioButton_ = new QRadioButton{tr("GL flat color")};
  connect(glGradientFlatRadioButton_, &QRadioButton::toggled, this,
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);

  glGradientHorizontalRadioButton_ = new QRadioButton{tr("GL Horizontal Gradient")};
  connect(glGradientHorizontalRadioButton_, &QRadioButton::toggled, this,
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);

  glGradientVerticalRadioButton_ = new QRadioButton{tr("GL Vertical Gradient")};
  connect(glGradientVerticalRadioButton_, &QRadioButton::toggled, this,
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);

  glGradientSymmetricTentLikeCushionRadioButton_ = new QRadioButton{
    tr("GL Symmetric Tent Like Cushion Gradient")},
  connect(glGradientSymmetricTentLikeCushionRadioButton_, &QRadioButton::toggled, this,
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);

  glGradientAsymmetricTentLikeCushionRadioButton_ = new QRadioButton{
    tr("GL Asymmetric Tent Like Cushion Gradient")};
  connect(glGradientAsymmetricTentLikeCushionRadioButton_, &QRadioButton::toggled, this,
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);

  gradientRadioButton_ = new QRadioButton{tr("Gradient")};  
  connect(gradientRadioButton_, &QRadioButton::toggled, this,
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);

  flatRadioButton_ = new QRadioButton{tr("Flat")};  
  connect(flatRadioButton_, &QRadioButton::toggled, this, 
    &TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle);
  
  layout->addWidget(bezierFuncLuminanceRadioButton_);
  layout->addWidget(glGradientDefaultRadioButton_);
  layout->addWidget(glGradientFlatRadioButton_);
  layout->addWidget(glGradientHorizontalRadioButton_);
  layout->addWidget(glGradientVerticalRadioButton_);
  layout->addWidget(glGradientSymmetricTentLikeCushionRadioButton_);
  layout->addWidget(glGradientAsymmetricTentLikeCushionRadioButton_);
  layout->addWidget(gradientRadioButton_);
  layout->addWidget(flatRadioButton_);

  groupBox->setLayout(layout);
  return groupBox;
}

void TreeVisualiserStylePanel::radioButtonRenderStyle_onToogle(bool checked)
{
  using IcicleMorphotreeWidget::DefaultPreset;
  using IcicleMorphotreeWidget::FlatPreset;
  using IcicleMorphotreeWidget::HorinzontalPreset;
  using IcicleMorphotreeWidget::VerticalPreset;
  using IcicleMorphotreeWidget::SymmetricTentLikeCushion;
  using IcicleMorphotreeWidget::AsymetricTentLikeCushion;

  if (checked) {
    QRadioButton *checkedBtn = static_cast<QRadioButton *>(sender());
    treeVis_->resetCache();
    if (checkedBtn == bezierFuncLuminanceRadioButton_) {
      treeVis_->useBezierFuncGNodeStyle();
    }
    if (checkedBtn == flatRadioButton_) {
      treeVis_->useFixedColorGNodeStyle();      
    }
    else if (checkedBtn == gradientRadioButton_) {
      treeVis_->useGradientGNodeStyle();            
    }
    else if (checkedBtn == glGradientDefaultRadioButton_) {
      treeVis_->useBilinearGradientStyle(
        std::make_shared<DefaultPreset>());
    }
    else if (checkedBtn == glGradientFlatRadioButton_) {
      treeVis_->useBilinearGradientStyle(
        std::make_shared<FlatPreset>());
    }
    else if (checkedBtn == glGradientHorizontalRadioButton_) {
      treeVis_->useBilinearGradientStyle(
        std::make_shared<HorinzontalPreset>());
    }
    else if (checkedBtn == glGradientVerticalRadioButton_) {
      treeVis_->useBilinearGradientStyle(
        std::make_shared<VerticalPreset>());
    }
    else if (checkedBtn == glGradientSymmetricTentLikeCushionRadioButton_) {
      treeVis_->useBilinearGradientStyle(
        std::make_shared<SymmetricTentLikeCushion>());
    }
    else if (checkedBtn == glGradientAsymmetricTentLikeCushionRadioButton_) {
      treeVis_->useBilinearGradientStyle(
        std::make_shared<AsymetricTentLikeCushion>());
    }
  }
}