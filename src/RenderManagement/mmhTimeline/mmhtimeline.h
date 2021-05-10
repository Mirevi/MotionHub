#ifndef MMHTIMELINE_H
#define MMHTIMELINE_H

#include <QWidget>

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

    float m_percentage = 0;
    float m_maxValue = 100;

    Ui::mmhTimeline *ui;
};
#endif // MMHTIMELINE_H
