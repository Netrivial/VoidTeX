#include "pdfpreview.h"

#include <QPdfDocument>
#include <QPdfView>
#include <QVBoxLayout>
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QFileInfo>
#include <QDebug>

PdfPreview::PdfPreview(QWidget *parent)
    : QWidget(parent)
{
    m_document = new QPdfDocument(this);
    m_view = new QPdfView(this);
    m_view->setDocument(m_document);
    m_view->setPageMode(QPdfView::PageMode::MultiPage);
    m_view->setZoomMode(QPdfView::ZoomMode::Custom);
    m_view->setZoomFactor(1.0);
    m_view->viewport()->installEventFilter(this);
    m_view->setFrameShape(QFrame::NoFrame);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);

    // setFrameShape(QFrame::NoFrame);
}

void PdfPreview::loadPdf(const QString &path)
{
    if (path.isEmpty() || !QFileInfo::exists(path)) return;
    m_currentPath = path;
    m_document->load(path);
}

/* Do not use anymore:
void PdfPreview::clear()
{
    m_currentPath.clear();
    if (m_document) {
        m_view->setDocument(nullptr);
        m_document->deleteLater();
        m_document = nullptr;
    }
}
*/

void PdfPreview::onDocumentStatusChanged()
{
    if (!m_document) return;
    const auto status = m_document->status();
    switch (status) {
    case QPdfDocument::Status::Ready:
        qDebug() << "PDF loaded:" << m_currentPath
                 << "pages:" << m_document->pageCount();
        break;
    case QPdfDocument::Status::Error:
        qWarning() << "PDF load error:" << m_currentPath;
        break;
    default:
        break;
    }
}

bool PdfPreview::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_view->viewport() && event->type() == QEvent::Wheel) {
        auto *wheel = static_cast<QWheelEvent*>(event);

        if (wheel->modifiers() & Qt::ControlModifier) {
            auto *hBar = m_view->horizontalScrollBar();
            auto *vBar = m_view->verticalScrollBar();

            const qreal oldFactor = m_view->zoomFactor();
            const qreal step = wheel->angleDelta().y() > 0 ? 1.3 : 1.0 / 1.3;
            const qreal newFactor = qBound(0.25, oldFactor * step, 5.0);

            if (qFuzzyCompare(newFactor, oldFactor))
                return true;

            const qreal k = newFactor / oldFactor;

            const QPointF pos = wheel->position();
            const qreal px = pos.x();
            const qreal py = pos.y();

            const qreal sx = hBar->value();
            const qreal sy = vBar->value();

            m_view->setZoomMode(QPdfView::ZoomMode::Custom);
            m_view->setZoomFactor(newFactor);

            hBar->setValue(qRound(sx * k + px * (k - 1.0)));
            vBar->setValue(qRound(sy * k + py * (k - 1.0)));

            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}