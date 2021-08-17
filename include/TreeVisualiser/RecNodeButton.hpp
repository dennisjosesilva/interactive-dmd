# pragma once 

#include <QWidget>
#include <QString>
#include <QIcon>

class QPushButton;

class RecNodeButton : public QWidget
{

Q_OBJECT

public:
  enum class Mode{ Unselected = 0, MonoDock = 1, MultiDock = 2 };

  RecNodeButton(const QIcon &unselectedIcon, const QIcon &monoDockIcon,
    const QIcon &multiDockIcon);

  inline Mode mode() const { return mode_; }
  inline void setMode(Mode mode) { mode_ = mode; }

protected slots:
  void onButton_click();

private:  
  Mode mode_;
  QIcon icons_[3];
  QPushButton *button_;
};