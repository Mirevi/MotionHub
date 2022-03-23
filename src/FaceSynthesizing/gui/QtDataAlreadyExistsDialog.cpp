#include "QtDataAlreadyExistsDialog.h"
#include "ui_QtDataAlreadyExistsDialog.h"

#include<iostream>

namespace facesynthesizing::infrastructure::qtgui {
	QtDataAlreadyExistsDialog::QtDataAlreadyExistsDialog(QWidget* parent) : QDialog(parent), 
		ui(new Ui::QtDataAlreadyExistsDialog) {
		ui->setupUi(this);
		isInitialized = true;
	}
	QtDataAlreadyExistsDialog::~QtDataAlreadyExistsDialog()
	{
		delete ui;
	}
	void QtDataAlreadyExistsDialog::setMessage(std::string message)
	{
		ui->messageLabel->setText(QString::fromStdString(message));
	}
	QtDataAlreadyExistsDialogResult QtDataAlreadyExistsDialog::getResult()
	{
		return result;
	}
	void QtDataAlreadyExistsDialog::onOverwrite()
	{
		result = QtDataAlreadyExistsDialogResult::Overwrite;
		this->done(100);
	}
	void QtDataAlreadyExistsDialog::onContinue()
	{
		result = QtDataAlreadyExistsDialogResult::Continue;
		this->done(100);
	}
	void QtDataAlreadyExistsDialog::onCancel()
	{
		result = QtDataAlreadyExistsDialogResult::Cancel;
		this->done(100);
	}
}