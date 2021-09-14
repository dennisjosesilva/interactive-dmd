#pragma once 

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QString>


class LabelWithProgressBar : public QWidget
{
Q_OBJECT

public:
  LabelWithProgressBar(QWidget *parent=nullptr);

  inline void setProgressBarRange(int min, int max) { progressBar_->setRange(min, max); }
  inline void setLabelText(const QString &text) { label_->setText(text); } 
  inline int maximum() const { return progressBar_->maximum(); }
  inline int minimum() const { return progressBar_->minimum(); }
  void setValue(int val);

  inline bool isReachMaximumProgress() const { return progressBar_->value() == progressBar_->maximum(); }

public:
 signals:
  void fullProgressBar();

private:
  QLabel *label_;
  QProgressBar *progressBar_;  
};  