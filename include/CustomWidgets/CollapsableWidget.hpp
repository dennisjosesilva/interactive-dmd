#pragma once 

#include <QWidget>

class OrientablePushButton;
class QLayout;

class CollapsableWidget : public QWidget
{
  Q_OBJECT
  
public:
  enum class ExpandableStatus { Expanded, Collapsed };

  CollapsableWidget(const QString &title,
    QWidget *mainWidget, QWidget *parent=nullptr);

  inline QWidget *mainWidget() { return mainWidget_; }
  void setMainWidget(QWidget *mainWidget);


protected:
  void expandableBtn_onTrigged();
  
private:
  OrientablePushButton *expandableBtn_;
  QWidget *mainWidget_;
  QLayout *layout_;
};