#pragma once 

#include <QDockWidget>

class ClosableDockWidget : public QDockWidget
{
Q_OBJECT

public:
  ClosableDockWidget(const QString &title, QWidget *mainWindow);

  void closeEvent(QCloseEvent *e) override;

public:
signals:
  void closed(ClosableDockWidget *dock);
};