// IMPLEMENTATION BASED ON https://stackoverflow.com/questions/53135674/how-to-create-a-vertical-rotated-button-in-qt-with-c

#include "CustomWidgets/OrientablePushButton.hpp"
#include <QPainter>
#include <QStyleOptionButton>
#include <QStylePainter>

OrientablePushButton::OrientablePushButton(QWidget *parent)
  :QPushButton{parent}
{}

OrientablePushButton::OrientablePushButton(const QString &title,
  QWidget *parent)
  : QPushButton{title, parent}
{}

OrientablePushButton::OrientablePushButton(const QIcon &icon, const QString &title,
  QWidget *parent)
  : QPushButton{icon, title, parent}
{}

QSize OrientablePushButton::sizeHint() const 
{
  QSize sh = QPushButton::sizeHint();

  if (orientation_ != OrientablePushButton::Horizontal) {
    sh.transpose();
  }

  return sh;
}

void OrientablePushButton::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QStylePainter painter(this);
  QStyleOptionButton option;
  initStyleOption(&option);

  if (orientation_ == OrientablePushButton::VerticalTopToBottom) {
    painter.rotate(90);
    painter.translate(0, -width());
    option.rect = option.rect.transposed();
  }
  else if (orientation_ == OrientablePushButton::VerticalBottomToTop) {
    painter.rotate(-90);
    painter.translate(-height(), 0);
    option.rect = option.rect.transposed();
  }

  painter.drawControl(QStyle::CE_PushButton, option);
}