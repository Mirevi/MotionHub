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

void mmhTimeline::resizeEvent(QResizeEvent* event)
{
    updateTimelinePosition();

   QWidget::resizeEvent(event);
}
