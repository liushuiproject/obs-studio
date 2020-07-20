#include <QtWidgets/QStyleOptionSlider>
#include "slider-ignorewheel.hpp"

SliderIgnoreScroll::SliderIgnoreScroll(QWidget *parent) : QSlider(parent)
{
	setFocusPolicy(Qt::StrongFocus);
}

SliderIgnoreScroll::SliderIgnoreScroll(Qt::Orientation orientation,
				       QWidget *parent)
	: QSlider(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	setOrientation(orientation);
}

void SliderIgnoreScroll::wheelEvent(QWheelEvent *event)
{
	if (!hasFocus())
		event->ignore();
	else
		QSlider::wheelEvent(event);
}

void SliderIgnoreScroll::mouseDoubleClickEvent(QMouseEvent *event) {
        QStyleOptionSlider opt;
        this->initStyleOption(&opt);
        QRect sr = this->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        if (sr.contains(event->pos())) {
		emit sliderHandleDoubleClicked();
		return;
        }
        QSlider::mouseDoubleClickEvent(event);
}