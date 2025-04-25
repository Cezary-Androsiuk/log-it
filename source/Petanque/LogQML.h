#ifndef LOGQML_H
#define LOGQML_H

#include <QObject>

#include "cpp/support/Log.h"

#define QML_LOG_PREFIX(funcName) (funcName.isEmpty() ? QString("QML(...)") : ("QML(" + funcName + ")"))

class LogQML : public QObject
{
    Q_OBJECT
    Q_PROPERTY(const QString &currentSession READ getCurrentSession CONSTANT FINAL)
public:
    explicit LogQML(QObject *parent = nullptr);

    typedef const QString &cQS;

    cQS getCurrentSession() const;

public slots:
    Log::Action toAction(const QString &action);
    Log::Action toAction(int action);

public slots:
    void i(cQS log, QString func = "", Log::Action action = Log::Action::All);
    void w(cQS log, QString func = "", Log::Action action = Log::Action::All);
    void e(cQS log, QString func = "", Log::Action action = Log::Action::All);
    void d(cQS log, QString func = "", Log::Action action = Log::Action::All);

signals:
};

#endif // LOGQML_H
