#ifndef PDFPREVIEW_H
#define PDFPREVIEW_H

#include <QWidget>

class QPdfDocument;
class QPdfView;

class PdfPreview : public QWidget
{
    Q_OBJECT

public:
    explicit PdfPreview(QWidget *parent = nullptr);

public slots:
    void loadPdf(const QString &path);
//    void clear(); <- do not use anymore

private slots:
    void onDocumentStatusChanged();

private:
    QPdfDocument *m_document = nullptr;
    QPdfView     *m_view     = nullptr;
    QString       m_currentPath;

protected: bool eventFilter(QObject*, QEvent*) override;

};

#endif // PDFPREVIEW_H