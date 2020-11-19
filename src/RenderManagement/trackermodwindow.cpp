#include "trackermodwindow.h"
#include "ui_trackermodwindow.h"

TrackerModWindow::TrackerModWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrackerModWindow)
{
    ui->setupUi(this);
}

TrackerModWindow::~TrackerModWindow()
{
    delete ui;
}
