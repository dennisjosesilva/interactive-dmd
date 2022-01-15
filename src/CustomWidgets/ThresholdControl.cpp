#include "CustomWidgets/ThresholdControl.hpp"
#include <QLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QTime>

ThresholdControl::ThresholdControl(QWidget *parent)
  :QWidget{parent}
{
  layout_ = new QHBoxLayout;
 
  QLayout *LayerThresholdLayout = createLayerThresholdLayout();
  layout_->addItem(LayerThresholdLayout);

  QLayout *IslandThresholdLayout = createIslandThresholdLayout();
  layout_->addItem(IslandThresholdLayout);

  QLayout *SaliencyThresholdLayout = createSaliencyThresholdLayout();
  layout_->addItem(SaliencyThresholdLayout);

  QLayout *HausdorffThresholdLayout = createHausdorffThresholdLayout();
  layout_->addItem(HausdorffThresholdLayout);

  QLayout *RunbtnLayout = createRunButtons();
  layout_->addItem(RunbtnLayout);

  setLayout(layout_);
}
// -------------------- number of layers ------------------------
QLayout *ThresholdControl::createLayerThresholdLayout()
{

  QLayout *layerThresLayout = new QHBoxLayout;

  QLabel *layerThresLabel = new QLabel{tr("L: "), this};
  
  layerThresSpinBox_ = new QDoubleSpinBox{this};
  layerThresSpinBox_->setRange(1, 255);
  layerThresSpinBox_->setSingleStep(1);
  layerThresSpinBox_->setValue(15);
  layerVal = 15;

  layerThresLayout->addWidget(layerThresLabel);
  layerThresLayout->addWidget(layerThresSpinBox_);

  connect(layerThresSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
    &ThresholdControl::layerThresSpinBox_onValueChanged);

  return layerThresLayout; 
}

 // ------------------------- Islands -----------------------
QLayout *ThresholdControl::createIslandThresholdLayout()
{
  QString str = tr("  ");
  str.append(QChar(0x3b5)); 
  str.append(tr(": ")); 
  QLabel* label = new QLabel;
  label->setText(str);
  
  QLayout *islandThresLayout = new QHBoxLayout;
  //QLabel *islandThresLabel = new QLabel{tr("  Island: "), this};
  
  islandThresSpinBox_ = new QDoubleSpinBox{this};
  islandThresSpinBox_->setRange(0.001, 0.1);
  islandThresSpinBox_->setDecimals(3);
  islandThresSpinBox_->setSingleStep(0.005);
  islandThresSpinBox_->setValue(0.01);
  IslandsVal = 0.01;

  islandThresLayout->addWidget(label);
  islandThresLayout->addWidget(islandThresSpinBox_);

  connect(islandThresSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
    &ThresholdControl::islandThresSpinBox_onValueChanged);

  return islandThresLayout; 
}

  // --------------- Saliency -----------------
QLayout *ThresholdControl::createSaliencyThresholdLayout()
{
  QString str1 = tr("  ");
  str1.append(QChar(0x3b4)); 
  str1.append(tr(": ")); 
  QLabel* label = new QLabel;
  label->setText(str1);

  QLayout *saliencyThresLayout = new QHBoxLayout;

  //QLabel *saliencyThresLabel = new QLabel{tr("  Saliency: "), this};
  
  saliencyThresSpinBox_ = new QDoubleSpinBox{this};
  saliencyThresSpinBox_->setRange(0.1, 3);
  saliencyThresSpinBox_->setSingleStep(0.05);
  saliencyThresSpinBox_->setValue(1.0);
  SaliencyVal = 1.0;

  saliencyThresLayout->addWidget(label);
  saliencyThresLayout->addWidget(saliencyThresSpinBox_);

  connect(saliencyThresSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
    &ThresholdControl::saliencyThresSpinBox_onValueChanged);

  return saliencyThresLayout; 
}
// --------------- HD filter -------------------------
QLayout *ThresholdControl::createHausdorffThresholdLayout()
{
  QString str2 = tr("  ");
  str2.append(QChar(0x3b3)); 
  str2.append(tr(": ")); 
  QLabel* label = new QLabel;
  label->setText(str2);

  QLayout *hausdorffThresLayout = new QHBoxLayout;

  //QLabel *hausdorffThresLabel = new QLabel{tr("  Hausdorff: "), this};
  
  hausdorffThresSpinBox_ = new QDoubleSpinBox{this};
  hausdorffThresSpinBox_->setRange(0.001, 0.01);
  hausdorffThresSpinBox_->setDecimals(3);
  hausdorffThresSpinBox_->setSingleStep(0.001);
  hausdorffThresSpinBox_->setValue(0.002);
  HDVal = 0.002;

  hausdorffThresLayout->addWidget(label);
  hausdorffThresLayout->addWidget(hausdorffThresSpinBox_);

  connect(hausdorffThresSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
    &ThresholdControl::hausdorffThresSpinBox_onValueChanged);

  return hausdorffThresLayout; 
}

