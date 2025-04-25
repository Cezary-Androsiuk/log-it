#ifndef LOG_H
#define LOG_H

#include <QDebug>
#include <QString>

#include <QList>
#include <QUrl>

#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QIODevice>

//////////////////// OLD DEBUG ////////////////////
#define FUNCTION __PRETTY_FUNCTION__
// #define FUNCTION __FUNCTION__
#define DB qDebug() << qSetFieldWidth(30) << FUNCTION << qSetFieldWidth(0)
#define WR qWarning() << qSetPadChar('#') << qSetFieldWidth(30) \
<< QString(" ") + FUNCTION + " " << qSetPadChar(' ') << qSetFieldWidth(0)
//////////////////////////////////////////////////


//////////////////// NEW DEBUG ////////////////////
#define I(x) Log::info    (__PRETTY_FUNCTION__, x);
#define W(x) Log::warning (__PRETTY_FUNCTION__, x);
#define E(x) Log::error   (__PRETTY_FUNCTION__, x);
#define D(x) Log::debug   (__PRETTY_FUNCTION__, x);
//////////////////////////////////////////////////


#define LOG_FILE "LogHistory.log"
#define MAX_LOG_FILE_SIZE 209'715'200 /// in bytes /// value above what, log file will be trimmed to TRIM_LOG_FILE_SIZE size
#define TRIM_LOG_FILE_SIZE 10'485'760 /// in bytes
#define EST_FUNCTION_LENGTH 70 // estimated function name length what will be reserved while creating log
#define SHORTER_FUNCTION_FILL_CHARACTER ' ' // characters that fills area before function name to fit estimated function name length
#define CONTENT_SPACE 10 // space between function name and content
#define CONTENT_SPACE_CHARACTER ' ' // characters that fills space between function name and content
#define SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT true // creates spaces between space: "x ........ y" instead of "x........y"

class Log
{
public:
    static void info(QString func, QString log);
    static void warning(QString func, QString log);
    static void error(QString func, QString log);
    static void debug(QString func, QString log);

private:
    static QString time();
    static QString buildPrefix(QString time, QString type, QString func);

    static void log(QString content);
    static void print(QString content);
    static void saveFile(QString content);
    static void addSession(QString content);

    static void fileSizeProtection();

public:
    class Convert{
    public:
        static QString listUrlToString(QList<QUrl> list);
    };

    static QString m_sessionLogs;
    static bool fileSizeProtectionExecuted;
};

#endif // LOG_H
