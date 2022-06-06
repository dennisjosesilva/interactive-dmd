#include "ManipulateCPs/AddHoverInfoPushButton.hpp"

AddHoverInfoPushButton::AddHoverInfoPushButton(const QString Disp_text,
const QIcon &icon, const QString &text, QWidget *parent)
: QPushButton{icon, text, parent},
displayedText{Disp_text}
{}

bool AddHoverInfoPushButton::event(QEvent *event)
{
	switch (event->type())
	{
	case QEvent::HoverEnter:
		hoverEnter(static_cast<QHoverEvent*>(event));
		return true;
		break;
	default:
		break;
	}
	return QWidget::event(event);
}

void AddHoverInfoPushButton::hoverEnter(QHoverEvent *event)
{
	setToolTip(displayedText);
	setStyleSheet("QPushButton::hover{background:rgb(150,150,150);}");
}
