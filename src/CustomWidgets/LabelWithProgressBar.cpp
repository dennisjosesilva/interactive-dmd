#include "CustomWidgets/LabelWithProgressBar.hpp"

#include <QHBoxLayout>

LabelWithProgressBar::LabelWithProgressBar(QWidget *parent)
  :QWidget{parent}
{
  QLayout *layout = new QHBoxLayout;

  label_ = new QLabel{this};
  label_->setContentsMargins(0, 0, 5, 0);
  layout->addWidget(label_);
  
  progressBar_ = new QProgressBar{this};
  layout->addWidget(progressBar_);

  layout->setContentsMargins(0, 0, 15, 0);
  layout->setSpacing(0);

  setLayout(layout);
}

void LabelWithProgressBar::setValue(int val)
{
  progressBar_->setValue(val);
  if (isReachMaximumProgress()) {
    emit fullProgressBar();
  }
}