#pragma once 

#include <QWidget>
#include <QPen>
#include <QPainter>

class ShowDegreeColorWidget : public QWidget
{
  Q_OBJECT
public:
  ShowDegreeColorWidget(int color_index, QWidget *parent);

protected:
  void paintEvent(QPaintEvent *event);

private:
  int ColorIndex;
};
