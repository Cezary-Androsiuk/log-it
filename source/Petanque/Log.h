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

#include "LogSession.h"

#define QAPF(...) QString::asprintf(__VA_ARGS__) /* QString as printf */
#define QAPF_T(text, ...) QString::asprintf(tr(text).toStdString().c_str(), __VA_ARGS__) /* QString as printf translate */

#define DISPLAY_OBJECT_LIFE_TIME true

/// Display Object Life Time Variable - Force
#define DOLTV_F(ptr, argsStr) {                                         \
    QString f_name(__FUNCTION__);                                       \
    if(f_name.isEmpty())    f_name = "unknown action";                  \
    if(f_name[0] == '~')    f_name = "Destroying " + f_name;            \
    else                    f_name = "Creating " + f_name;              \
    QString qargsStr(argsStr);                                          \
    if(!qargsStr.isEmpty()) qargsStr = " (" + qargsStr + ")";           \
    D(f_name + qargsStr + QAPF(": %p", ptr), Log::Action::SaveSession); \
}

/// Display Object Life Time - Force
#define DOLT_F(ptr) DOLTV_F(ptr, "")

/// Display Object Life Time Variable
#define DOLTV(ptr, argsStr)     \
if(DISPLAY_OBJECT_LIFE_TIME) {  \
    DOLTV_F(ptr, argsStr)       \
}

/// Display Object Life Time
#define DOLT(ptr) DOLTV(ptr, "")


//////////////////// OLD DEBUG ////////////////////
#define FUNCTION __PRETTY_FUNCTION__
// #define FUNCTION __FUNCTION__
#define DB qDebug() << qSetFieldWidth(30) << FUNCTION << qSetFieldWidth(0)
#define WR qWarning() << qSetPadChar('#') << qSetFieldWidth(30) \
<< QString(" ") + FUNCTION + " " << qSetPadChar(' ') << qSetFieldWidth(0)
//////////////////////////////////////////////////


//////////////////// NEW DEBUG ////////////////////
#define GET_MACRO(_1, _2, NAME, ...) NAME

#define I_1(x) Log::info    (__PRETTY_FUNCTION__, (x));
#define W_1(x) Log::warning (__PRETTY_FUNCTION__, (x));
#define E_1(x) Log::error   (__PRETTY_FUNCTION__, (x));
#define D_1(x) Log::debug   (__PRETTY_FUNCTION__, (x));

#define I_2(x, a) Log::info    (__PRETTY_FUNCTION__, (x), Log::Action(a));
#define W_2(x, a) Log::warning (__PRETTY_FUNCTION__, (x), Log::Action(a));
#define E_2(x, a) Log::error   (__PRETTY_FUNCTION__, (x), Log::Action(a));
#define D_2(x, a) Log::debug   (__PRETTY_FUNCTION__, (x), Log::Action(a));

#define I(...) GET_MACRO(__VA_ARGS__, I_2, I_1)(__VA_ARGS__)
#define W(...) GET_MACRO(__VA_ARGS__, W_2, W_1)(__VA_ARGS__)
#define E(...) GET_MACRO(__VA_ARGS__, E_2, E_1)(__VA_ARGS__)
#define D(...) GET_MACRO(__VA_ARGS__, D_2, D_1)(__VA_ARGS__)
//////////////////////////////////////////////////


#define LOG_FILE "petanque_team_logs.log"
#define EST_FUNCTION_LENGTH 90 // estimated function name length what will be reserved while creating log
#define SHORTER_FUNCTION_FILL_CHARACTER ' ' // characters that fills area before function name to fit estimated function name length
#define CONTENT_SPACE 10 // space between function name and content
#define CONTENT_SPACE_CHARACTER ' ' // characters that fills space between function name and content
#define SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT true // creates spaces between space: "x ........ y" instead of "x........y"

class Log
{
public:
    enum Action{
        None = 0,
        Save = 1 << 0,
        Print = 1 << 1,
        Session = 1 << 2,
        SavePrint = Save | Print,
        SaveSession = Save | Session,
        PrintSession = Print | Session,
        All = Save | Print | Session,
    };

    typedef const QString &cQS;

    static constexpr Action actionForceHighest = Action::All; // set highest ( will be compared with & sign )
    static constexpr Action actionForceLowest = Action::None;  // set lowest ( will be compared with | sign )

    static void info(cQS func, cQS log, Action action = Action(Action::All));
    static void warning(cQS func, cQS log, Action action = Action(Action::All));
    static void error(cQS func, cQS log, Action action = Action(Action::All));
    static void debug(cQS func, cQS log, Action action = Action(Action::All));

private:
    static QString time();
    static QString buildPrefix(LogTypeEnum logType, cQS function, bool time = false);
    static QString buildStartPrefix();

    static void log(LogTypeEnum logType, cQS function, cQS log, Action action = Action::All);
    static void safeLog(LogTypeEnum logType, cQS function, cQS log, Action action = Action::All);
    static void print(cQS content);
    static void saveFile(cQS content);
    static void addSession(cQS content);
    // static void addSession(LogTypeEnum logType, const QString &function, const QString &message);

public:
    class Convert{
    public:
        static QString listUrlToString(QList<QUrl> list);
    };

    static bool firstLog;

    // static LogSession currentSession;
    static QString currentSession;
};

#endif // LOG_H
