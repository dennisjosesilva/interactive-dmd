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
  void unitHeightSpinBox_onValueChanged(double val);
  void radioButtonRenderStyle_onToogle(bool checked);

private:
  QLabel *title_;

  QDoubleSpinBox *unitHeightSpinBox_;
  QRadioButton *flatRadioButton_;
  QRadioButton *gradientRadioButton_;

  TreeVisualiser *treeVis_;
};
