#include "Log.h"

QString Log::m_sessionLogs = QString();
bool Log::fileSizeProtectionExecuted = false;

void Log::info(QString func, QString log)
{
    Log::log( Log::buildPrefix(Log::time(), "I", func) + log );
}

void Log::warning(QString func, QString log)
{
    Log::log( Log::buildPrefix(Log::time(), "W", func) + log );
}

void Log::error(QString func, QString log)
{
    Log::log( Log::buildPrefix(Log::time(), "E", func) + log );
}

void Log::debug(QString func, QString log)
{
    Log::log( Log::buildPrefix(Log::time(), "D", func) + log );
}

QString Log::time()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    return currentDateTime.toString("yyyy-MM-dd HH:mm:ss.zzz");
}

QString Log::buildPrefix(QString time, QString type, QString func)
{
    QString prefix;
    // set time
    prefix = "[" + time +  "]" + " ";
    // set type
    prefix += type + " ";

    // set function name
    if(func.length() >= EST_FUNCTION_LENGTH)
        prefix += func;
    else
    {
        size_t fill = EST_FUNCTION_LENGTH - func.length();
        prefix += QString(SHORTER_FUNCTION_FILL_CHARACTER).repeated(fill);
        prefix += func;
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

void Log::log(QString content)
{
    Log::fileSizeProtection();

    Log::print(content);
    Log::saveFile(content);
    Log::addSession(content);
}

void Log::print(QString content)
{
    qDebug() << content.toStdString().c_str();
}

void Log::saveFile(QString content)
{
    QFile file(LOG_FILE);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        WR << "Error while creating info log!";
        return;
    }

    QTextStream out(&file);
    out << content << "\n";
    file.close();
}

void Log::addSession(QString content)
{
    Log::m_sessionLogs += content + "\n";
}

void Log::fileSizeProtection()
{
    if(Log::fileSizeProtectionExecuted)
        return;

    Log::fileSizeProtectionExecuted = true;

    /// get log file size
    QFile file(LOG_FILE);
    if(!file.exists())
        return;
    qint64 fileSize = file.size();

    /// compare with given def max size
    if(fileSize < MAX_LOG_FILE_SIZE)
        return;

    /// trim to given def size
    if (!file.open(QIODevice::ReadOnly))
    {
        WR << "Error while oppening info log!";
        return;
    }
    file.seek(fileSize - TRIM_LOG_FILE_SIZE);
    QByteArray trimmedData = file.read(TRIM_LOG_FILE_SIZE);
    file.close();

    file.moveToTrash(); /// delete old (to large) file

    if(!file.open(QIODevice::WriteOnly))
    {
        WR << "Error while recreating, the smaller info log file!";
        return;
    }
    file.write(trimmedData);
    file.close();

    DB << "Trimmed log file! Old version was moved to the trash.";
}

QString Log::Convert::listUrlToString(QList<QUrl> list)
{
    QString str("[");
    for(const auto &i : list)
        str += "\"" + i.toLocalFile() + "\", ";
    return str + "\b\b]";
}
