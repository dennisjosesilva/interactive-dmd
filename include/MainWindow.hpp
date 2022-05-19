#pragma once 

#include <QMainWindow>
#include "MainWidget.hpp"
#include "CustomWidgets/LabelWithProgressBar.hpp"

#include <QList>


class QStackedWidget;
class QDockWidget;
class QAction;
class QProgressBar;
class QComboBox;

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
  void mtreeTypeComboBox_onIndexChanged(int index);
  void nodeSelectionClickAct_onToggled(bool checked);  
  void syncTreeAct_onTrigged();
  void imageZoomInAct_onTrigged();
  void imageZoomOutAct_onTrigged();

  void treeVis_onNodeSkeletonAssociation(int numberOfNodes);

  void showAreaFilteringDialog();

  void areaAct_onTriggered(bool checked);
  void perimeterAct_onTriggered(bool checked);
  void volumeAct_onTriggered(bool checked);
  void circularityAct_onTriggered(bool checked);
  void complexityAct_onTriggered(bool checked);
  void nskelptAct_onTriggered(bool checked);

  void nodeHighlightAct_onTriggered(bool checked);
  void ChangeCentralWidget_(CpViewer *cv);
  
private:
  void createMenus();  
  void createTreeFilteringMenu();
  void createTreeAttributeVisualitionMenus();  
  void uncheckAttrVisActs();

  void createToolBar();
  
  void showProgressBar();

private:
  QStackedWidget *centralWidget_;
  MainWidget *mainWidget_;
  QDockWidget *dockTreeVis_;
  
  QAction *showTreeVisAct_;
  QComboBox *mtreeTypeComboBox_;
  QAction *nodeSelectionClickAct_;
  QAction *imageZoomInAct_;
  QAction *imageZoomOutAct_;
  QAction *nodeHighlightAct_;
  QAction *syncTreeAct_;

  QAction *dmdProcessAct_;

  LabelWithProgressBar *progressBar_;
  bool shouldUpdateProgressBar_;

  QMenu *preprocessImageMenu_;
  QAction *areaFilteringAct_;

  QMenu *attrVisMenu_;
  QAction *areaAct_;
  QAction *perimeterAct_;
  QAction *volumeAct_;
  QAction *circularityAct_;
  QAction *complexityAct_;
  QAction *nskelptAct_;  
};
