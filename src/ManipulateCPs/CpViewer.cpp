#include "ManipulateCPs/CpViewer.hpp"
#include <QPushButton>
#include <QScrollArea>

#include <QWheelEvent>

#include <cmath>

#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

CpViewer::CpViewer(int W, int H, QWidget *parent)
  :QWidget(parent), w(W), h(H)
{
  QLayout *mainLayout = new QVBoxLayout;

  QLayout *btnLayout = new QHBoxLayout; 

  QPushButton *showCPsBtn = new QPushButton { QIcon{":/images/Spline_CPs_icon.png"}, tr(""), this};
  showCPsBtn->setIconSize(QSize{32, 32});
  connect(showCPsBtn, &QPushButton::clicked, this, &CpViewer::showCPsBtn_press);

  QPushButton *skelRecBtn = new QPushButton();
  skelRecBtn->setText("Reconstruct");
  //QPushButton *skelRecBtn = new QPushButton { QIcon{":/images/Skel_icon.png"}, tr(""), this};
  skelRecBtn->setFixedSize(QSize{182, 38});//?
  connect(skelRecBtn, &QPushButton::clicked, this, &CpViewer::ReconBtn_press);
  
  btnLayout->addWidget(showCPsBtn);
  btnLayout->addWidget(skelRecBtn);

  
  QHBoxLayout *CPrLayout = new QHBoxLayout;  
  
  // -------------------- number of leaves (extinction filter) ------------------------
  CPrLayout->addWidget(new QLabel{"CP radius:    ", this});
  CPradiusSlider_ = new QSlider{Qt::Horizontal, this};
  CPradiusSlider_->setRange(1, w/2);
  CPradius =  w/2;
  CPradiusSlider_->setValue(CPradius);
  CPradiusLabel_ = new QLabel(QString::number(CPradius), this);
  CPradiusLabel_->setFixedWidth(35);
  CPradiusLabel_->setAlignment(Qt::AlignHCenter);
  connect(CPradiusSlider_, &QSlider::sliderMoved, this,
    &CpViewer::CPradiusSlider_onValueChange);
  
  CPrLayout->addWidget(CPradiusSlider_);  
  CPrLayout->addWidget(CPradiusLabel_);

  mainLayout->addItem(btnLayout);
  mainLayout->addItem(CPrLayout);

  manipulate_CPs = new ManipulateCPs(w, h);
  mainLayout->addWidget(manipulate_CPs);
  
  setLayout(mainLayout);

  connect(manipulate_CPs, &ManipulateCPs::PressNode,
    this, &CpViewer::ChangeSliderValue);
}


void CpViewer::showCPsBtn_press()
{
  manipulate_CPs->ShowingCPs(CPs);
}

void CpViewer::ReconBtn_press()
{
  manipulate_CPs->ReconFromMovedCPs(recon_, inty);
}


void CpViewer::CPradiusSlider_onValueChange(int val)
{  
  if(CPradius != w/2) manipulate_CPs->changeCurrNodeR(val);
  //cout<<"CPradius "<<CPradius<<endl;
  CPradiusLabel_->setText(QString::number(val));
}

void CpViewer::ChangeSliderValue(int radius)
{
  CPradius = radius;
  CPradiusSlider_->setValue(radius);
  CPradiusLabel_->setText(QString::number(radius));
}