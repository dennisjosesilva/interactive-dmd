#pragma once 

#include <QMainWindow>
#include "MainWidget.hpp"
#include "dmdProcess.hpp"

class QDockWidget;
class QAction;


class MainWindow : public QMainWindow
{
  Q_OBJECT 
public:
  MainWindow();


private slots:
  void saveAs();
  void open();

  void treeVisAct_onToggled(bool checked=true);
  void nodeSelectionClickAct_onToggled(bool checked);
  void imageZoomInAct_onTrigged();
  void imageZoomOutAct_onTrigged();

private:
  void createMenus();
  void createToolBar();

private:
  MainWidget *mainWidget_;
  QDockWidget *dockTreeVis_;
  
  QAction *showTreeVisAct_;
  QAction *nodeSelectionClickAct_;
  QAction *imageZoomInAct_;
  QAction *imageZoomOutAct_;
};
