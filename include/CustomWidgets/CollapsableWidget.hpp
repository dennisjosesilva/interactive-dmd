#pragma once 

#include <QWidget>
#include <QFrame>

class OrientablePushButton;
class QLayout;

class CollapsableMainWidget : public QFrame 
{
public:
  CollapsableMainWidget(QWidget *parent=nullptr):QFrame{parent} {}

  virtual void refresh() = 0;
};


class CollapsableWidget : public QWidget
{
  Q_OBJECT
public:
  enum Status { Expanded, Collapsed };

  CollapsableWidget(const QString &title,
    CollapsableMainWidget *mainWidget, QWidget *parent=nullptr);

  inline QWidget *mainWidget() { return mainWidget_; }
  inline void refresh() { mainWidget_->refresh(); }

  void collapse();
  void expand();

  inline Status status() const { return status_; }

protected:
  void expandableBtn_onTrigged();
  
private:
  OrientablePushButton *expandableBtn_;
  CollapsableMainWidget *mainWidget_;
  QLayout *layout_;

  Status status_;
};