#include "MainWindow.hpp"

#include <QStatusBar>

MainWindow::MainWindow()
{
  setWindowTitle("Interactive DMD");  

  statusBar()->showMessage(tr("Ready"));
}