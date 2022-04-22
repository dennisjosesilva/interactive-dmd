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
  QWidget *parent): 
  CollapsableMainWidget{parent}, 
  treeVis_{treeVis}, 
  bottomPanel_{nullptr}
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
    removeBottomPanel();
    if (checkedBtn == bezierFuncLuminanceRadioButton_) {
      treeVis_->useBezierFuncGNodeStyle();
      addBottomPanel(new BezierControlPanel{treeVis_});
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

void TreeVisualiserStylePanel::refresh()
{
  addBottomPanel(new BezierControlPanel{treeVis_});
}

void TreeVisualiserStylePanel::addBottomPanel(QWidget *panel)
{
  if (bottomPanel_ != nullptr) 
    removeBottomPanel();
    
  bottomPanel_ = panel;
  layout()->addWidget(bottomPanel_);
}

void TreeVisualiserStylePanel::removeBottomPanel()
{
  if (bottomPanel_ != nullptr) {
    layout()->removeWidget(bottomPanel_);
    bottomPanel_->deleteLater();
    bottomPanel_ = nullptr;
  }
}

// Bezier Panel
BezierControlPanel::BezierControlPanel(TreeVisualiser *treeVis)
  : treeVis_{treeVis},  
    factory_{std::dynamic_pointer_cast<BezierFuncNodeFactory>(treeVis->treeWidget()->gnodeFactory())}
{
  QVBoxLayout *mainLayout = new QVBoxLayout;

  QHBoxLayout *spinBoxTopLeftLayout = new QHBoxLayout;

  // Top left square spin box   
  QLabel *topLeftSquareLabel = new QLabel{tr("top left square intensity: "), this};
  spinBoxTopLeftLayout->addWidget(topLeftSquareLabel);

  topLeftSquareIntSpinBox_ = new QDoubleSpinBox{this};
  topLeftSquareIntSpinBox_->setRange(0, 1);
  topLeftSquareIntSpinBox_->setSingleStep(0.05f);
  topLeftSquareIntSpinBox_->setValue(factory_->vtop());
  connect(topLeftSquareIntSpinBox_, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
    &BezierControlPanel::topLeftSquareIntSpinBox_onValueChanged);
  spinBoxTopLeftLayout->addWidget(topLeftSquareIntSpinBox_);

  mainLayout->addItem(spinBoxTopLeftLayout);

  // bottom right edge spog box 
  QHBoxLayout *bottomRightEdgesLayout = new QHBoxLayout;

  QLabel *bottomRightEdgesLabel = new QLabel{tr("bottom right edges intensity: "), this};
  bottomRightEdgesLayout->addWidget(bottomRightEdgesLabel);

  bottomRightEdgesIntSpinBox_ = new QDoubleSpinBox{this};
  bottomRightEdgesIntSpinBox_->setRange(0, 1);
  bottomRightEdgesIntSpinBox_->setSingleStep(0.05f);
  bottomRightEdgesIntSpinBox_->setValue(factory_->vbottom());
  connect(bottomRightEdgesIntSpinBox_, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
    &BezierControlPanel::bottomRightEdgesIntSpinBox_inValueChanged);
  bottomRightEdgesLayout->addWidget(bottomRightEdgesIntSpinBox_);
  
  mainLayout->addItem(bottomRightEdgesLayout);

  setLayout(mainLayout);
} 

void BezierControlPanel::topLeftSquareIntSpinBox_onValueChanged(double val)
{
  float fval = static_cast<float>(val);
  factory_->setHLeft(fval);
  factory_->setHMiddle(fval);
  factory_->setVTop(fval);
  factory_->setVMiddle(fval);
  treeVis_->updateTreeRendering();
}

void BezierControlPanel::bottomRightEdgesIntSpinBox_inValueChanged(double val)
{
  float fval = static_cast<float>(val);
  factory_->setVBottom(fval);
  factory_->setHRight(fval);
  treeVis_->updateTreeRendering();
}