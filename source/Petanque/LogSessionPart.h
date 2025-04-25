#ifndef LOGSESSIONPART_H
#define LOGSESSIONPART_H

#include <QObject>
#include <QList>

#include "cpp/enums/LogTypeEnum.h"

class LogSessionPart : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LogTypeEnum logType      READ getLogType     CONSTANT FINAL)
    Q_PROPERTY(const QString &function  READ getFunction    CONSTANT FINAL)
    Q_PROPERTY(const QString &message   READ getMessage     CONSTANT FINAL)

public:
    explicit LogSessionPart(
        LogTypeEnum logType,
        const QString &function,
        const QString &message,
        QObject *parent = nullptr);

public:
    LogTypeEnum getLogType() const;
    const QString &getFunction() const;
    const QString &getMessage() const;

private:
    const LogTypeEnum m_logType;
    const QString m_function;
    const QString m_message;
};

typedef LogSessionPart * LogSessionPartRPtr;
typedef QList<LogSessionPartRPtr> LogSessionPartRPtrList;

#endif // LOGSESSIONPART_H
