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

  QLayout *CPradiusLayout = createTextLayout();
  btnLayout->addItem(CPradiusLayout);

  mainLayout->addItem(btnLayout);

  manipulate_CPs = new ManipulateCPs(w, h);
  mainLayout->addWidget(manipulate_CPs);
  
  setLayout(mainLayout);

  connect(manipulate_CPs, &ManipulateCPs::PressNode,
    this, &CpViewer::ChangeValueDisplay);
}

QLayout *CpViewer::createTextLayout()
{
  QLayout *TextLayout = new QVBoxLayout;
  QLayout *CPradiusLayout = new QHBoxLayout;

  QLabel *CPradiusLabel = new QLabel{tr("  Radius: "), this};
  CPradiusLabel_num = new QLabel(QString::number(0), this);
  
  CPradiusLayout->addWidget(CPradiusLabel);
  CPradiusLayout->addWidget(CPradiusLabel_num);

  QLayout *DegreeLayout = new QHBoxLayout;

  QLabel *DegreeLabel = new QLabel{tr("  Degree: "), this};
  DegreeLabel_num = new QLabel(QString::number(0), this);
 
  DegreeLayout->addWidget(DegreeLabel);
  DegreeLayout->addWidget(DegreeLabel_num);

  TextLayout->addItem(CPradiusLayout);
  TextLayout->addItem(DegreeLayout);
  
  return TextLayout; 
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

void CpViewer::ChangeValueDisplay(int radius, int degree)
{
  if(radius != 0) CPradiusLabel_num->setText(QString::number(radius));
  if(degree != 0) DegreeLabel_num->setText(QString::number(degree));
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