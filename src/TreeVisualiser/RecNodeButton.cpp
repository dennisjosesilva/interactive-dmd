# include "TreeVisualiser/RecNodeButton.hpp"

#include <QPushButton>
#include <QHBoxLayout>

RecNodeButton::RecNodeButton(const QIcon &unselectedIcon, const QIcon &monoDockIcon,
  const QIcon &multiDockIcon)
  : icons_{unselectedIcon, monoDockIcon, multiDockIcon},
    mode_{Mode::Unselected}
{
  QLayout *layout = new QHBoxLayout;

  button_ = new QPushButton(icons_[static_cast<int>(mode_)], tr(""), this);
  button_->setIconSize(QSize(32, 32));
  connect(button_, &QPushButton::clicked, this, &RecNodeButton::onButton_click);
  layout->addWidget(button_);
  
  setLayout(layout);
}

void RecNodeButton::onButton_click()
{
  mode_ = static_cast<Mode>((static_cast<int>(mode_) + 1) % 3);
  button_->setIcon(icons_[static_cast<int>(mode_)]);
}