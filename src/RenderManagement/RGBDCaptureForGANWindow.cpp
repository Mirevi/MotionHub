#include "RGBDCaptureForGANWindow.h"
#include "ui_RGBDCaptureForGANWindow.h"

RGBDCaptureForGANWindow::RGBDCaptureForGANWindow(ConfigManager* configManager, QWidget* parent) : QDialog(parent), ui(new Ui::RGBDCaptureForGANWindow)
{
	ui->setupUi(this);

	m_configManager = configManager;

}



RGBDCaptureForGANWindow::~RGBDCaptureForGANWindow()
{

	delete ui;

}