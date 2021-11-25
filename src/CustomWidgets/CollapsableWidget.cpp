#include "CustomWidgets/CollapsableWidget.hpp"

#include <QPushButton>

CollapsableWidget::CollapsableWidget(const QString &title, QWidget *mainWidget, 
  QWidget *parent)
  :QWidget{parent},
   mainWidget_{mainWidget}
{
  expandableBtn_ = new QPushButton{title, this};
  connect(expandableBtn_, &QPushButton::clicked, this, 
    &CollapsableWidget::expandableBtn_onTrigged);
}


void CollapsableWidget::expandableBtn_onTrigged()
{  
}
