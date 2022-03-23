#pragma once

#include "GUITabView.h"
#include "QtCaptureDataView.h"
#include "QtDataAlreadyExistsDialog.h"

#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>
#include <QString>

namespace gui = facesynthesizing::domain::adapters::gui;

namespace Ui
{
	class QtTabView;
}

namespace facesynthesizing::infrastructure::qtgui{
	class QtTabView : public QDialog, public gui::TabView {
		Q_OBJECT

	public:
		explicit QtTabView(QWidget* parent = nullptr);
		~QtTabView();
		void addTab(QWidget* page, const QString& label);
		void changeListenerTriggered(util::ChangeObservable* source);
	signals:
		void updateLockedAsync();
		void updateCurrentTabAsync();
		void updateUserPromptsAsync();
	private slots:
		void updateLocked();
		void updateCurrentTab();
		void updateUserPrompts();

		void onTabChanged();
		void onUserPromtFinished();
	private:
		void connectSignalsAndSlots();
		void setCurrentTab(gui::GUITab* tab);

		std::shared_ptr<QtDataAlreadyExistsDialog> dataAlreadyExistsDialog;
		Ui::QtTabView*ui;
	};
}