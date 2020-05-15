#include <QFile>
#include <QTextStream>
#include <QScrollBar>
#include <QFont>
#include <QFontDatabase>
#include <QPushButton>
#include <QCheckBox>
#include <QLayout>
#include <string>

#include "log-viewer.hpp"
#include "qt-wrappers.hpp"

void OBSLogViewer::OBSLogUpdated(void *param, calldata_t *data)
{
	OBSLogViewer *window = reinterpret_cast<OBSLogViewer *>(param);
	int type = calldata_int(data, "type");
	const char *text = calldata_string(data, "message");

	QMetaObject::invokeMethod(window, "AddLine", Q_ARG(int, type),
				  Q_ARG(const char *, text));
}

OBSLogViewer::OBSLogViewer(QWidget *parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	const QFont fixedFont =
		QFontDatabase::systemFont(QFontDatabase::FixedFont);

	textArea = new QTextEdit();
	textArea->setReadOnly(true);
	textArea->setFont(fixedFont);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	QPushButton *clearButton = new QPushButton(QTStr("Clear"));
	connect(clearButton, &QPushButton::clicked, this,
		&OBSLogViewer::ClearText);
	QPushButton *closeButton = new QPushButton(QTStr("Close"));
	connect(closeButton, &QPushButton::clicked, this, &QDialog::hide);

	QCheckBox *showStartup = new QCheckBox(QTStr("ShowOnStartup"));
	showStartup->setChecked(ShowOnStartup());
	connect(showStartup, SIGNAL(toggled(bool)), this,
		SLOT(ToggleShowStartup(bool)));

	buttonLayout->addSpacing(10);
	buttonLayout->addWidget(showStartup);
	buttonLayout->addStretch();
	buttonLayout->addWidget(clearButton);
	buttonLayout->addWidget(closeButton);
	buttonLayout->addSpacing(10);
	buttonLayout->setContentsMargins(0, 0, 0, 4);

	layout->addWidget(textArea);
	layout->addLayout(buttonLayout);
	setLayout(layout);

	setWindowTitle(QTStr("LogViewer"));
	resize(800, 300);

	const char *geom = config_get_string(App()->GlobalConfig(), "LogViewer",
					     "geometry");

	if (geom != nullptr) {
		QByteArray ba = QByteArray::fromBase64(QByteArray(geom));
		restoreGeometry(ba);
	}

	InitLog();

	logUpdateSignal.Connect(obs_get_signal_handler(), "log_updated",
				OBSLogUpdated, this);
}

OBSLogViewer::~OBSLogViewer()
{
	config_set_string(App()->GlobalConfig(), "LogViewer", "geometry",
			  saveGeometry().toBase64().constData());

	logUpdateSignal.Disconnect();
	deleteLater();
}

void OBSLogViewer::ToggleShowStartup(bool checked)
{
	config_set_bool(App()->GlobalConfig(), "LogViewer", "ShowLogStartup",
			checked);
}

bool OBSLogViewer::ShowOnStartup()
{
	return config_get_bool(App()->GlobalConfig(), "LogViewer",
			       "ShowLogStartup");
}

void OBSLogViewer::InitLog()
{
	char logDir[512];
	std::string path;

	if (GetConfigPath(logDir, sizeof(logDir), "obs-studio/logs")) {
		path += logDir;
		path += "/";
		path += App()->GetCurrentLog();
	}

	QFile file(QT_UTF8(path.c_str()));

	if (file.open(QIODevice::ReadOnly)) {
		QTextStream in(&file);

		while (!in.atEnd()) {
			QString line = in.readLine();
			AddLine(LOG_INFO, QT_TO_UTF8(line));
		}

		file.close();
	}
}

void OBSLogViewer::AddLine(int type, const char *text)
{
	QString str = QT_UTF8(text);
	QString msg = str.toHtmlEscaped();

	switch (type) {
	case LOG_WARNING:
		msg = "<font color=\"#c08000\">" + str.toHtmlEscaped() +
		      "</font>";
		break;
	case LOG_ERROR:
		msg = "<font color=\"#c00000\">" + str.toHtmlEscaped() +
		      "</font>";
		break;
	}

	QScrollBar *scroll = textArea->verticalScrollBar();
	bool bottomScrolled = scroll->value() >= scroll->maximum() - 10;

	if (bottomScrolled)
		scroll->setValue(scroll->maximum());

	textArea->insertHtml(msg + "<br>");

	if (bottomScrolled)
		scroll->setValue(scroll->maximum());
}

void OBSLogViewer::ClearText()
{
	textArea->clear();
}
