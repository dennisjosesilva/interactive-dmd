#pragma once 

#include <QMainWindow>
#include "MainWidget.hpp"
#include "CustomWidgets/LabelWithProgressBar.hpp"

#include <QList>

class QDockWidget;
class QAction;
class QProgressBar;

class MainWindow : public QMainWindow
{
  Q_OBJECT 
public:
  MainWindow();

  void setMinMaxProgressBar(int min, int max);

  QAction *showTreeVisAct() { return showTreeVisAct_; }

private slots:
  void saveAs();
  void open();

  void dmdProcessAct_onTrigged();

  void treeVisAct_onToggled(bool checked=true);
  void nodeSelectionClickAct_onToggled(bool checked);
  void imageZoomInAct_onTrigged();
  void imageZoomOutAct_onTrigged();

  void treeVis_onNodeSkeletonAssociation(int numberOfNodes);

  void areaAct_onTriggered(bool checked);
  void perimeterAct_onTriggered(bool checked);
  void volumeAct_onTriggered(bool checked);
  void circularityAct_onTriggered(bool checked);
  void complexityAct_onTriggered(bool checked);
  void nskelptAct_onTriggered(bool checked);

  void nodeHighlightAct_onTriggered(bool checked);

  void gradientGNodeStyleAct_onTriggered(bool checked);
  void fixedColorGNodeStyleAct_onTriggered(bool checked);

private:
  void createMenus();  
  void createTreeAttributeVisualitionMenus();
  void createTreeVisualiserMenus();
  void uncheckAttrVisActs();

  void createToolBar();
  
  void showProgressBar();

private:
  MainWidget *mainWidget_;
  QDockWidget *dockTreeVis_;
  
  QAction *showTreeVisAct_;
  QAction *nodeSelectionClickAct_;
  QAction *imageZoomInAct_;
  QAction *imageZoomOutAct_;
  QAction *nodeHighlightAct_;

  QAction *dmdProcessAct_;

  LabelWithProgressBar *progressBar_;
  bool shouldUpdateProgressBar_;

  QMenu *attrVisMenu_;
  QAction *areaAct_;
  QAction *perimeterAct_;
  QAction *volumeAct_;
  QAction *circularityAct_;
  QAction *complexityAct_;
  QAction *nskelptAct_;

  QMenu *treeVisMenu_;
  QAction *gradientGNodeStyleAct_;
  QAction *fixedColorGNodeStyleAct_;
    
};
