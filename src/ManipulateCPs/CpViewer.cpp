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

  AddHoverInfoPushButton *showCPsBtn = new AddHoverInfoPushButton {tr("Show all CPs"),
   QIcon{":/images/Spline_CPs_icon.png"}, tr(""), this};
  showCPsBtn->setIconSize(QSize{32, 32});
  connect(showCPsBtn, &QPushButton::clicked, this, &CpViewer::showCPsBtn_press);

  AddHoverInfoPushButton *removeCPsBtn = new AddHoverInfoPushButton {tr("Do not show CPs"),
   QIcon{":/images/Remove_SplineCPs_icon.png"}, tr(""), this};
  removeCPsBtn->setIconSize(QSize{32, 32});
  connect(removeCPsBtn, &QPushButton::clicked, this, &CpViewer::removeCPsBtn_press);

  AddCPsBtn = new AddHoverInfoPushButton {tr("Add a CP. Select a branch (CP) first"), 
   QIcon{":/images/AddCP_icon.png"}, tr(""), this};
  //AddCPsBtn = new QPushButton { QIcon{":/images/AddCP_icon.png"}, tr(""), this};
  AddCPsBtn->setIconSize(QSize{32, 32});
  connect(AddCPsBtn, &QPushButton::clicked, this, &CpViewer::AddCPsBtn_press);

  DeleteCPsBtn = new AddHoverInfoPushButton {tr("Delete a CP"), 
  QIcon{":/images/DeleteCP_icon.png"}, tr(""), this};
  DeleteCPsBtn->setIconSize(QSize{32, 32});
  connect(DeleteCPsBtn, &QPushButton::clicked, this, &CpViewer::DeleteCPsBtn_press);
  
  AddHoverInfoPushButton *DeleteMultiCPsBtn = new AddHoverInfoPushButton {tr("Delete all selected CPs"),
   QIcon{":/images/DeleteMultiCP_icon.png"}, tr(""), this};
  DeleteMultiCPsBtn->setIconSize(QSize{32, 32}); 
  connect(DeleteMultiCPsBtn, &QPushButton::clicked, this, &CpViewer::DeleteMultiCPsBtn_press);

  DeleteABranchBtn = new AddHoverInfoPushButton {tr("Delete the selected branch"),
   QIcon{":/images/DeleteABranch_icon.png"}, tr(""), this};
  DeleteABranchBtn->setIconSize(QSize{32, 32}); 
  connect(DeleteABranchBtn, &QPushButton::clicked, this, &CpViewer::DeleteABranchBtn_press);


  AddHoverInfoPushButton *rotateCPsBtn = new AddHoverInfoPushButton {tr("Rotation. Set a focus first"),
   QIcon{":/images/rotate_CPs.png"}, tr(""), this};
  rotateCPsBtn->setIconSize(QSize{32, 32}); 
  connect(rotateCPsBtn, &QPushButton::clicked, this, &CpViewer::rotateCPsBtn_press);

  AddHoverInfoPushButton *ZoomInOutBtn = new AddHoverInfoPushButton {tr("Scaling. Set a focus first"),
   QIcon{":/images/zoom_in_out.png"}, tr(""), this};
  ZoomInOutBtn->setIconSize(QSize{32, 32}); 
  connect(ZoomInOutBtn, &QPushButton::clicked, this, &CpViewer::ZoomInOutBtn_press);


  AddHoverInfoPushButton *skelRecBtn = new AddHoverInfoPushButton(tr("Reconstruct the changed component"),
   QIcon{}, tr("Reconstruct\n CC"), this);
  //skelRecBtn->setText("Reconstruct\n CC");
  //QPushButton *skelRecBtn = new QPushButton { QIcon{":/images/Skel_icon.png"}, tr(""), this};
  skelRecBtn->setFixedSize(QSize{90, 38});//?
  connect(skelRecBtn, &QPushButton::clicked, this, &CpViewer::ReconBtn_press);
  

  AddHoverInfoPushButton *skelRecBtn_ = new AddHoverInfoPushButton(tr("Reconstruct the whole image"),
   QIcon{}, tr("Reconstruct\n Image"), this);
  //skelRecBtn_->setText("Reconstruct\n Image");
  //QPushButton *skelRecBtn_ = new QPushButton { QIcon{":/images/Skel_icon.png"}, tr(""), this};
  skelRecBtn_->setFixedSize(QSize{90, 38});//?
  connect(skelRecBtn_, &QPushButton::clicked, this, &CpViewer::ReconImageBtn_press);

  btnLayout->addWidget(showCPsBtn);
  btnLayout->addWidget(removeCPsBtn);
  btnLayout->addWidget(AddCPsBtn);
  btnLayout->addWidget(DeleteCPsBtn);
  btnLayout->addWidget(DeleteABranchBtn);
  btnLayout->addWidget(DeleteMultiCPsBtn);
  btnLayout->addWidget(rotateCPsBtn);
  btnLayout->addWidget(ZoomInOutBtn);
  btnLayout->addWidget(skelRecBtn);
  btnLayout->addWidget(skelRecBtn_);

  QLayout *CPradiusLayout = createTextLayout();
  btnLayout->addItem(CPradiusLayout);

  mainLayout->addItem(btnLayout);

  manipulate_CPs = new ManipulateCPs(w, h);
  mainLayout->addWidget(manipulate_CPs);
  
  QLayout *DegreeColorLayout = createDegreeColorLayout();
  mainLayout->addItem(DegreeColorLayout);

  setLayout(mainLayout);

  MainWidgetImageViewer->CPviewer_show_message(0);

  connect(manipulate_CPs, &ManipulateCPs::PressNode,
    this, &CpViewer::ChangeValueDisplay);

  //connect(manipulate_CPs, &ManipulateCPs::setUnSync, this, &CpViewer::SetUnSync);
}

