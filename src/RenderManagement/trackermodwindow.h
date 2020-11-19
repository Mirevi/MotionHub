#ifndef TRACKERMODWINDOW_H
#define TRACKERMODWINDOW_H

#include <QtWidgets/QDialog>

namespace Ui
{
	class TrackerModWindow;
}

class TrackerModWindow : public QDialog
{
	Q_OBJECT

public:
	explicit TrackerModWindow(QWidget *parent = nullptr);
	~TrackerModWindow();

private:
	Ui::TrackerModWindow *ui;
};

#endif // TRACKERMODWINDOW_H
