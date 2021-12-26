//#include <ImageViewerWidget/ImageViewerWidget.hpp>

#include "ManipulateCPs/ManipulateCPs.hpp"
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>

class QPushButton;

class CpViewer : public QWidget
{
Q_OBJECT

public:
  CpViewer(int W, int H, QWidget *parent=nullptr);

  inline void setImage(const QImage &newImage) { manipulate_CPs->setImage(newImage); }
  inline void UpdateWH(int width, int height) {manipulate_CPs-> UpdateWH(width, height);}
  inline void Update() {manipulate_CPs-> Update();}
  inline void transData(int intensity, vector<vector<Vector3<float>>> Cps, dmdReconstruct* recon) 
  {inty = intensity; manipulate_CPs->setCPs(Cps); recon_ = recon;}

protected:
  QLayout *createTextLayout();

 
protected slots:
  void showCPsBtn_press();
  void removeCPsBtn_press();
  void ReconBtn_press();
  void ReconImageBtn_press();
  void AddCPsBtn_press(); 
  void DeleteCPsBtn_press();
  void DeleteMultiCPsBtn_press();
  
  void ChangeValueDisplay(int val, int degree);
 
private:
  QLabel *CPradiusLabel_num;
  QLabel *DegreeLabel_num;
  int degree;
  ManipulateCPs *manipulate_CPs;
  int w, h;
  int inty;
  dmdReconstruct *recon_;
  //QDoubleSpinBox *CPradiusSpinBox_;
  QDoubleSpinBox *DegreeSpinBox_;
};