QLayout *CpViewer::createDegreeColorLayout()
{
  QLayout *Layout = new QHBoxLayout;
  Layout->addWidget(new QLabel{"(D)egree = 1: ", this});
  ShowDegreeColorWidget *colorLine1 = new ShowDegreeColorWidget(1, this);
  Layout->addWidget(colorLine1);
  
  Layout->addWidget(new QLabel{" D = 2: ", this});
  ShowDegreeColorWidget *colorLine2 = new ShowDegreeColorWidget(2, this);
  Layout->addWidget(colorLine2);

  Layout->addWidget(new QLabel{" D = 3: ", this});
  ShowDegreeColorWidget *colorLine3 = new ShowDegreeColorWidget(3, this);
  Layout->addWidget(colorLine3);

  Layout->addWidget(new QLabel{" D = 4: ", this});
  ShowDegreeColorWidget *colorLine4 = new ShowDegreeColorWidget(4, this);
  Layout->addWidget(colorLine4);

  Layout->addWidget(new QLabel{" D > 4: ", this});
  ShowDegreeColorWidget *colorLine5 = new ShowDegreeColorWidget(5, this);
  Layout->addWidget(colorLine5);


  return Layout;
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
void CpViewer::transData(dmdReconstruct* recon, bool MaxTree)
{
  recon_ = recon; 
  Max_Tree = MaxTree;
  getCPsMap();
}

void CpViewer::getCPsMap(){
  QMap<unsigned int, vector<vector<Vector3<float>>>> CPmap = recon_->getCplistMap();
  manipulate_CPs->getCPmap(CPmap);
  if(CPmap.size() > 1)
  {
    AddCPsBtn->setDisabled(true);
    DeleteCPsBtn->setDisabled(true);
    //DeleteMultiCPsBtn->setDisabled(true);
    DeleteABranchBtn->setDisabled(true);

  }
  else{
    AddCPsBtn->setEnabled(true);
    DeleteCPsBtn->setEnabled(true);
    //DeleteMultiCPsBtn->setEnabled(true);
    DeleteABranchBtn->setEnabled(true);

  }
}
void CpViewer::showCPsBtn_press()
{
  manipulate_CPs->ShowingCPs();
  MainWidgetImageViewer->CPviewer_show_message(1);
}

void CpViewer::removeCPsBtn_press()
{
  manipulate_CPs->Update();
  MainWidgetImageViewer->CPviewer_show_message(2);
}

void CpViewer::ReconBtn_press()
{
  manipulate_CPs->ReconFromMovedCPs(recon_, Max_Tree);
  MainWidgetImageViewer->CPviewer_show_message(9);
}

void CpViewer::ReconImageBtn_press()
{
  bool HaveProcessed = manipulate_CPs->ReconImageFromMovedCPs(recon_, Max_Tree);
  if(HaveProcessed)
    MainWidgetImageViewer->markTreeAsUnsynchronized();
}

void CpViewer::ChangeValueDisplay(int radius, int degree)
{
  if(radius != 0) CPradiusLabel_num->setText(QString::number(radius));
  if(degree != 0) DegreeLabel_num->setText(QString::number(degree));
}

void CpViewer::AddCPsBtn_press()
{
  manipulate_CPs->AddOneCp();
  MainWidgetImageViewer->CPviewer_show_message(3);
}
void CpViewer::DeleteCPsBtn_press()
{
  manipulate_CPs->deleteCurrCp();
  MainWidgetImageViewer->CPviewer_show_message(4);
}
void CpViewer::DeleteMultiCPsBtn_press()
{
  manipulate_CPs->deleteMultiCp();
  MainWidgetImageViewer->CPviewer_show_message(5);
}
void CpViewer::DeleteABranchBtn_press()
{
  manipulate_CPs->DeleteTheBranch();
  MainWidgetImageViewer->CPviewer_show_message(6);
}
void CpViewer::rotateCPsBtn_press()
{
  manipulate_CPs->rotateCPsBtnPressed();
  MainWidgetImageViewer->CPviewer_show_message(7);
}
void CpViewer::ZoomInOutBtn_press()
{
  manipulate_CPs->ZoomInOutBtn_pressed();
  MainWidgetImageViewer->CPviewer_show_message(8);
}
