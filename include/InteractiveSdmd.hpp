#pragma once 

#include <QWidget>
#include <QImage>
#include <QMap>
#include <QLabel>
#include <QSlider>
#include <QDockWidget>
#include <QString>
#include <QStatusBar>
#include <ImageViewerWidget/ImageViewerWidget.hpp>
#include "dmdProcess.hpp"
#include "dmdReconstruct.hpp"

class InteractiveSdmd : public QWidget
{
Q_OBJECT

protected:
  using ImageViewerWidget = ImageViewerWidget::ImageViewerWidget;

public:

  InteractiveSdmd();
  inline void setImage(const QImage &newImage) { imageViewer_->setImage(newImage); }
  inline void readIntoSdmd(const char *c_str) { dmdProcess_.readFromFile(c_str); }
  QSize sizeHint() const override;

protected:  
  
  QImage fieldToImage(FIELD<float> *field) const; 

  QLayout* createRunButtons();
  QLayout* createSdmdControls();
  ImageViewerWidget *imageViewer_;

protected slots:
   
  void RunBtn_press(); 
 
  void numberLayersSlider_onValueChange(int val);
  void IslandsSlider_onValueChange(int val);
  void SkelSaliencySlider_onValueChange(int val);

private:
  QSlider *numberLayersSlider_;
  QLabel *numberLayersValueLabel_;

  QSlider *IslandsSlider_;
  QLabel *IslandsValueLabel_;

  QSlider *SaliencySlider_;
  QLabel *SaliencyValueLabel_;


  dmdProcess dmdProcess_;
  dmdReconstruct dmdRecon_;
  int layerVal;
  float IslandsVal, SaliencyVal;
  QStatusBar *bar;
 
};