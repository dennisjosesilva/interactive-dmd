#pragma once 

#include <QMainWindow>
#include "MainWidget.hpp"

class MainWindow : public QMainWindow
{
  Q_OBJECT 
public:
  MainWindow();


private slots:
  void saveAs();
  void open();

private:
  void createMenus();


private:
  MainWidget *mainWidget_;
};