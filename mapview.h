#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>

class MapView : public QGraphicsView
{
public:
    MapView(QWidget *parent = NULL);

    void calculateZoom();
    void resetZoom();

protected:
    virtual void wheelEvent(QWheelEvent* event);
    void resizeEvent(QResizeEvent *event);

private:
    qreal m_currentZoom;
    qreal m_minZoom;
    qreal m_maxZoom;
};

#endif // MAPVIEW_H
