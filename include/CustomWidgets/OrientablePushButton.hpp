#pragma once

// IMPLEMENTATION BASED ON https://stackoverflow.com/questions/53135674/how-to-create-a-vertical-rotated-button-in-qt-with-c


#include <QPushButton>

class OrientablePushButton : public QPushButton
{
  Q_OBJECT
public:
  enum Orientation {
    Horizontal,
    VerticalTopToBottom,
    VerticalBottomToTop
  };

  OrientablePushButton(QWidget *parent = nullptr);
  OrientablePushButton(const QString &text, QWidget *parent=nullptr);
  OrientablePushButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr);

  QSize sizeHint() const;

  inline Orientation orientation() const { return orientation_; }
  inline void setOrientation(Orientation orientation) { orientation_ = orientation; }

protected:
  void paintEvent(QPaintEvent *event);


private:
  Orientation orientation_ = Horizontal;
};