#pragma once 

#include <QMainWindow>
#include "MainWidget.hpp"
#include "dmdProcess.hpp"
#include "CustomWidgets/LabelWithProgressBar.hpp"

class QDockWidget;
class QAction;
class QProgressBar;

class MainWindow : public QMainWindow
{
  Q_OBJECT 
public:
  MainWindow();

  void setMinMaxProgressBar(int min, int max);

private slots:
  void saveAs();
  void open();

  void dmdProcessAct_onTrigged();

  void treeVisAct_onToggled(bool checked=true);
  void nodeSelectionClickAct_onToggled(bool checked);
  void imageZoomInAct_onTrigged();
  void imageZoomOutAct_onTrigged();

  void treeVis_onNodeSkeletonAssociation(int numberOfNodes);

  

private:
  void createMenus();
  void createToolBar();

  void showProgressBar();

private:
  MainWidget *mainWidget_;
  QDockWidget *dockTreeVis_;
  
  QAction *showTreeVisAct_;
  QAction *nodeSelectionClickAct_;
  QAction *imageZoomInAct_;
  QAction *imageZoomOutAct_;

// for dmd
  dmdProcess *dmd;
  QAction *dmdProcessAct_;

  LabelWithProgressBar *progressBar_;
};
