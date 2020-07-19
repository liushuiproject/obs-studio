#pragma once

#include <QDialog>
#include <QTextEdit>
#include "obs-app.hpp"

class OBSLogViewer : public QDialog {
	Q_OBJECT

private:
	OBSSignal logUpdateSignal;
	static void OBSLogUpdated(void *param, calldata_t *data);

	QPointer<QTextEdit> textArea;

	void InitLog();

private slots:
	void AddLine(int type, const char *text);
	void ClearText();
	void ToggleShowStartup(bool checked);

public:
	OBSLogViewer(QWidget *parent = 0);
	~OBSLogViewer();

	bool ShowOnStartup();
};
