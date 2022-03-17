#pragma once

#include "TabView.h"
#include "QtCaptureDataView.h"

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
	private slots:
		void onTabChanged();
	private:
		void setCurrentTab(gui::GUITab* tab);

		Ui::QtTabView*ui;
	};
}