#include "CustomWidgets/CollapsableWidget.hpp"
#include "CustomWidgets/OrientablePushButton.hpp"
#include <QLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <QDebug>

CollapsableWidget::CollapsableWidget(const QString &title, QWidget *mainWidget, 
  QWidget *parent)
  :QWidget{parent},
   mainWidget_{mainWidget}
{
  expandableBtn_ = new OrientablePushButton{title, this};
  expandableBtn_->setOrientation(OrientablePushButton::VerticalTopToBottom);
  expandableBtn_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

  layout_ = new QHBoxLayout;
  layout_->addWidget(expandableBtn_);

  connect(expandableBtn_, &QPushButton::clicked, this, 
    &CollapsableWidget::expandableBtn_onTrigged);

  setLayout(layout_);
}


void CollapsableWidget::expandableBtn_onTrigged()
{  
  qDebug() << "Blah";
}
