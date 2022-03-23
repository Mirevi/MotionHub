#pragma once

#include <string>
#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>

namespace Ui
{
	class QtDataAlreadyExistsDialog;
}

namespace facesynthesizing::infrastructure::qtgui {
	enum class QtDataAlreadyExistsDialogResult {
		Overwrite, Continue, Cancel
	};

	class QtDataAlreadyExistsDialog : public QDialog {
		Q_OBJECT

	public:
		explicit QtDataAlreadyExistsDialog(QWidget* parent = nullptr);
		~QtDataAlreadyExistsDialog();
		void setMessage(std::string message);
		QtDataAlreadyExistsDialogResult getResult();

	private slots:
		void onOverwrite();
		void onContinue();
		void onCancel();
	private:
		QtDataAlreadyExistsDialogResult result = QtDataAlreadyExistsDialogResult::Cancel;
		bool isInitialized = false;
		Ui::QtDataAlreadyExistsDialog* ui;
	};
}