#ifndef LOGSESSION_H
#define LOGSESSION_H

#include <QObject>

#include "LogSessionPart.h"

class LogSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(const LogSessionPartRPtrList &parts READ getParts NOTIFY sessionChanged FINAL)

public:
    explicit LogSession(QObject *parent = nullptr);
    ~LogSession();

    void addPart(
        LogTypeEnum logType,
        const QString &function,
        const QString &message);

signals:
    void sessionChanged();

public:
    const LogSessionPartRPtrList &getParts() const;

private:
    LogSessionPartRPtrList m_parts;
};

#endif // LOGSESSION_H
