#pragma once 

#include <QWidget>

class OrientablePushButton;
class QLayout;

class CollapsableWidget : public QWidget
{
  Q_OBJECT
  
public:
  enum Status { Expanded, Collapsed };

  CollapsableWidget(const QString &title,
    QWidget *mainWidget, QWidget *parent=nullptr);

  inline QWidget *mainWidget() { return mainWidget_; }
  void setMainWidget(QWidget *mainWidget);

  inline Status status() const { return status_; }

protected:
  void expandableBtn_onTrigged();
  
private:
  OrientablePushButton *expandableBtn_;
  QWidget *mainWidget_;
  QLayout *layout_;

  Status status_;
};