QLayout *ThresholdControl::createRunButtons()
{
  QLayout *btnLayout = new QHBoxLayout;

  QCheckBox *checkBox = new QCheckBox(this);
  checkBox->setText("Interp");
  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(Interp_onStateChanged(int)));
  btnLayout->addWidget(checkBox);

  QCheckBox *checkBox2 = new QCheckBox(this);
  checkBox2->setText("OrigImg");
  connect(checkBox2, SIGNAL(stateChanged(int)), this, SLOT(DisplayOrigImg_onStateChanged(int)));
  btnLayout->addWidget(checkBox2);

  //QPushButton *skelRecBtn = new QPushButton{ QIcon{":/images/Skel_icon.png"}, "", this};
  //skelRecBtn->setIconSize(QSize{50, 50});
  QPushButton *skelRecBtn = new QPushButton();
  skelRecBtn->setText("Run");
  skelRecBtn->setFixedSize(QSize{50, 30});
  connect(skelRecBtn, &QPushButton::clicked, this, &ThresholdControl::RunBtn_press);
  
  btnLayout->addWidget(skelRecBtn);

  return btnLayout;
}

void ThresholdControl::layerThresSpinBox_onValueChanged(double val)
{
  layerVal = val;
}

void ThresholdControl::islandThresSpinBox_onValueChanged(double val)
{
  IslandsVal = val;
}
void ThresholdControl::saliencyThresSpinBox_onValueChanged(double val)
{
  SaliencyVal = val;
}
void ThresholdControl::hausdorffThresSpinBox_onValueChanged(double val)
{
  HDVal = val;
}
void ThresholdControl::Interp_onStateChanged(int state)
{
  if(state == 2) InterpState = true;//checked
  else InterpState = false;//0-unchecked
}
void ThresholdControl::DisplayOrigImg_onStateChanged(int state)
{
  if(state == 2) //checked
  {
    emit DisplayOriginalImg();
  }
  //else //0-unchecked

}

QImage ThresholdControl::fieldToImage(FIELD<float> *fimg) const
{
  QImage img{fimg->dimX(), fimg->dimY(), QImage::Format_Grayscale8};
  float *fimg_data = fimg->data();
  uchar *img_data = img.bits();

  int N = fimg->dimX() * fimg->dimY();
  for (int i = 0; i < N; ++i)
    img_data[i] = static_cast<uchar>(fimg_data[i]);
 
  return img;
}

void ThresholdControl::RunBtn_press()
{
  int CPnum;
  
  //bar->showMessage(tr("Removing Islands..."));
  
  dmdProcess_.removeIslands(IslandsVal, nullptr);
  //bar->showMessage(tr("Selecting layers..."));
  dmdProcess_.LayerSelection(true, layerVal);
  emit LayerHasBeenSelected(dmdProcess_.get_selected_intensity());
  //bar->showMessage(tr("Computing Skeletons..."));
  CPnum = dmdProcess_.computeSkeletons(SaliencyVal, HDVal, nullptr);

  //bar->showMessage(tr("Reading Control points..."));
  dmdRecon_.readControlPoints(dmdProcess_.getImgWidth(), dmdProcess_.getImgHeight(), dmdProcess_.clear_color, dmdProcess_.get_gray_levels());
  //bar->showMessage(tr("Reconstruction..."));
  //cout<<"InterpState: "<<InterpState<<endl;
  QTime time;
  time.start();
  QImage img = dmdRecon_.ReconstructImage(InterpState);
  //dmdRecon_.ReconstructImage(InterpState);
  //bar->showMessage("Reconstruction finished! Total CPs: " + QString::number(CPnum));
  //QImage img = fieldToImage(dmdRecon_.getOutput());    
  cout<<time.elapsed()<<" ms."<<endl;

  emit ImageHasBeenReconstructed(img);
}