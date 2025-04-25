#include "cpp/support/Log.h"

bool Log::firstLog = true;
// LogSession Log::currentSession = LogSession();
QString Log::currentSession = QString();

void Log::info(cQS func, cQS log, Log::Action action)
{
    Log::safeLog(LogTypeEnum::Info, func, log, action);
}

void Log::warning(cQS func, cQS log, Log::Action action)
{
    Log::safeLog(LogTypeEnum::Warning, func, log, action);
}

void Log::error(cQS func, cQS log, Log::Action action)
{
    Log::safeLog(LogTypeEnum::Error, func, log, action);
}

void Log::debug(cQS func, cQS log, Log::Action action)
{
    Log::safeLog(LogTypeEnum::Debug, func, log, action);
}

QString Log::time()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    return currentDateTime.toString("yyyy-MM-dd HH:mm:ss.zzz");
}

QString Log::buildPrefix(LogTypeEnum logType, cQS function, bool time)
{
    QString prefix;

    // set time
    if(time)
        prefix = "[" + Log::time() +  "]" + " ";

    // set type
    if(logType == LogTypeEnum::Info)            prefix += "I ";
    else if(logType == LogTypeEnum::Warning)    prefix += "W " "### ";
    else if(logType == LogTypeEnum::Error)      prefix += "E " "### ### ";
    else if(logType == LogTypeEnum::Debug)      prefix += "D ";
    else
    {
        qDebug() << "unknown type" << logType << "returning: ?";
        prefix += "? ";
    }

    // set function name
    if(function.length() >= EST_FUNCTION_LENGTH)
        prefix += function;
    else
    {
        size_t fill = EST_FUNCTION_LENGTH - function.length() - prefix.size();
        prefix += QString(SHORTER_FUNCTION_FILL_CHARACTER).repeated(fill);
        prefix += function;
    }

#if SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT
    prefix += " ";
#endif

    // set space between content
    prefix += QString(CONTENT_SPACE_CHARACTER).repeated(CONTENT_SPACE);

#if SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT
    prefix += " ";
#endif

    return prefix;
}

QString Log::buildStartPrefix()
{
    QString prefix;
    // set time
    prefix = "[" + Log::time() +  "]" + "   ";

    return prefix + "--- [APPLICATION STARTED] ---";
}

void Log::log(LogTypeEnum logType, cQS function, cQS log, Log::Action action)
{
    Action limitedAction = Action( (action | Log::actionForceLowest) & Log::actionForceHighest );

    QString logWithTime = buildPrefix(logType, function, true) + log;
    QString logWithoutTime = buildPrefix(logType, function, false) + log;

    switch(action){
    case Log::Action::None: return;
    case Log::Action::Save:
        Log::saveFile(logWithTime);
        break;
    case Log::Action::Print:
        Log::print(logWithoutTime);
        break;
    case Log::Action::Session:
        Log::addSession(logWithoutTime); // Log::addSession(logType, function, log);
        break;
    case Log::Action::SavePrint:
        Log::saveFile(logWithTime);
        Log::print(logWithoutTime);
        break;
    case Log::Action::SaveSession:
        Log::saveFile(logWithTime);
        Log::addSession(logWithoutTime); // Log::addSession(logType, function, log);
        break;
    case Log::Action::PrintSession:
        Log::print(logWithoutTime);
        Log::addSession(logWithoutTime); // Log::addSession(logType, function, log);
        break;
    case Log::Action::All:
        Log::saveFile(logWithTime);
        Log::print(logWithoutTime);
        Log::addSession(logWithoutTime); // Log::addSession(logType, function, log);
    }
}

void Log::safeLog(LogTypeEnum logType, cQS function, cQS log, Action action)
{
    try {
        Log::log(logType, function, log, action);
    } catch (...) {
        qDebug() << "logging failed";
    }
}

void Log::print(cQS content)
{
    qDebug() << content.toStdString().c_str();
}

void Log::saveFile(cQS content)
{
    QFile file(LOG_FILE);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        WR << "Error while creating info log!";
        return;
    }

    QTextStream out(&file);
    if(firstLog)
    {
        out << Log::buildStartPrefix() << "\n";
        firstLog = false;
    }
    out << content << "\n";
    file.close();
}

void Log::addSession(cQS content)
{
    Log::currentSession += content + "\n";
}

void Log::addSessionSafe(cQS content)
{
    try{
        Log::addSession(content);
    }
    catch (...) {
        qDebug() << "adding log to session failed";
    }

}

// void Log::addSession(LogTypeEnum logType, const QString &function, const QString &message)
// {
//     Log::currentSession.addPart(logType, function, message);
// }

QString Log::Convert::listUrlToString(QList<QUrl> list)
{
    QString str("[");
    for(const auto &i : list)
        str += "\"" + i.toLocalFile() + "\", ";
    return str + "\b\b]";
}

#define AC_TYPE(x) static_cast<Log::ActionType>(x) // Log Action Type

Log::Action operator|(Log::Action a, Log::Action b)
{
    return static_cast<Log::Action> ( AC_TYPE(a) | AC_TYPE(b) );
}

Log::Action operator&(Log::Action a, Log::Action b)
{
    return static_cast<Log::Action> ( AC_TYPE(a) & AC_TYPE(b) );
}

