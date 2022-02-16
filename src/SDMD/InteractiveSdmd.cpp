
//#include "MainWidget.hpp"
#include "SDMD/InteractiveSdmd.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QIcon>
#include <QDockWidget>
#include <QAction>

#include <QSpacerItem>
#include <QSizePolicy>
#include <QCheckBox>
#include <QDebug>
#include <QPushButton>
#include <QColorDialog>
#include <QInputDialog>
#include <QFileDialog>
 
 
InteractiveSdmd::InteractiveSdmd()
{
  QLayout *mainLayout = new QVBoxLayout;

  QLayout *SaliencybtnLayout = createSaliencyButtons();

  mainLayout->addItem(SaliencybtnLayout);

  QLayout *ToolLayout = new QHBoxLayout;

  QLayout *thresholdLayout = createSdmdControls();
  QLayout *btnLayout = createRunButtons();

  ToolLayout->addItem(thresholdLayout);
  ToolLayout->addItem(btnLayout);

  mainLayout->addItem(ToolLayout);
  
  //imageViewer_ = new ImageViewerWidget{this};
  scribble_ = new scribbleWidget();
  mainLayout->addWidget(scribble_);

  bar = new QStatusBar;
  bar->showMessage(tr("Set the thresholds and press the Run button."));
  mainLayout->addWidget(bar);

  setLayout(mainLayout);
  
}
QLayout *InteractiveSdmd::createRunButtons()
{
  QLayout *btnLayout = new QVBoxLayout;

  QCheckBox *checkBox = new QCheckBox(this);
  checkBox->setText("Interp");
  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
  btnLayout->addWidget(checkBox);

  //QPushButton *skelRecBtn = new QPushButton{ QIcon{":/images/Skel_icon.png"}, "", this};
  //skelRecBtn->setIconSize(QSize{50, 50});
  QPushButton *skelRecBtn = new QPushButton();
  skelRecBtn->setText("Run");
  skelRecBtn->setFixedSize(QSize{50, 30});
  connect(skelRecBtn, &QPushButton::clicked, this, &InteractiveSdmd::RunBtn_press);
  
  btnLayout->addWidget(skelRecBtn);

  return btnLayout;
}
QLayout *InteractiveSdmd::createSaliencyButtons()
{
    QLayout *btnLayout = new QHBoxLayout;

    QPushButton *colorBtn = new QPushButton{ QIcon{":/images/color_icon.png"}, "", this};
    colorBtn->setIconSize(QSize{32, 32});
    connect(colorBtn, &QPushButton::clicked, this, &InteractiveSdmd::colorBtn_press);
    
    QPushButton *widthBtn = new QPushButton{ QIcon{":/images/width_icon.png"}, "", this};
    widthBtn->setIconSize(QSize{32, 32});
    connect(widthBtn, &QPushButton::clicked, this, &InteractiveSdmd::widthBtn_press);

    QPushButton *clearBtn = new QPushButton{ QIcon{":/images/clear_screen_icon.png"}, "", this};
    clearBtn->setIconSize(QSize{32, 32});
    connect(clearBtn, &QPushButton::clicked, this, &InteractiveSdmd::clearBtn_press);
    
    QPushButton *clearImgBtn = new QPushButton{ QIcon{":/images/clear_image_icon.png"}, "", this};
    clearImgBtn->setIconSize(QSize{32, 32});
    connect(clearImgBtn, &QPushButton::clicked, this, &InteractiveSdmd::clearImgBtn_press);
    
    QPushButton *saliencyBtn = new QPushButton{ QIcon{":/images/saliency_icon.png"}, "", this};
    saliencyBtn->setIconSize(QSize{32, 32});
    connect(saliencyBtn, &QPushButton::clicked, this, &InteractiveSdmd::saliencyBtn_press);
    
    QPushButton *DrawEllipseBtn = new QPushButton{ QIcon{":/images/ellipse_icon.png"}, "", this};
    DrawEllipseBtn->setCheckable(true);
    DrawEllipseBtn->setIconSize(QSize{32, 32});
    connect(DrawEllipseBtn, &QPushButton::toggled, this, &InteractiveSdmd::DrawEllipseBtn_toggled);
    

    btnLayout->addWidget(colorBtn);
    btnLayout->addWidget(widthBtn);
    btnLayout->addWidget(clearBtn);
    btnLayout->addWidget(clearImgBtn);
    btnLayout->addWidget(DrawEllipseBtn);
    btnLayout->addWidget(saliencyBtn);

    return btnLayout;
}
QLayout *InteractiveSdmd::createSdmdControls()
{  
  QLayout *controlsLayout = new QVBoxLayout;
  QHBoxLayout *nleavesLayout = new QHBoxLayout;  
  QHBoxLayout *IslandsLayout = new QHBoxLayout;
  QHBoxLayout *saliencyLayout = new QHBoxLayout;
  QHBoxLayout *HDLayout = new QHBoxLayout;

  // -------------------- number of layers (extinction filter) ------------------------
  nleavesLayout->addWidget(new QLabel{"# layers:     ", this});
  numberLayersSlider_ = new QSlider{Qt::Horizontal, this};
  numberLayersSlider_->setRange(1, 50);
  layerVal = 15;
  numberLayersSlider_->setValue(layerVal);
  numberLayersValueLabel_ = new QLabel(QString::number(layerVal), this);
  numberLayersValueLabel_->setFixedWidth(35);
  numberLayersValueLabel_->setAlignment(Qt::AlignHCenter);
  connect(numberLayersSlider_, &QSlider::sliderMoved, this,
    &InteractiveSdmd::numberLayersSlider_onValueChange);
  
  nleavesLayout->addWidget(numberLayersSlider_);  
  nleavesLayout->addWidget(numberLayersValueLabel_);

  controlsLayout->addItem(nleavesLayout);

  // ------------------------- Islands ------------------------------------------------
  IslandsLayout->addWidget(new QLabel{"Islands:       ", this});
  IslandsSlider_ = new QSlider{Qt::Horizontal, this};
  IslandsSlider_->setRange(1,100);//actually /1000, i.e.(0.001, 0.1);
  IslandsVal = 0.01;
  IslandsSlider_->setValue(IslandsVal*1000);
  IslandsValueLabel_ = new QLabel{QString::number(IslandsVal), this};
  IslandsValueLabel_->setFixedWidth(50);
  IslandsValueLabel_->setAlignment(Qt::AlignHCenter);
  connect(IslandsSlider_, &QSlider::sliderMoved, this, 
    &InteractiveSdmd::IslandsSlider_onValueChange);

  IslandsLayout->addWidget(IslandsSlider_);
  IslandsLayout->addWidget(IslandsValueLabel_);

  controlsLayout->addItem(IslandsLayout);

  // --------------- Saliency -----------------------------------------
  saliencyLayout->addWidget(new QLabel{"Saliency:    ", this});
  SaliencySlider_ = new QSlider{Qt::Horizontal, this};
  SaliencySlider_->setRange(1, 200);
  SaliencyVal = 1.08;
  SaliencySlider_->setValue(SaliencyVal*100);
  SaliencyValueLabel_ = new QLabel{QString::number(SaliencyVal), this};
  SaliencyValueLabel_->setFixedWidth(50);
  SaliencyValueLabel_->setAlignment(Qt::AlignHCenter);
  connect(SaliencySlider_, &QSlider::sliderMoved, this,
    &InteractiveSdmd::SkelSaliencySlider_onValueChange);

  saliencyLayout->addWidget(SaliencySlider_);
  saliencyLayout->addWidget(SaliencyValueLabel_);

  controlsLayout->addItem(saliencyLayout);

// --------------- HD filter -----------------------------------------
  HDLayout->addWidget(new QLabel{"Hausdorff: ", this});
  HDSlider_ = new QSlider{Qt::Horizontal, this};
  HDSlider_->setRange(1, 20);//actually /2000, i.e.(0.0005, 0.01);
  HDVal = 0.003;
  HDSlider_->setValue(HDVal*2000);
  HDLabel_ = new QLabel{QString::number(HDVal), this};
  HDLabel_->setFixedWidth(50);
  HDLabel_->setAlignment(Qt::AlignHCenter);
  connect(HDSlider_, &QSlider::sliderMoved, this,
    &InteractiveSdmd::HDSlider_onValueChange);

  HDLayout->addWidget(HDSlider_);
  HDLayout->addWidget(HDLabel_);

  controlsLayout->addItem(HDLayout);
  return controlsLayout;
}
void InteractiveSdmd::onStateChanged(int state)
{
  if(state == 2) InterpState = true;//checked
  else InterpState = false;//0-unchecked

}
void InteractiveSdmd::RunBtn_press()
{
  int CPnum;
  
  bar->showMessage(tr("Removing Islands..."));
  
  dmdProcess_.removeIslands(IslandsVal, nullptr);
  bar->showMessage(tr("Selecting layers..."));
  dmdProcess_.LayerSelection(true, layerVal);
  bar->showMessage(tr("Computing Skeletons..."));
  // CPnum = dmdProcess_.computeSkeletons(SaliencyVal, HDVal, nullptr);

  // bar->showMessage(tr("Reading Control points..."));
  // dmdRecon_.readControlPoints(scribble_->image().width(), scribble_->image().height(), dmdProcess_.clear_color, dmdProcess_.get_gray_levels());
  // bar->showMessage(tr("Reconstruction..."));
  // cout<<"InterpState: "<<InterpState<<endl;
  // QImage img = dmdRecon_.ReconstructImage(InterpState);
  
  // bar->showMessage("Reconstruction finished! Total CPs: " + QString::number(CPnum));

  // //QImage img = fieldToImage(dmdRecon_.getOutput());    
  // setImage(img);
  
}

