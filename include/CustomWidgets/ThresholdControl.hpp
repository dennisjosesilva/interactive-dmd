#pragma once 

#include <QWidget>
#include <QDoubleSpinBox>

class OrientablePushButton;
class QLayout;

class ThresholdControl : public QWidget
{
  Q_OBJECT
  
public:
  ThresholdControl(QWidget *parent=nullptr);

protected:
  QLayout *createLayerThresholdLayout();
  QLayout *createIslandThresholdLayout();
  QLayout *createSaliencyThresholdLayout();
  QLayout *createHausdorffThresholdLayout();
  QLayout* createRunButtons();

protected slots:
  void layerThresSpinBox_onValueChanged(double val);
  void islandThresSpinBox_onValueChanged(double val);
  void saliencyThresSpinBox_onValueChanged(double val);
  void hausdorffThresSpinBox_onValueChanged(double val);
  void onStateChanged(int state);
  void RunBtn_press(); 
 
private:
 
  QDoubleSpinBox *layerThresSpinBox_;
  QDoubleSpinBox *islandThresSpinBox_;
  QDoubleSpinBox *saliencyThresSpinBox_;
  QDoubleSpinBox *hausdorffThresSpinBox_;
  QLayout *layout_;
  bool InterpState = false;
};