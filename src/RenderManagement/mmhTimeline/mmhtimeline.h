#ifndef MMHTIMELINE_H
#define MMHTIMELINE_H

#include <QWidget>
#include <QFrame>

#include "MotionHubUtil/Console.h"

QT_BEGIN_NAMESPACE
namespace Ui { class mmhTimeline; }
QT_END_NAMESPACE

class  mmhTimeline : public QWidget
{
    Q_OBJECT

public:
    mmhTimeline(QWidget *parent = nullptr);
    ~mmhTimeline();

    void setValue(float newValue);
    void setMaxValue(float newMax);
    float getValue();
    float getMaxValue();

    void setMaxValue(int maxValue);
    int getMaxValue();

protected:
    void mouseMoveEvent(    QMouseEvent * event) override;
    void mousePressEvent(   QMouseEvent * event) override;
    void mouseReleaseEvent( QMouseEvent * event) override;
    void resizeEvent(       QResizeEvent* event) override;

signals:
    void timelinePressed(   float value);
    void timelineReleased(  float value);
    void timelineMoved(     float value);




private:
    void updateTimelinePosition();
    void updateHelperLines();
    void updateFrame();


    void clearHelperLines();



    float m_percentage = 0;
    float m_maxValue = 100;

    int m_maxValue = 500;
    int m_helperDistance = 100;

    std::vector<QFrame*> m_helpLines;


    Ui::mmhTimeline *ui;
};
#endif // MMHTIMELINE_H
