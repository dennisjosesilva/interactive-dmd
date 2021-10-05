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

  mainLayout->addItem(btnLayout);

  manipulate_CPs = new ManipulateCPs(w, h);
  mainLayout->addWidget(manipulate_CPs);
  
  setLayout(mainLayout);
}


void CpViewer::showCPsBtn_press()
{
  manipulate_CPs->ShowingCPs(CPs);
}

void CpViewer::ReconBtn_press()
{
  manipulate_CPs->ReconFromMovedCPs(recon_, inty);
}