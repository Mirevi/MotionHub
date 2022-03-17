#include "QtTabView.h"
#include "ui_QtTabView.h"

#include<iostream>

namespace facesynthesizing::infrastructure::qtgui{
    QtTabView::QtTabView(QWidget* parent) : QDialog(parent), ui(new Ui::QtTabView) {
        ui->setupUi(this);
    }

    QtTabView::~QtTabView() {
        delete ui;
        this->tabViewModel->detachListener(this);
    }

    void QtTabView::addTab(QWidget* page, const QString& label)
    {
        this->ui->tabWidget->addTab(page, label);
    }

    void QtTabView::changeListenerTriggered(util::ChangeObservable* source)
    {       

        if (source == tabViewModel.get()) {
            // isLocked
            for (int i = 0; i < ui->tabWidget->count(); i++) {
                ui->tabWidget->setTabEnabled(i, !tabViewModel->isLocked);
            }
            int currentIndex = ui->tabWidget->currentIndex();
            ui->tabWidget->setTabEnabled(currentIndex, true);

            // currentTab
            switch (tabViewModel->currentTab) {
            case gui::FaceSynthesizingGUITabType::CAPTURE:
                setCurrentTab(captureDataView.get());
                break;
            case gui::FaceSynthesizingGUITabType::CONVERT:
                //setCurrentTab(convertDataView.get());
                break;
            case gui::FaceSynthesizingGUITabType::TRAIN:
                //setCurrentTab(trainView.get());
                break;
            case gui::FaceSynthesizingGUITabType::INFERENCE:
                //setCurrentTab(inferenceView.get());
                break;
            default:
                throw "Unknown FaceSynthesizingGUITabType.";
            }
        }
    }

    void QtTabView::setCurrentTab(gui::GUITab* tab)
    {
        auto currentWidget = dynamic_cast<QWidget*>(tab);
        ui->tabWidget->setCurrentWidget(currentWidget);
    }

    void QtTabView::onTabChanged() {
        gui::GUITab* newTab = dynamic_cast<gui::GUITab*>(ui->tabWidget->currentWidget());
        tabViewModel->currentTab = newTab->getTabType();
        tabViewModel->notify();
    }
}