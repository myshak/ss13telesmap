#include "mapview.h"

#include <QtGlobal>
#include <QWheelEvent>

MapView::MapView(QWidget *parent) :
    QGraphicsView(parent),
    m_currentZoom(1.0f),
    m_minZoom(1.0f),
    m_maxZoom(2.0f)
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void MapView::wheelEvent(QWheelEvent* event)
{
    // Get the number of steps of the wheel
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    // Do nothing if the user is trying to zoom past the limits
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

    // Reset the zoom, so we can specify an absolute scale factor instead of relative to the current scale
    resetMatrix();
    scale(m_currentZoom, m_currentZoom);

    event->accept();
}

void MapView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    calculateZoom();

    if(m_currentZoom < m_minZoom) {
        m_currentZoom = m_minZoom;
    }

    resetMatrix();
    scale(m_currentZoom, m_currentZoom);
}

void MapView::calculateZoom()
{
    if(scene()) {
        int max_size = height();

        // The minimum zoom should give us no empty space outside the scene
        if(max_size < width()) {
            max_size = width();
            m_minZoom = static_cast<float>(max_size)/scene()->width();
        } else {
            m_minZoom = static_cast<float>(max_size)/scene()->height();
        }
    }else{
        m_minZoom = 1.0f;
    }

}

void MapView::resetZoom()
{
    m_currentZoom = 1.0f;
    resetMatrix();
}
