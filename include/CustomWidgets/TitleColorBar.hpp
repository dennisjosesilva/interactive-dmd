#pragma once 

#include <IcicleMorphotreeWidget/Graphics/ColorBar.hpp>

#include <QWidget>
#include <QLabel>

class TitleColorBar : public QWidget
{
  Q_OBJECT
public:
  using ColorBar = IcicleMorphotreeWidget::ColorBar; 

  TitleColorBar(ColorBar *colorBar, QWidget *parent=nullptr);

  inline float maxValue() const { return colorBar_->maxValue(); }
  inline float minValue() const { return colorBar_->minValue(); }
  inline void setMaxValue(float val) { colorBar_->setMaxValue(val); }
  inline void setMinValue(float val) { colorBar_->setMinValue(val); }

  inline bool showNumbers() const { return colorBar_->showNumbers(); }
  inline void setShowNumbers(bool show) { colorBar_->setShowNumbers(show); }

  void setTitle(const QString &title) { text_->setText(title); }

private:
  QLabel *text_;
  ColorBar *colorBar_;
};