void InteractiveSdmd::saliencyBtn_press(){
  
  int CPnum;
  FIELD<float> *scribble_sm = imageToField(scribble_->image());
  bar->showMessage(tr("Removing Islands..."));
  dmdProcess_.removeIslands(IslandsVal, scribble_sm);//scribble_sm has been modified.
 
  //scribble_->saliencyProcess();
  bar->showMessage(tr("Selecting layers..."));
  dmdProcess_.LayerSelection(true, layerVal);
  bar->showMessage(tr("Computing Skeletons..."));
  //CPnum = dmdProcess_.computeSkeletons(SaliencyVal, HDVal, scribble_sm);
  //CPnum = dmdProcess_.computeSkeletons(SaliencyVal, HDVal, nullptr);
  //
  //bar->showMessage(tr("Reading Control points..."));
  // dmdRecon_.readControlPoints(scribble_->image().width(), scribble_->image().height(), dmdProcess_.clear_color, dmdProcess_.get_gray_levels());
  // bar->showMessage(tr("Reconstruction..."));
  // QImage img = dmdRecon_.ReconstructImage(InterpState);
  
  // bar->showMessage("Reconstruction finished! Total CPs: " + QString::number(CPnum));

  // //QImage img = fieldToImage(dmdRecon_.getOutput());    
  // setImage(img);
}

