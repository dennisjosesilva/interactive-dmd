#include "ManipulateCPs/ShowDegreeColorWidget.hpp"

ShowDegreeColorWidget::ShowDegreeColorWidget(int color_index, QWidget *parent)
: QWidget(parent),
  ColorIndex(color_index)
{}

void ShowDegreeColorWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPen pen;
	switch (ColorIndex)
	{
		case 1:
			pen.setColor(QColor(0,0,255,255));
			break;
		case 2:
			pen.setColor(QColor(0,255,255,255));
			break;
		case 3:
			pen.setColor(QColor(255,255,0,255));
			break;
		case 4:
			pen.setColor(QColor(255,0,0,255));
			break;
		default:
			pen.setColor(QColor(255,0,0,255));
			break;
	}
	//pen.setColor(QColor(255,0,0,255));
	pen.setWidth(3);
	QPainter painter(this);
	painter.setPen(pen);
	painter.drawLine(QPoint(0,8), QPoint(45,8));
}
