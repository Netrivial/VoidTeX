#include "compiler.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

Compiler::Compiler(QObject *parent)
    : QObject(parent)
{
    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::SeparateChannels);

    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &Compiler::onReadyReadStdout);
    connect(m_process, &QProcess::readyReadStandardError,
            this, &Compiler::onReadyReadStderr);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &Compiler::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred,
            this, &Compiler::onProcessError);
}

bool Compiler::isRunning() const
{
    return m_process->state() != QProcess::NotRunning;
}

Compiler::Job Compiler::makeDefaultJob(const QString &texFilePath) const
{
    const QFileInfo fi(texFilePath);
    const QString workDir = fi.absolutePath();
    const QString buildDir = workDir + "/build";
    QDir().mkpath(buildDir);

    Job job;
    job.program = m_engine;   // "pdflatex"
    job.workingDir = workDir;
    job.sourcePath = texFilePath;
    job.pdfPath = buildDir + "/" + fi.completeBaseName() + ".pdf";
    job.arguments = {
        "-interaction=nonstopmode",
        "-file-line-error",
        "-output-directory=build",
        "-synctex=1",
        fi.fileName()
    };
    return job;
}

void Compiler::run(const Job &job)
{
    if (isRunning()) {
        emit outputLine(tr("[compiler] Уже выполняется другая задача, отменяю..."));
        cancel();
        return;
    }

    m_currentJob = job;
    m_canceled = false;
    m_stdoutBuffer.clear();
    m_stderrBuffer.clear();

    m_process->setWorkingDirectory(job.workingDir);

    emit started(job.program, job.arguments);
    emit outputLine(tr("[compiler] $ %1 %2")
                        .arg(job.program, job.arguments.join(' ')));

    m_process->start(job.program, job.arguments);
}

void Compiler::cancel()
{
    if (!isRunning())
        return;

    m_canceled = true;
    emit outputLine(tr("[compiler] Прерывание..."));

    m_process->terminate();
    if (!m_process->waitForFinished(1500))
        m_process->kill();
}

void Compiler::onReadyReadStdout()
{
    m_stdoutBuffer.append(m_process->readAllStandardOutput());
    emitBufferLines(m_stdoutBuffer);
}

void Compiler::onReadyReadStderr()
{
    m_stderrBuffer.append(m_process->readAllStandardError());
    emitBufferLines(m_stderrBuffer);
}

void Compiler::emitBufferLines(QByteArray &buffer, bool flushAll)
{
    int idx;
    while (!flushAll && (idx = buffer.indexOf('\n')) != -1) {
        const QByteArray line = buffer.left(idx);
        buffer.remove(0, idx + 1);
        emit outputLine(QString::fromLocal8Bit(line).trimmed());
    }
    if (flushAll && !buffer.isEmpty()) {
        emit outputLine(QString::fromLocal8Bit(buffer).trimmed());
        buffer.clear();
    }
}

void Compiler::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    emitBufferLines(m_stdoutBuffer, /*flushAll=*/true);
    emitBufferLines(m_stderrBuffer, /*flushAll=*/true);

    Result result;
    result.exitCode = exitCode;
    result.success = (status == QProcess::NormalExit && exitCode == 0);

    if (result.success)
        result.pdfPath = m_currentJob.pdfPath;

    if (m_canceled)
        emit outputLine(tr("[compiler] Процесс прерван пользователем."));

    emit finished(result);
}

void Compiler::onProcessError(QProcess::ProcessError error)
{
    Result result;
    result.success = false;

    switch (error) {
    case QProcess::FailedToStart:
        result.errorMessage = tr("Не удалось запустить '%1'. "
                                 "Убедитесь, что он установлен и доступен в PATH.")
                                  .arg(m_currentJob.program);
        break;
    case QProcess::Crashed:
        result.errorMessage = tr("Процесс аварийно завершился.");
        break;
    case QProcess::Timedout:
        result.errorMessage = tr("Процесс не ответил вовремя.");
        break;
    default:
        result.errorMessage = tr("Ошибка процесса: %1").arg(int(error));
        break;
    }

    emit outputLine(tr("[compiler] %1").arg(result.errorMessage));
    emit finished(result);
}