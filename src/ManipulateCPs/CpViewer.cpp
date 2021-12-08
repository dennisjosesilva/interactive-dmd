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

// -------------------- First Row - buttons  ------------------------
  QLayout *btnLayout = new QHBoxLayout; 

  QPushButton *showCPsBtn = new QPushButton { QIcon{":/images/Spline_CPs_icon.png"}, tr(""), this};
  showCPsBtn->setIconSize(QSize{32, 32});
  connect(showCPsBtn, &QPushButton::clicked, this, &CpViewer::showCPsBtn_press);

  QPushButton *removeCPsBtn = new QPushButton { QIcon{":/images/Remove_SplineCPs_icon.png"}, tr(""), this};
  removeCPsBtn->setIconSize(QSize{32, 32});
  connect(removeCPsBtn, &QPushButton::clicked, this, &CpViewer::removeCPsBtn_press);


  QPushButton *AddCPsBtn = new QPushButton { QIcon{":/images/AddCP_icon.png"}, tr(""), this};
  AddCPsBtn->setIconSize(QSize{32, 32});
  connect(AddCPsBtn, &QPushButton::clicked, this, &CpViewer::AddCPsBtn_press);

  QPushButton *DeleteCPsBtn = new QPushButton { QIcon{":/images/DeleteCP_icon.png"}, tr(""), this};
  DeleteCPsBtn->setIconSize(QSize{32, 32});
  connect(DeleteCPsBtn, &QPushButton::clicked, this, &CpViewer::DeleteCPsBtn_press);
  
  QPushButton *DeleteMultiCPsBtn = new QPushButton { QIcon{":/images/DeleteMultiCP_icon.png"}, tr(""), this};
  DeleteMultiCPsBtn->setIconSize(QSize{32, 32}); 
  connect(DeleteMultiCPsBtn, &QPushButton::clicked, this, &CpViewer::DeleteMultiCPsBtn_press);

  QPushButton *skelRecBtn = new QPushButton();
  skelRecBtn->setText("Reconstruct\n CC");
  //QPushButton *skelRecBtn = new QPushButton { QIcon{":/images/Skel_icon.png"}, tr(""), this};
  skelRecBtn->setFixedSize(QSize{100, 38});//?
  connect(skelRecBtn, &QPushButton::clicked, this, &CpViewer::ReconBtn_press);
  

  QPushButton *skelRecBtn_ = new QPushButton();
  skelRecBtn_->setText("Reconstruct\n Image");
  //QPushButton *skelRecBtn_ = new QPushButton { QIcon{":/images/Skel_icon.png"}, tr(""), this};
  skelRecBtn_->setFixedSize(QSize{100, 38});//?
  connect(skelRecBtn_, &QPushButton::clicked, this, &CpViewer::ReconImageBtn_press);

  btnLayout->addWidget(showCPsBtn);
  btnLayout->addWidget(removeCPsBtn);
  btnLayout->addWidget(AddCPsBtn);
  btnLayout->addWidget(DeleteCPsBtn);
  btnLayout->addWidget(DeleteMultiCPsBtn);
  btnLayout->addWidget(skelRecBtn);
  btnLayout->addWidget(skelRecBtn_);

  mainLayout->addItem(btnLayout);

// -------------------- Second Row - BranchManipulate  ------------------------
  //QLayout *BranchManipulate = new QHBoxLayout; 

  QLayout *Sliders = new QVBoxLayout;

  // ------------ CPradiusSlider_  ---------------
  QHBoxLayout *CPrLayout = new QHBoxLayout;
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
  Sliders->addItem(CPrLayout);
  
  
  // ------------ degreeSlider_  ---------------
  QHBoxLayout *degreeLayout = new QHBoxLayout;
  degreeLayout->addWidget(new QLabel{"Degree:      ", this});
  degreeSlider_ = new QSlider{Qt::Horizontal, this};
  degreeSlider_->setRange(1, 1);
  degree =  1;
  degreeSlider_->setValue(degree);
  degreeLabel_ = new QLabel(QString::number(degree), this);
  degreeLabel_->setFixedWidth(35);
  degreeLabel_->setAlignment(Qt::AlignHCenter);
  connect(degreeSlider_, &QSlider::sliderMoved, this,
    &CpViewer::degreeSlider_onValueChange);
  
  degreeLayout->addWidget(degreeSlider_);  
  degreeLayout->addWidget(degreeLabel_);
  Sliders->addItem(degreeLayout);


  //QLayout *CPbuttons = new QVBoxLayout;


  //BranchManipulate->addItem(Sliders);
  //BranchManipulate->addItem(CPbuttons);

  mainLayout->addItem(Sliders);

  manipulate_CPs = new ManipulateCPs(w, h);
  mainLayout->addWidget(manipulate_CPs);
  
  setLayout(mainLayout);

  connect(manipulate_CPs, &ManipulateCPs::PressNode,
    this, &CpViewer::ChangeSliderValue);
}


void CpViewer::showCPsBtn_press()
{
  manipulate_CPs->ShowingCPs();
}

void CpViewer::removeCPsBtn_press()
{
  manipulate_CPs->Update();
}

void CpViewer::ReconBtn_press()
{
  manipulate_CPs->ReconFromMovedCPs(recon_, inty);
}

void CpViewer::ReconImageBtn_press()
{
  manipulate_CPs->ReconImageFromMovedCPs(recon_);
}

void CpViewer::CPradiusSlider_onValueChange(int val)
{  
  if(CPradius != w/2) manipulate_CPs->changeCurrNodeR(val);
  //cout<<"CPradius "<<CPradius<<endl;
  CPradiusLabel_->setText(QString::number(val));
}
void CpViewer::degreeSlider_onValueChange(int val)
{
  manipulate_CPs->changeCurrbranchDegree(val);
  degreeLabel_->setText(QString::number(val));
}

void CpViewer::ChangeSliderValue(int radius, int maxDegree, int degree)
{
  CPradius = radius;
  CPradiusSlider_->setValue(radius);
  CPradiusLabel_->setText(QString::number(radius));

  degreeSlider_->setRange(1, maxDegree);
  degreeSlider_->setValue(degree);
  degreeLabel_->setText(QString::number(degree));

}
void CpViewer::AddCPsBtn_press()
{
  manipulate_CPs->AddOneCp();
}
void CpViewer::DeleteCPsBtn_press()
{
  manipulate_CPs->deleteCurrCp();
}
void CpViewer::DeleteMultiCPsBtn_press()
{
  manipulate_CPs->deleteMultiCp();
}