void InteractiveSdmd::DrawEllipseBtn_toggled(bool checked){
  if(checked) scribble_->setEllipseMode(true);
  else scribble_->setEllipseMode(false);
}

void InteractiveSdmd::clearBtn_press(){
  scribble_->clearImage();
}
void InteractiveSdmd::clearImgBtn_press(){
  QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Open File"), QDir::currentPath());
  if (!fileName.isEmpty())
      scribble_->openImage(fileName);
}

void InteractiveSdmd::colorBtn_press(){
  QColor newColor = QColorDialog::getColor(scribble_->penColor());
  if (newColor.isValid())
      scribble_->setPenColor(newColor);
}
void InteractiveSdmd::widthBtn_press(){
  bool ok;
  int newWidth = QInputDialog::getInt(this, tr("Scribble"),
                                      tr("Select pen width:"),
                                      scribble_->penWidth(),
                                      1, 100, 50, &ok);
  if (ok)
      scribble_->setPenWidth(newWidth);
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

void InteractiveSdmd::HDSlider_onValueChange(int val)
{
  HDVal = val / 2000.0;
  HDLabel_->setText(QString::number(HDVal));
}

FIELD<float> *InteractiveSdmd::imageToField(QImage img) const
{
  
  FIELD<float> *fimg = new FIELD<float>{ 
    static_cast<int>(img.width()), static_cast<int>(img.height()) };

  float *fimg_data = fimg->data();
  uchar *img_data = img.bits();

  int N = img.width() * img.height();
  for (int i = 0; i < N; ++i)
    fimg_data[i] = static_cast<float>(img_data[i]);
  
  //fimg->NewwritePGM("new.pgm");
  return fimg;
}
