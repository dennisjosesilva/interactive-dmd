
//#include "MainWidget.hpp"
#include "InteractiveSdmd.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QIcon>
#include <QDockWidget>
#include <QAction>

#include <QSpacerItem>
#include <QSizePolicy>

#include <QDebug>
#include <QPushButton>


InteractiveSdmd::InteractiveSdmd()
{
  QLayout *mainLayout = new QVBoxLayout;
  QLayout *ToolLayout = new QHBoxLayout;

  QLayout *thresholdLayout = createSdmdControls();
  QLayout *btnLayout = createRunButtons();

  ToolLayout->addItem(thresholdLayout);
  ToolLayout->addItem(btnLayout);

  mainLayout->addItem(ToolLayout);
  
  imageViewer_ = new ImageViewerWidget{this};
  //imageViewer_->scrollAreaWidget()->viewport()->installEventFilter(this);
  mainLayout->addWidget(imageViewer_);

  bar = new QStatusBar;
  bar->showMessage(tr("Set the thresholds and press the Run button."));
  mainLayout->addWidget(bar);

  setLayout(mainLayout);
  
}

QLayout *InteractiveSdmd::createRunButtons()
{
  QLayout *btnLayout = new QHBoxLayout;

  //QPushButton *skelRecBtn = new QPushButton{ QIcon{":/images/Skel_icon.png"}, "", this};
  //skelRecBtn->setIconSize(QSize{50, 50});
  QPushButton *skelRecBtn = new QPushButton();
  skelRecBtn->setText("Run");
  skelRecBtn->setFixedSize(QSize{50, 30});
  connect(skelRecBtn, &QPushButton::clicked, this, &InteractiveSdmd::RunBtn_press);
  
  btnLayout->addWidget(skelRecBtn);

  return btnLayout;
}

QLayout *InteractiveSdmd::createSdmdControls()
{  
  QLayout *controlsLayout = new QVBoxLayout;
  QHBoxLayout *nleavesLayout = new QHBoxLayout;  
  QHBoxLayout *areaLayout = new QHBoxLayout;
  QHBoxLayout *areaDiffLayout = new QHBoxLayout;

  // -------------------- number of leaves (extinction filter) ------------------------
  nleavesLayout->addWidget(new QLabel{"# layers:    ", this});
  numberLayersSlider_ = new QSlider{Qt::Horizontal, this};
  numberLayersSlider_->setRange(1, 50);
  layerVal = 4;
  numberLayersSlider_->setValue(layerVal);
  numberLayersValueLabel_ = new QLabel(QString::number(layerVal), this);
  numberLayersValueLabel_->setFixedWidth(35);
  numberLayersValueLabel_->setAlignment(Qt::AlignHCenter);
  connect(numberLayersSlider_, &QSlider::sliderMoved, this,
    &InteractiveSdmd::numberLayersSlider_onValueChange);
  
  nleavesLayout->addWidget(numberLayersSlider_);  
  nleavesLayout->addWidget(numberLayersValueLabel_);

  controlsLayout->addItem(nleavesLayout);

  // ------------------------- area filter ------------------------------------------------
  areaLayout->addWidget(new QLabel{"Islands:      ", this});
  IslandsSlider_ = new QSlider{Qt::Horizontal, this};
  IslandsSlider_->setRange(1,100);//actually /1000, i.e.(0.001, 0.1);
  IslandsVal = 0.03;
  IslandsSlider_->setValue(IslandsVal*1000);
  IslandsValueLabel_ = new QLabel{QString::number(IslandsVal), this};
  IslandsValueLabel_->setFixedWidth(50);
  IslandsValueLabel_->setAlignment(Qt::AlignHCenter);
  connect(IslandsSlider_, &QSlider::sliderMoved, this, 
    &InteractiveSdmd::IslandsSlider_onValueChange);

  areaLayout->addWidget(IslandsSlider_);
  areaLayout->addWidget(IslandsValueLabel_);

  controlsLayout->addItem(areaLayout);

  // --------------- progressive area diff filter -----------------------------------------
  areaDiffLayout->addWidget(new QLabel{"Saliency:   ", this});
  SaliencySlider_ = new QSlider{Qt::Horizontal, this};
  SaliencySlider_->setRange(1, 200);
  SaliencyVal = 0.4;
  SaliencySlider_->setValue(SaliencyVal*100);
  SaliencyValueLabel_ = new QLabel{QString::number(SaliencyVal), this};
  SaliencyValueLabel_->setFixedWidth(50);
  SaliencyValueLabel_->setAlignment(Qt::AlignHCenter);
  connect(SaliencySlider_, &QSlider::sliderMoved, this,
    &InteractiveSdmd::SkelSaliencySlider_onValueChange);

  areaDiffLayout->addWidget(SaliencySlider_);
  areaDiffLayout->addWidget(SaliencyValueLabel_);

  controlsLayout->addItem(areaDiffLayout);

  return controlsLayout;
}

void InteractiveSdmd::RunBtn_press()
{
  bar->showMessage(tr("Removing Islands..."));
  dmdProcess_.removeIslands(IslandsVal);
  bar->showMessage(tr("Selecting layers..."));
  dmdProcess_.LayerSelection(true, layerVal);
  bar->showMessage(tr("Computing Skeletons..."));
  dmdProcess_.computeSkeletons(SaliencyVal);

  bar->showMessage(tr("Reading Control points..."));
  dmdRecon_.readControlPoints(imageViewer_->image().width(), imageViewer_->image().height(), dmdProcess_.clear_color, dmdProcess_.get_gray_levels());
  bar->showMessage(tr("Reconstruction..."));
  dmdRecon_.ReconstructImage(false);
  bar->showMessage(tr("Reconstruction finished!"));
  QImage img = fieldToImage(dmdRecon_.getOutput());    
  setImage(img);
  
}

void InteractiveSdmd::numberLayersSlider_onValueChange(int val)
{  
  layerVal = val;
  numberLayersValueLabel_->setText(QString::number(val));
}

void InteractiveSdmd::IslandsSlider_onValueChange(int val)
{
  IslandsVal = val / 1000.0;
  IslandsValueLabel_->setText(QString::number(IslandsVal));
}

void InteractiveSdmd::SkelSaliencySlider_onValueChange(int val)
{
  SaliencyVal = val / 100.0;
  SaliencyValueLabel_->setText(QString::number(SaliencyVal));
}

QImage InteractiveSdmd::fieldToImage(FIELD<float> *fimg) const
{
  QImage img{fimg->dimX(), fimg->dimY(), QImage::Format_Grayscale8};
  float *fimg_data = fimg->data();
  uchar *img_data = img.bits();

  int N = fimg->dimX() * fimg->dimY();
  for (int i = 0; i < N; ++i)
    img_data[i] = static_cast<uchar>(fimg_data[i]);
 
  return img;
}


QSize InteractiveSdmd::sizeHint() const //set the size of the DockWidget.
{
  const QImage img = imageViewer_->image();

  return QSize{ img.width(), img.height() + 50 };
}