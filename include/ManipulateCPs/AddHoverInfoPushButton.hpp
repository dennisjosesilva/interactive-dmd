#pragma once 

#include <QPushButton>
#include <QHoverEvent>

class AddHoverInfoPushButton : public QPushButton
{
  Q_OBJECT
public:
  AddHoverInfoPushButton(const QString Disp_text,
  const QIcon &icon, const QString &text, QWidget *parent);

protected:
  void hoverEnter(QHoverEvent *event);
  bool event(QEvent *event);

private:
  const QString displayedText;
};
