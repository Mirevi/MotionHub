#include "QtTabView.h"
#include "ui_QtTabView.h"

#include<iostream>

namespace facesynthesizing::infrastructure::qtgui{
    QtTabView::QtTabView(QWidget* parent) : QDialog(parent), ui(new Ui::QtTabView) {
        ui->setupUi(this);
        connectSignalsAndSlots();
    }
    void QtTabView::connectSignalsAndSlots()
    {
        QObject::connect(this, &QtTabView::updateLockedAsync,
            this, &QtTabView::updateLocked, Qt::QueuedConnection);
        QObject::connect(this, &QtTabView::updateCurrentTabAsync,
            this, &QtTabView::updateCurrentTab, Qt::QueuedConnection);
        QObject::connect(this, &QtTabView::updateUserPromptsAsync,
            this, &QtTabView::updateUserPrompts, Qt::QueuedConnection);
    }
    QtTabView::~QtTabView() {
        delete ui;
    }
    void QtTabView::addTab(QWidget* page, const QString& label)
    {
        this->ui->tabWidget->addTab(page, label);
    }

    void QtTabView::changeListenerTriggered(util::ChangeObservable* source)
    {       
        if (source == tabViewModel.get()) {
            updateLockedAsync();
            updateCurrentTabAsync();
            updateUserPromptsAsync();
        }
    }
    void QtTabView::updateLocked()
    {
        int currentIndex = ui->tabWidget->currentIndex();
        ui->tabWidget->setTabEnabled(currentIndex, true);
        for (int i = 0; i < ui->tabWidget->count(); i++) {
            if (i != currentIndex)
                ui->tabWidget->setTabEnabled(i, !tabViewModel->isLocked);
        }
    }
    void QtTabView::updateCurrentTab()
    {
        switch (tabViewModel->currentTab) {
        case gui::GUITabType::CAPTURE:
            setCurrentTab(captureDataView.get());
            break;
        case gui::GUITabType::CONVERT:
            setCurrentTab(convertDataView.get());
            break;
        case gui::GUITabType::TRAINING:
            //setCurrentTab(trainingView.get());
            break;
        case gui::GUITabType::INFERENCE:
            //setCurrentTab(inferenceView.get());
            break;
        default:
            throw "Unknown FaceSynthesizingGUITabType.";
        }
    }
    void QtTabView::setCurrentTab(gui::GUITab* tab)
    {
        auto currentWidget = dynamic_cast<QWidget*>(tab);
        ui->tabWidget->setCurrentWidget(currentWidget);
    }
    void QtTabView::updateUserPrompts()
    {
        if (tabViewModel->showDataAlreadyExistsUserPrompt) {
            std::cout << "showDataAlreadyExistsUserPrompt" << std::endl;
            dataAlreadyExistsDialog = std::make_shared<QtDataAlreadyExistsDialog>();
            dataAlreadyExistsDialog->setMessage(tabViewModel->dialogMessage);
            QObject::connect(dataAlreadyExistsDialog.get(), &QtDataAlreadyExistsDialog::finished,
                this, &QtTabView::onUserPromtFinished, Qt::AutoConnection);

            dataAlreadyExistsDialog->setModal(true);
            dataAlreadyExistsDialog->open();
            std::cout << "showDataAlreadyExistsUserPrompt finished" << std::endl;
        }
        else {
            dataAlreadyExistsDialog = nullptr;
        }
    }

    void QtTabView::onTabChanged() {
        gui::GUITab* newTab = dynamic_cast<gui::GUITab*>(ui->tabWidget->currentWidget());
        tabViewModel->currentTab = newTab->getTabType();
        tabViewModel->notify();
    }
    void QtTabView::onUserPromtFinished()
    {
        if (tabViewModel->showDataAlreadyExistsUserPrompt) {
            tabViewModel->showDataAlreadyExistsUserPrompt = false;
            QtDataAlreadyExistsDialogResult result = dataAlreadyExistsDialog->getResult();

            switch (result) {
            case QtDataAlreadyExistsDialogResult::Overwrite:
                guiEventForwarder->dataAlreadySetPromtDone(usecases::DataAlreadyExistsResult::Overwrite);
                break;
            case QtDataAlreadyExistsDialogResult::Continue:
                guiEventForwarder->dataAlreadySetPromtDone(usecases::DataAlreadyExistsResult::Continue);
                break;
            case QtDataAlreadyExistsDialogResult::Cancel:
                guiEventForwarder->cancel();
                break;
            }
        }
        tabViewModel->notify();
    }
}