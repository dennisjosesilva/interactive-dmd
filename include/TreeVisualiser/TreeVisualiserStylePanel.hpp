#pragma once 

#include <QFrame>
#include "CustomWidgets/CollapsableWidget.hpp"

#include <IcicleMorphotreeWidget/Graphics/Node/BezierFuncNodeFactory.hpp>

class QLabel;
class QGroupBox;
class QRadioButton;
class TreeVisualiser;
class QDoubleSpinBox;
class QLayout;

class TreeVisualiserStylePanel : public CollapsableMainWidget
{
Q_OBJECT
public:  
  TreeVisualiserStylePanel(TreeVisualiser *treeVis, QWidget *parent=nullptr);  

protected:
  QLayout* createTitle();
  QGroupBox *createRenderStyleSection();
  QLayout *createMeasuresSection();

  QWidget *createHLine();

  void refresh() override;

  void addBottomPanel(QWidget *panel);
  void removeBottomPanel();



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

  QWidget *bottomPanel_;

  TreeVisualiser *treeVis_;
};

class BezierControlPanel : public QWidget 
{
Q_OBJECT
public:
  using BezierFuncNodeFactory  = IcicleMorphotreeWidget::BezierFuncNodeFactory;
  using BezierFuncNodeFactoryPtr = std::shared_ptr<BezierFuncNodeFactory>;

  BezierControlPanel(TreeVisualiser* treeVis);

public slots:
  void topLeftSquareIntSpinBox_onValueChanged(double val);
  void bottomRightEdgesIntSpinBox_inValueChanged(double val);

private:
  QLayout* createSpinBox(const QString &labelTxt);

private:
  QDoubleSpinBox *topLeftSquareIntSpinBox_;
  QDoubleSpinBox *bottomRightEdgesIntSpinBox_;

  BezierFuncNodeFactoryPtr factory_;
  TreeVisualiser *treeVis_;
};