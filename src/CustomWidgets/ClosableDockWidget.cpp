#include "CustomWidgets/ClosableDockWidget.hpp"

ClosableDockWidget::ClosableDockWidget(const QString &title, 
  QWidget *mainWindow)
  : QDockWidget{title, mainWindow}
{}

void ClosableDockWidget::closeEvent(QCloseEvent *e)
{
  emit closed(this);
}