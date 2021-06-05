#include "mmhtimeline.h"
#include "ui_mmhtimeline.h"
#include <QMouseEvent>
#include <iostream>
#include <algorithm>


mmhTimeline::mmhTimeline(QWidget *parent) : QWidget(parent), ui(new Ui::mmhTimeline)
{
    ui->setupUi(this);

}

mmhTimeline::~mmhTimeline()
{
    delete ui;
}



void mmhTimeline::setValue(float newValue)
{
    m_percentage = newValue;
    updateTimelinePosition();
}

float mmhTimeline::getValue()
{
    return m_percentage;
}

void mmhTimeline::setMaxValue(int maxValue)
{
    m_maxValue = maxValue;

    updateHelperLines();

}

int mmhTimeline::getMaxValue()
{
    return m_maxValue;
}



void mmhTimeline::mouseMoveEvent(QMouseEvent *event)
{

    m_percentage = event->pos().x() * 100.0f / geometry().width();
    m_percentage = std::clamp(m_percentage, 0.0f, 100.0f);

    emit timelineMoved(m_percentage);

    updateTimelinePosition();
    QWidget::mouseMoveEvent(event);
}

void mmhTimeline::mousePressEvent(QMouseEvent *event)
{
    m_percentage = event->pos().x() * 100.0f / geometry().width();
    m_percentage = std::clamp(m_percentage, 0.0f, 100.0f);


    emit timelinePressed(m_percentage);


    updateTimelinePosition();
    QWidget::mousePressEvent(event);
}

void mmhTimeline::mouseReleaseEvent(QMouseEvent *event)
{
    m_percentage = event->pos().x() * 100.0f / geometry().width();
    m_percentage = std::clamp(m_percentage, 0.0f, 100.0f);

    emit timelineReleased(m_percentage);

    updateTimelinePosition();
    QWidget::mouseReleaseEvent(event);
}



void mmhTimeline::updateTimelinePosition()
{
    ui->cursor_line->setGeometry(m_percentage * (geometry().width() -5) / 100.0f, 0, 5, geometry().height());
}

void mmhTimeline::updateHelperLines()
{    
    
    //remove all old helper lines
    clearHelperLines();

    //calculate number of helper lines
    int numOfHelper = floor(m_maxValue / m_helperDistance);

    //if last line is exactly on the edge, remove it
    if (m_maxValue % m_helperDistance == 0)
    {
        numOfHelper--;
    }

    //create helper lines
    for (size_t i = 0; i < numOfHelper; i++)
    {
        //create line object
        QFrame* currHelpLine = new QFrame(this);

        //set helper to background
        currHelpLine->lower();

        //set correct name
        std::string currName = "helpLine_" + std::to_string(i);
        currHelpLine->setObjectName(QString::fromUtf8(currName.c_str()));

        //calc current helper position
        int xPos = (i + 1) * (geometry().width() - 2) * m_helperDistance / m_maxValue;

        //set size and position
        currHelpLine->setGeometry(xPos, 0, 1, geometry().height());

        //add current helper to pool, so iit can be deleted later
        m_helpLines.push_back(currHelpLine);

        //no visibility without this
        currHelpLine->show();

    }
}

void mmhTimeline::updateFrame()
{

    ui->frame_top->setGeometry(0, 0, geometry().width(), 2);
    ui->frame_bot->setGeometry(0, geometry().height() - 2, geometry().width(), 2);
    ui->frame_left->setGeometry(0, 0, 2, geometry().height());
    ui->frame_right->setGeometry(geometry().width() - 2, 0, 2, geometry().height());



}

void mmhTimeline::clearHelperLines()
{

    for (size_t i = 0; i < m_helpLines.size(); i++)
    {
        delete m_helpLines[i];
    }

    m_helpLines.clear();
}

void mmhTimeline::resizeEvent(QResizeEvent* event)
{
    updateTimelinePosition();

    updateHelperLines();

    updateFrame();

   QWidget::resizeEvent(event);
}