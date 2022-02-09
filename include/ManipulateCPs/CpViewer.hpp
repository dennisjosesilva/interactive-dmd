//#include <ImageViewerWidget/ImageViewerWidget.hpp>

#pragma once

#include "ManipulateCPs/ManipulateCPs.hpp"
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>
//#include <QStatusBar>
#include "ManipulateCPs/AddHoverInfoPushButton.hpp"
#include "ManipulateCPs/ShowDegreeColorWidget.hpp"

class QPushButton;
class MainWidget;


class CpViewer : public QWidget
{
Q_OBJECT

public:
  CpViewer(int W, int H, MainWidget *mainWidget, QWidget *parent=nullptr);

  inline void setImage(const QImage &newImage) { manipulate_CPs->setImage(newImage); }
  inline void UpdateWH(int width, int height) {manipulate_CPs-> UpdateWH(width, height);}
  inline void Update() {manipulate_CPs-> Update();}
  //inline void transData(dmdReconstruct* recon) 
  //{recon_ = recon; getCPsMap(); }
  void transData(dmdReconstruct* recon); 
  
  void getCPsMap();
  
protected:
  QLayout *createTextLayout();
  QLayout *createDegreeColorLayout();

 
protected slots:
  void showCPsBtn_press();
  void removeCPsBtn_press();
  void ReconBtn_press();
  void ReconImageBtn_press();
  void AddCPsBtn_press(); 
  void DeleteCPsBtn_press();
  void DeleteMultiCPsBtn_press();
  void DeleteABranchBtn_press();
  void rotateCPsBtn_press();
  void ZoomInOutBtn_press();
  
  void ChangeValueDisplay(int val, int degree);
  //void SetUnSync();
 
private:
  QLabel *CPradiusLabel_num;
  QLabel *DegreeLabel_num;
  int degree;
  ManipulateCPs *manipulate_CPs;
  int w, h;
  
  dmdReconstruct *recon_;
  
  QDoubleSpinBox *DegreeSpinBox_;

  AddHoverInfoPushButton *AddCPsBtn;
  AddHoverInfoPushButton *DeleteCPsBtn;
  AddHoverInfoPushButton *DeleteABranchBtn;  

  MainWidget *MainWidgetImageViewer;
  
};

