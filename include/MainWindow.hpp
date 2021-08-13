#pragma once 

#include <QMainWindow>
#include "MainWidget.hpp"

class QDockWidget;


class MainWindow : public QMainWindow
{
  Q_OBJECT 
public:
  MainWindow();


private slots:
  void saveAs();
  void open();

  void showTreeVisualiser();

private:
  void createMenus();
  void createToolBar();

private:
  MainWidget *mainWidget_;
  QDockWidget *dockTreeVis_;
};