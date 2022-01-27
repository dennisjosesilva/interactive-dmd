#include "ManipulateCPs/CpViewer.hpp"

#include "MainWidget.hpp"

#include <QPushButton>
#include <QScrollArea>

#include <QWheelEvent>

#include <cmath>

#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

CpViewer::CpViewer(int W, int H, MainWidget *imageViewer, QWidget *parent)
  :QWidget(parent), w(W), h(H), MainWidgetImageViewer{imageViewer}
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

  AddCPsBtn = new QPushButton { QIcon{":/images/AddCP_icon.png"}, tr(""), this};
  AddCPsBtn->setIconSize(QSize{32, 32});
  connect(AddCPsBtn, &QPushButton::clicked, this, &CpViewer::AddCPsBtn_press);

  DeleteCPsBtn = new QPushButton { QIcon{":/images/DeleteCP_icon.png"}, tr(""), this};
  DeleteCPsBtn->setIconSize(QSize{32, 32});
  connect(DeleteCPsBtn, &QPushButton::clicked, this, &CpViewer::DeleteCPsBtn_press);
  
  DeleteMultiCPsBtn = new QPushButton { QIcon{":/images/DeleteMultiCP_icon.png"}, tr(""), this};
  DeleteMultiCPsBtn->setIconSize(QSize{32, 32}); 
  connect(DeleteMultiCPsBtn, &QPushButton::clicked, this, &CpViewer::DeleteMultiCPsBtn_press);

  DeleteABranchBtn = new QPushButton { QIcon{":/images/DeleteABranch_icon.png"}, tr(""), this};
  DeleteABranchBtn->setIconSize(QSize{32, 32}); 
  connect(DeleteABranchBtn, &QPushButton::clicked, this, &CpViewer::DeleteABranchBtn_press);


  QPushButton *rotateCPsBtn = new QPushButton { QIcon{":/images/rotate_CPs.png"}, tr(""), this};
  rotateCPsBtn->setIconSize(QSize{32, 32}); 
  connect(rotateCPsBtn, &QPushButton::clicked, this, &CpViewer::rotateCPsBtn_press);

  QPushButton *ZoomInOutBtn = new QPushButton { QIcon{":/images/zoom_in_out.png"}, tr(""), this};
  ZoomInOutBtn->setIconSize(QSize{32, 32}); 
  connect(ZoomInOutBtn, &QPushButton::clicked, this, &CpViewer::ZoomInOutBtn_press);


  QPushButton *skelRecBtn = new QPushButton();
  skelRecBtn->setText("Reconstruct\n CC");
  //QPushButton *skelRecBtn = new QPushButton { QIcon{":/images/Skel_icon.png"}, tr(""), this};
  skelRecBtn->setFixedSize(QSize{90, 38});//?
  connect(skelRecBtn, &QPushButton::clicked, this, &CpViewer::ReconBtn_press);
  

  QPushButton *skelRecBtn_ = new QPushButton();
  skelRecBtn_->setText("Reconstruct\n Image");
  //QPushButton *skelRecBtn_ = new QPushButton { QIcon{":/images/Skel_icon.png"}, tr(""), this};
  skelRecBtn_->setFixedSize(QSize{90, 38});//?
  connect(skelRecBtn_, &QPushButton::clicked, this, &CpViewer::ReconImageBtn_press);

  btnLayout->addWidget(showCPsBtn);
  btnLayout->addWidget(removeCPsBtn);
  btnLayout->addWidget(AddCPsBtn);
  btnLayout->addWidget(DeleteCPsBtn);
  btnLayout->addWidget(DeleteMultiCPsBtn);
  btnLayout->addWidget(DeleteABranchBtn);
  btnLayout->addWidget(rotateCPsBtn);
  btnLayout->addWidget(ZoomInOutBtn);
  btnLayout->addWidget(skelRecBtn);
  btnLayout->addWidget(skelRecBtn_);

  QLayout *CPradiusLayout = createTextLayout();
  btnLayout->addItem(CPradiusLayout);

  mainLayout->addItem(btnLayout);

  manipulate_CPs = new ManipulateCPs(w, h);
  mainLayout->addWidget(manipulate_CPs);
  
  bar = new QStatusBar;
  bar->showMessage(tr("Click the first button to show control points."));
  mainLayout->addWidget(bar);
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
void CpViewer::getCPsMap(){
  QMap<unsigned int, vector<vector<Vector3<float>>>> CPmap = recon_->getCplistMap();
  manipulate_CPs->getCPmap(CPmap);
  if(CPmap.size() > 1)
  {
    AddCPsBtn->setDisabled(true);
    DeleteCPsBtn->setDisabled(true);
    DeleteMultiCPsBtn->setDisabled(true);
    DeleteABranchBtn->setDisabled(true);

  }
  else{
    AddCPsBtn->setEnabled(true);
    DeleteCPsBtn->setEnabled(true);
    DeleteMultiCPsBtn->setEnabled(true);
    DeleteABranchBtn->setEnabled(true);

  }

}
void CpViewer::show_message(int WhichMessage)
{
  //bar->showMessage("Reconstruction finished! Total CPs: " + QString::number(CPnum));
  switch (WhichMessage)
  {
    case 1:
      bar->showMessage(tr("Show all the spline control points of the shape."));
      break;
    case 2:
      bar->showMessage(tr("Cancel the display of control points."));
      break;
    case 3:
      bar->showMessage(tr("Add a control point."));
      break;
    case 4:
      bar->showMessage(tr("Delete a control point."));
      break;
    case 5:
      bar->showMessage(tr("Delete multiple control points."));
      break;
    case 6:
      bar->showMessage(tr("Delete the current branch."));
      break;
    case 7:
      bar->showMessage(tr("Rotate the selected control points."));
      break;
    case 8:
      bar->showMessage(tr("Zoom in/out the selected control points."));
      break;
    case 9:
      bar->showMessage(tr("Display the changed shape."));
      break;
    case 10:
      bar->showMessage(tr("Display the changed image."));
      break;
   
  }

}

void CpViewer::showCPsBtn_press()
{
  manipulate_CPs->ShowingCPs();
  show_message(1);
}

void CpViewer::removeCPsBtn_press()
{
  manipulate_CPs->Update();
  show_message(2);
}

void CpViewer::ReconBtn_press()
{
  manipulate_CPs->ReconFromMovedCPs(recon_);
  show_message(9);
}

void CpViewer::ReconImageBtn_press()
{
  manipulate_CPs->ReconImageFromMovedCPs(recon_);
  show_message(10);
  MainWidgetImageViewer->addYellowBoard();
}

void CpViewer::ChangeValueDisplay(int radius, int degree)
{
  if(radius != 0) CPradiusLabel_num->setText(QString::number(radius));
  if(degree != 0) DegreeLabel_num->setText(QString::number(degree));
}
void CpViewer::AddCPsBtn_press()
{
  manipulate_CPs->AddOneCp();
  show_message(3);
}
void CpViewer::DeleteCPsBtn_press()
{
  manipulate_CPs->deleteCurrCp();
  show_message(4);
}
void CpViewer::DeleteMultiCPsBtn_press()
{
  manipulate_CPs->deleteMultiCp();
  show_message(5);
}
void CpViewer::DeleteABranchBtn_press()
{
  manipulate_CPs->deleteABranch();
  show_message(6);
}
void CpViewer::rotateCPsBtn_press()
{
  manipulate_CPs->rotateCPsBtnPressed();
  show_message(7);
}
void CpViewer::ZoomInOutBtn_press()
{
  manipulate_CPs->ZoomInOutBtn_pressed();
  show_message(8);
}