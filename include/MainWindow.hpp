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
  void dmdProcessAct_onTrigged();

private:
  void createMenus();
  void createToolBar();

private:
  MainWidget *mainWidget_;
  QDockWidget *dockTreeVis_;
  QAction *showTreeVisAct_;

//for DMD
  dmdProcess *dmd;
  QAction *dmdProcessAct_;
};