#pragma once 

#include <QFrame>

class QLabel;
class QGroupBox;
class QRadioButton;
class TreeVisualiser;
class QDoubleSpinBox;
class QLayout;

class TreeVisualiserStylePanel : public QFrame
{
Q_OBJECT
public:  
  TreeVisualiserStylePanel(TreeVisualiser *treeVis, QWidget *parent=nullptr);  

protected:
  QLayout* createTitle();
  QGroupBox *createRenderStyleSection();
  QLayout *createMeasuresSection();

  QWidget *createHLine();

protected slots:
  void radioButtonRenderStyle_onToogle(bool checked);

private:
  QLabel *title_;

  QRadioButton *bezierFuncLuminanceRadioButton_;
  QRadioButton *flatRadioButton_;
  QRadioButton *gradientRadioButton_;
  QRadioButton *glGradientDefaultRadioButton_;
  QRadioButton *glGradientFlatRadioButton_;
  QRadioButton *glGradientHorizontalRadioButton_;
  QRadioButton *glGradientVerticalRadioButton_;
  QRadioButton *glGradientSymmetricTentLikeCushionRadioButton_;
  QRadioButton *glGradientAsymmetricTentLikeCushionRadioButton_;

  TreeVisualiser *treeVis_;
};
