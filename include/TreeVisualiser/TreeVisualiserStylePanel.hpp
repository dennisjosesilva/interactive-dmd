#pragma once 

#include <QFrame>

class QLabel;
class TreeVisualiser;
class QLayout;

class TreeVisualiserStylePanel : public QFrame
{
Q_OBJECT
public:  
  TreeVisualiserStylePanel(TreeVisualiser *treeVis, QWidget *parent=nullptr);  

protected:
  QLayout* createTitle();

private:
  QLabel *title_;
  TreeVisualiser *treeVis_;
};
