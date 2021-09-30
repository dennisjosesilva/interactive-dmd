#include "CustomWidgets/TitleColorBar.hpp"

#include <QVBoxLayout>

TitleColorBar::TitleColorBar(ColorBar *colorBar, QWidget *parent)
  :QWidget{parent},
   colorBar_{colorBar}
{
  QLayout *layout = new QVBoxLayout;

  text_ = new QLabel{this};

  text_->setAlignment(Qt::AlignCenter);
  layout->addWidget(text_);
  layout->addWidget(colorBar_);

  setLayout(layout);
}