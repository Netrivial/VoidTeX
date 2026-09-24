#ifndef COMPILER_H
#define COMPILER_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

class Compiler : public QObject
{
    Q_OBJECT

public:
    struct Job
    {
        QString     program;         // "pdflatex"
        QStringList arguments;       // ["-interaction=nonstopmode", ...]
        QString     workingDir;
        QString     sourcePath;
        QString     pdfPath;
    };

    struct Result
    {
        bool    success = false;
        int     exitCode = -1;
        QString pdfPath;
        QString errorMessage;
    };

    explicit Compiler(QObject *parent = nullptr);

    bool isRunning() const;

    QString engine() const { return m_engine; }
    void    setEngine(const QString &engine) { m_engine = engine; }

    Job makeDefaultJob(const QString &texFilePath) const;

public slots:
    void run(const Compiler::Job &job);
    void cancel();

signals:
    void started(const QString &program, const QStringList &arguments);
    void outputLine(const QString &line);
    void finished(const Compiler::Result &result);

private slots:
    void onReadyReadStdout();
    void onReadyReadStderr();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onProcessError(QProcess::ProcessError error);

private:
    void emitBufferLines(QByteArray &buffer, bool flushAll = false);

    QProcess *m_process = nullptr;
    QString   m_engine = "pdflatex";

    QByteArray m_stdoutBuffer;
    QByteArray m_stderrBuffer;

    Job m_currentJob;
    bool m_canceled = false;
};

#endif // COMPILER_H