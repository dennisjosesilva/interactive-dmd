#pragma once 

#include <QDialog>
#include <QWidget>

#include <random>


#include "ManipulateCPs/ControlPoint.hpp"

class QDoubleSpinBox;
class QSpinBox;
class QLabel;
class QCheckBox;
class QGridLayout;
class QPushButton;
class QLineEdit;
class QFrame;
class QComboBox; 

class ManipulateCPs;
class Node_;

class dmdReconstruct;

// Class and implementation based on 
// https://stackoverflow.com/questions/28618900/c-generate-random-numbers-following-normal-distribution-within-range
class MinMaxRandomNumGenerator
{
public:
  MinMaxRandomNumGenerator(float min, float max);
  MinMaxRandomNumGenerator(unsigned int seed, float min, float max);

  float gen();

private:
  float min_;
  float max_;

  std::normal_distribution<float> dist_;
  std::default_random_engine generator_;
};


class DataAugmentationDialog : public QDialog
{
public:
  DataAugmentationDialog(ManipulateCPs *manipulateCPs_, 
    bool maxTree, dmdReconstruct *recon, QWidget *parent=nullptr);

private:
  void createDisplacementInput();
  void createRadiusInput();
  void createRotationInput();
  void createScaleInput();

  QLayout *createGenerateBtn();
  void generateTranslation(QVector<ControlPoint> &cps);
  void generateRadius(QVector<ControlPoint> &cps);
  void generateRotation(QVector<ControlPoint> &cps);
  void generateScale(QVector<ControlPoint> &cps);

  QFrame *createHLineFrame();
  QLayout *createMultiSampleGeneratorPanel();

  void generateRandomChanges();

  QImage reconImage();

protected slots:
  void displacementCheckBox_stateChanged(int state);
  void radiusCheckBox_stateChanged(int state);
  void rotationCheckBox_stateChanged(int state);
  void scaleCheckBox_stateChanged(int state);

  void generateBtn_onClicked();

  // Multi samples generation
  void browserBtn_onClick();
  void generateMultiBtn_onClick();

private:
  QGridLayout *inputLayout_;

  QCheckBox *displacementCheckBox_;
  QDoubleSpinBox *displacementSpinBox_;

  QCheckBox *radiusCheckBox_;
  QDoubleSpinBox *radiusStartSpinBox_;
  QDoubleSpinBox *radiusEndSpinBox_;

  QCheckBox *rotationCheckBox_;
  QDoubleSpinBox *rotationStartSpinBox_;
  QDoubleSpinBox *rotationEndSpinBox_;

  QCheckBox *scaleCheckBox_;
  QDoubleSpinBox *scaleStartSpinBox_;
  QDoubleSpinBox *scaleEndSpinBox_;

  QPushButton *generateBtn_;

  QLineEdit *directoryLineEdit_;
  QLineEdit *basenameLineEdit_;
  QComboBox *fileExtComboBox_;
  QSpinBox *startIndexSpinBox_;
  QSpinBox *numberOfSamplesSpinBox_;
  QPushButton *generateMultiBtn_;
  
  ManipulateCPs *manipulateCPs_;

  dmdReconstruct *recon_;
  bool maxTree_;
};