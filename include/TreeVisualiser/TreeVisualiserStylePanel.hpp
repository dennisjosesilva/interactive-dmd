#pragma once 

#include <QFrame>

class QLabel;
class QGroupBox;
class TreeVisualiser;
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

private:
  QLabel *title_;
  TreeVisualiser *treeVis_;
};
