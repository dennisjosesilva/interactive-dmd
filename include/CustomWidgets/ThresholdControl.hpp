#pragma once 

#include <QWidget>
#include <QDoubleSpinBox>
#include "SDMD/dmdProcess.hpp"
#include "SDMD/dmdReconstruct.hpp"

class OrientablePushButton;
class QLayout;

class ThresholdControl : public QWidget
{
  Q_OBJECT
  
public:
  ThresholdControl(QWidget *parent=nullptr);
  inline void readImgIntoSdmd(const char *c_str) { dmdProcess_.readFromFile(c_str); }
  void readQImgIntoSdmd(const QImage &img);
  inline bool getInterpState() {return InterpState;}
  inline float getSaliencyVal() {return SaliencyVal;}
  inline float getHDVal() {return HDVal;}

protected:
  QLayout *createLayerThresholdLayout();
  QLayout *createIslandThresholdLayout();
  QLayout *createSaliencyThresholdLayout();
  QLayout *createHausdorffThresholdLayout();
  QLayout* createRunButtons();
  QImage fieldToImage(FIELD<float> *field) const; 
  FIELD<float> *imageToField(QImage img) const;

public: 
signals: 
  void ImageHasBeenReconstructed(QImage reconImage); 
  void LayerHasBeenSelected(vector<int> selectedIntensity);
  void DisplayOriginalImg();

protected slots:
  void layerThresSpinBox_onValueChanged(double val);
  void islandThresSpinBox_onValueChanged(double val);
  void saliencyThresSpinBox_onValueChanged(double val);
  void hausdorffThresSpinBox_onValueChanged(double val);
  void Interp_onStateChanged(int state);
  void DisplayOrigImg_onStateChanged(int state);
  void RunBtn_press(); 
 
private:
 
  QDoubleSpinBox *layerThresSpinBox_;
  QDoubleSpinBox *islandThresSpinBox_;
  QDoubleSpinBox *saliencyThresSpinBox_;
  QDoubleSpinBox *hausdorffThresSpinBox_;
  QLayout *layout_;
  bool InterpState = false;

  int layerVal;
  float IslandsVal, SaliencyVal, HDVal;

  dmdProcess dmdProcess_;
  dmdReconstruct dmdRecon_;
};