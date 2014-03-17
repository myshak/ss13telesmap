#include "mapview.h"

#include <QtGlobal>
#include <QWheelEvent>

MapView::MapView(QWidget *parent) :
    QGraphicsView(parent),
    m_currentZoom(1.0f),
    m_minZoom(1.0f),
    m_maxZoom(2.0f)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void MapView::wheelEvent(QWheelEvent* event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    if((m_currentZoom <= m_minZoom && numSteps <= 0) || (m_currentZoom >= m_maxZoom && numSteps >= 0)) {
        return;
    }

    m_currentZoom += 0.25 * numSteps;

    if(m_currentZoom <= m_minZoom) {
        m_currentZoom = m_minZoom;
    }

    if(m_currentZoom >= m_maxZoom) {
        m_currentZoom = m_maxZoom;
    }

    resetMatrix();
    scale(m_currentZoom, m_currentZoom);

    event->accept();
}

void MapView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    calculateZoom();
    resetMatrix();
    scale(m_currentZoom, m_currentZoom);
}

void MapView::calculateZoom()
{
    if(scene()) {
        m_minZoom = 1.0f;
        int max_size = height();

        if(max_size < width()) {
            max_size = width();
            m_minZoom = static_cast<float>(max_size)/scene()->width();
        } else {
            m_minZoom = static_cast<float>(max_size)/scene()->height();
        }


    }
}
