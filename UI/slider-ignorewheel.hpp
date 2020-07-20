#pragma once

#include <QSlider>
#include <QInputEvent>
#include <QtCore/QObject>

class SliderIgnoreScroll : public QSlider {
	Q_OBJECT

public:
	SliderIgnoreScroll(QWidget *parent = nullptr);
	SliderIgnoreScroll(Qt::Orientation orientation,
			   QWidget *parent = nullptr);

signals:
	virtual void sliderHandleDoubleClicked();

protected:
	virtual void wheelEvent(QWheelEvent *event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};
