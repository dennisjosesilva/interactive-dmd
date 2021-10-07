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
  {inty = intensity; CPs = Cps; recon_ = recon;}

 
protected slots:
  void showCPsBtn_press();
  void ReconBtn_press();
  void CPradiusSlider_onValueChange(int val);
  void ChangeSliderValue(int val);
 
private:
  QSlider *CPradiusSlider_;
  QLabel *CPradiusLabel_;
  int CPradius;
  ManipulateCPs *manipulate_CPs;
  int w, h;
  int inty;
  vector<vector<Vector3<float>>> CPs;
  dmdReconstruct *recon_;
};
