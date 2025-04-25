#include "LogSession.h"

LogSession::LogSession(QObject *parent)
    : QObject{parent}
{}

LogSession::~LogSession()
{
    for(const auto &part : m_parts)
        delete part;
}

void LogSession::addPart(
    LogTypeEnum logType,
    const QString &function,
    const QString &message)
{
    m_parts.append( new LogSessionPart( logType, function, message ) );
    emit this->sessionChanged();
}

const LogSessionPartRPtrList &LogSession::getParts() const
{
    return m_parts;
}
