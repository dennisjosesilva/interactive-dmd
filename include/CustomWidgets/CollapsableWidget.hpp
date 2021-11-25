#pragma once 

#include <QWidget>

class QPushButton;
class QLayout;

class CollapsableWidget : QWidget
{
  Q_OBJECT
  
public:
  enum class ExpandableStatus { Expanded, Collapsed };

  CollapsableWidget(const QString &title,
    QWidget *mainWidget, QWidget *parent=nullptr);

  inline QWidget *mainWidget() { mainWidget_; }
  void setMainWidget(QWidget *mainWidget);


protected:
  void expandableBtn_onTrigged();
  
private:
  QPushButton *expandableBtn_;
  QWidget *mainWidget_;
  QLayout *layout_;
};