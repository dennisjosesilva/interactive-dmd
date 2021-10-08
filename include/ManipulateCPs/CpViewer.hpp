//#include <ImageViewerWidget/ImageViewerWidget.hpp>

#include "ManipulateCPs/ManipulateCPs.hpp"
#include <QWidget>
#include <QSlider>
#include <QLabel>

class QPushButton;

class CpViewer : public QWidget
{
Q_OBJECT

public:
  CpViewer(int W, int H, QWidget *parent=nullptr);

  inline void setImage(const QImage &newImage) { manipulate_CPs->setImage(newImage); }
  inline void UpdateWH(int width, int height) {manipulate_CPs-> UpdateWH(width, height);}
  inline void Update() {manipulate_CPs-> Update(); CPradius = w/2;}
  inline void transData(int intensity, vector<vector<Vector3<float>>> Cps, dmdReconstruct* recon) 
  {inty = intensity; manipulate_CPs->setCPs(Cps); recon_ = recon;}

 
protected slots:
  void showCPsBtn_press();
  void removeCPsBtn_press();
  void ReconBtn_press();
  void ReconImageBtn_press();
  void AddCPsBtn_press();
  void DeleteCPsBtn_press();
  void CPradiusSlider_onValueChange(int val);
  void degreeSlider_onValueChange(int val);
  void ChangeSliderValue(int val, int maxDegree, int degree);
 
private:
  QSlider *CPradiusSlider_;
  QSlider *degreeSlider_;
  QLabel *CPradiusLabel_;
  QLabel *degreeLabel_;
  int CPradius;
  int degree;
  ManipulateCPs *manipulate_CPs;
  int w, h;
  int inty;
  dmdReconstruct *recon_;
};

