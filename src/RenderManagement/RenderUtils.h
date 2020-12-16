#pragma once

#include <string>

#include "QtWidgets/qprogressdialog.h"
#include <QtWidgets/QMainWindow>



inline void startProgressBar(int maxValue, int* currentValue, std::string lable, QWidget* parentWidget)
{
	//create progress bar
	QProgressDialog progress(QString(lable.c_str()), "Cancel", 0, maxValue, parentWidget);
	progress.setWindowModality(Qt::WindowModal);

	//while still saving
	while (*currentValue < maxValue)
	{
		//update progress bar
		progress.setValue(*currentValue);

		if (progress.wasCanceled())
			break;
	}

	progress.setValue(*currentValue);
}