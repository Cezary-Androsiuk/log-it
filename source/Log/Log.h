#ifndef LOG_H
#define LOG_H

/*

SAPF(...)       - String As PrintF - creates string from a printf like formatting

I(...)          - Info - info log - log starts with "I"
W(...)          - Warning - log starts with "W ###"
E(...)          - Error - log starts with "E ### ###"
D(...)          - Debug - log starts with "D"
R(...)          - Raw - log starts with "R" - no formating in terminal output but more space in logs

IA(a, ...)      - InfoAction - as Info log but action can be scpecified (only print or only sesion and save etc.)
WA(a, ...)      - WarningAction - as Warning log but action can be scpecified (only print or only sesion and save etc.)
EA(a, ...)      - ErrorAction - as Error log but action can be scpecified (only print or only sesion and save etc.)
DA(a, ...)      - DebugAction - as Debug log but action can be scpecified (only print or only sesion and save etc.)
RA(a, ...)      - RawAction - as Raw log but action can be scpecified (only print or only sesion and save etc.)

TRM             - TRace Method - placed at start of any class method allows to track later how program behaves
TRF             - TRace Function - placed at start of any function or a static method allows to track later how program behaves
TRMV(...)       - TRace Method Variables - just like TRM but allows to pass arguments (SAPF format) that are given to that method
TRFV(...)       - TRace Function Variables - just like TRF but allows to pass arguments (SAPF format) that are given to that function

DOLT            - Display Object Life Time - placed in constructor and destructor allows to log informations (with pointer to it) about that object life time
DOLT_F          - Display Object Life Time - Force - just like DOLT, but can't be disabled by DISPLAY_OBJECT_LIFE_TIME
DOLTV(...)      - Display Object Life Time with Variables - just like a DOLT but allows to pass values of an constructor arguments
DOLTV_F(...)    - Display Object Life Time with Variables - Force - just like a DOLT_F and DOLTV(...)


==================================
Examples:

SAPF(...)       - SAPF("%d", 1); or SAPF("%d %s", 1, "abc"); or SAPF("%d %s %s", 1, "abc", stringVar.c_str());

I(...)          - I("%d = %s", 1, "abc");
W(...)          - W("%d = %s", 1, "abc");
E(...)          - E("%d = %s", 1, "abc");
D(...)          - D("%d = %s", 1, "abc");
R(...)          - R("%d = %s", 1, "abc");

IA(a, ...)      - IA(Log::Action::None, "%d = %s", 1, "abc");
WA(a, ...)      - WA(Log::Action::Save, "%d = %s", 1, "abc");
EA(a, ...)      - EA(Log::Action::Print, "%d = %s", 1, "abc");
DA(a, ...)      - DA(Log::Action::PrintSession, "%d = %s", 1, "abc");
RA(a, ...)      - RA(Log::Action::All, "%d = %s", 1, "abc");

TRM             - void function(){TRM; //////; }
TRF             - void function(){TRF; //////; }
TRMV(...)       - void function(int a, char *b){TRMV("%d, %s", a, b); //////; }
TRFV(...)       - void function(int a, char *b){TRFV("%d, %s", a, b); //////; }

DOLT            - MyClass(){DOLT; //////; }
DOLT_F          - MyClass(){DOLT_F; //////; }
DOLTV(...)      - MyClass(int c, int b, int a){DOLTV("1, 2, 3") or DOLTV("1, 2, 3", this); //////; }
DOLTV_F(...)    - MyClass(int c, int b, int a){DOLTV_F("1, 2, 3") or DOLTV_F("1, 2, 3", this); //////; }

*/


#define USE_QT_SUPPORT false /// in external layer of abstraction QString will be used insead of std::string
/// it allows to use I(someQStringVariable); instead of  I(someQStringVariable.toStdString());

#define ENABLE_MANAGING_LOG_INSTANCE_LIFE_TIME false /// decides if 'SingletonManager' class is a friend and could manage singleton life time by Log::instance
#define EST_FUNCTION_LENGTH 70 /// estimated function name length what will be reserved while creating log
#define SHORTER_FUNCTION_FILL_CHARACTER ' ' /// characters that fills area before function name to fit estimated function name length
#define CONTENT_SPACE 10 /// space between function name and content
#define CONTENT_SPACE_CHARACTER ' ' /// characters that fills space between function name and content
#define SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT true /// creates spaces between space: "x ........ y" instead of "x........y"
#define DISPLAY_OBJECT_LIFE_TIME true /// decides if Display Object Life Time (DOLT) is enabled
#define ENABLE_TRACE_LOGGING true /// decides if trace logging is enabled and functions names will be saved to logs
#define MAX_LINE_INDEX_NUMBER_LENGTH_IN_TRACE_LOG 7 /// to keep logs pretty - assert that max lines count in any file is 9'999'999
#define MAX_FILES_IN_PROJECT_COUNT_NUMBER_LENGTH 4 /// to keep logs pretty - assert that max count of files in project is 9'999

#include <cstdio>

#include <string>
#include <vector>

#if USE_QT_SUPPORT
#include <QString>
#include <QList>
#include <QUrl>
#endif /// USE_QT_SUPPORT

#include <fstream>
#include <cstdarg> // va_list
#include <unordered_map>

// #include "LogSession.h"

extern const char *version;
extern const char *debugLogsOutputDirectory;
extern const char *traceLogsOutputDirectory;
extern const char *traceLogsInfoFileName;

typedef std::string str; /// string
typedef const str &cstr; /// const string

#if USE_QT_SUPPORT
typedef QString qstr; /// q string
typedef const qstr &cqstr; /// const q string
#endif /// USE_QT_SUPPORT

#if USE_QT_SUPPORT
typedef qstr estr; /// extern string
typedef cqstr cestr; /// const extern string
#else
typedef str estr; /// extern string
typedef cstr cestr; /// const extern string
#endif /// USE_QT_SUPPORT

/// String As PrintF
#define SAPF(...) Log::asprintf(__VA_ARGS__)

/// DOLT wtih 'Force' allows to disable all other DOLT and focus on only one class
/// DOLT Variable (DOLTV) allows to pass arguments that are used in constructor/destructor


#if USE_QT_SUPPORT
#define ESTR_IS_EMPTY(s) s.isEmpty()
#else
#define ESTR_IS_EMPTY(s) s.empty()
#endif /// USE_QT_SUPPORT

/// Display Object Life Time Variable - Force - 2 arguments (constructor arguments values and pointer to object)
#define __DOLTV_F_2(argsStr, ptr) {                                           \
estr f_name(__FUNCTION__);                                              \
    if(ESTR_IS_EMPTY(f_name))      f_name = "unknown action";               \
    if(f_name[0] == '~')    f_name = "Destroying " + f_name;                \
    else                    f_name = "Creating " + f_name;                  \
    estr argsEStr(argsStr);                                                 \
    if(!ESTR_IS_EMPTY(argsEStr)) argsEStr = "(" + argsEStr + ")";            \
    DA(Log::Action::SaveSession, f_name + argsEStr + SAPF(": %p", ptr));    \
}

/// Display Object Life Time Variable - Force - 1 argument (constructor arguments values)
#define __DOLTV_F_1(argsStr)        __DOLTV_F_2(argsStr, this);

/// Display Object Life Time Variable - Force - variant arguments
#define __DOLTV_F_GET_OVERRIDE(_1, _2, NAME, ...) NAME
#define DOLTV_F(...) __DOLTV_F_GET_OVERRIDE(__VA_ARGS__, __DOLTV_F_2, __DOLTV_F_1)(__VA_ARGS__);

/// Display Object Life Time - Force
#define DOLT_F DOLTV_F("");

#if DISPLAY_OBJECT_LIFE_TIME
/// Display Object Life Time Variable - variant arguments
#define __DOLTV_GET_OVERRIDE(_1, _2, NAME, ...) NAME
#define DOLTV(...) __DOLTV_GET_OVERRIDE(__VA_ARGS__, __DOLTV_F_2, __DOLTV_F_1)(__VA_ARGS__);

/// Display Object Life Time
#define DOLT DOLTV_F("");
#else
/// Display Object Life Time Variable - variant arguments
#define DOLTV(...)

/// Display Object Life Time
#define DOLT
#endif

#define I(...) Log::getInstance()->info    (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__)) /// info
#define W(...) Log::getInstance()->warning (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__)) /// warning
#define E(...) Log::getInstance()->error   (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__)) /// error
#define D(...) Log::getInstance()->debug   (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__)) /// debug
#define R(...) Log::getInstance()->raw     (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__)) /// raw

#define IA(a, ...) Log::getInstance()->info    (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__), Log::Action(a)); /// info
#define WA(a, ...) Log::getInstance()->warning (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__), Log::Action(a)); /// warning
#define EA(a, ...) Log::getInstance()->error   (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__), Log::Action(a)); /// error
#define DA(a, ...) Log::getInstance()->debug   (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__), Log::Action(a)); /// debug
#define RA(a, ...) Log::getInstance()->raw     (__PRETTY_FUNCTION__, SAPF(__VA_ARGS__), Log::Action(a)); /// raw

// #define TR

#if ENABLE_TRACE_LOGGING
// TRace Method
#define TRM Log::getInstance()->trace           (__FILE__, /*__FUNCTION__,*/ __PRETTY_FUNCTION__, __LINE__, this, ""); /// trace method

// TRace Method Variables
#define TRMV(...) Log::getInstance()->trace     (__FILE__, /*__FUNCTION__,*/ __PRETTY_FUNCTION__, __LINE__, this, SAPF(__VA_ARGS__)); /// trace method variables

// TRace Function (or static method)
#define TRF Log::getInstance()->trace           (__FILE__, /*__FUNCTION__,*/ __PRETTY_FUNCTION__, __LINE__, nullptr, ""); /// trace function

// TRace Function Variables (or static method)
#define TRFV(...) Log::getInstance()->trace     (__FILE__, /*__FUNCTION__,*/ __PRETTY_FUNCTION__, __LINE__, nullptr, SAPF(__VA_ARGS__)); /// trace function variables
#else
#define TRM
#define TRMV(...)
#define TRF
#define TRFV(...)
#endif


class Log
{
public:
    enum class Type{
        Info,
        Warning,
        Error,
        Debug,
        Raw,
    };
    static const char *logTypeToStr(Type type);

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
    static const char *logActionToStr(Action action);

    static constexpr Action actionForceHighest = Action::All; /// set highest ( will be compared with & sign )
    static constexpr Action actionForceLowest = Action::None;  /// set lowest ( will be compared with | sign )

private:
    Log();
    ~Log();
public:


    static Log *getInstance();

    void info(cstr func, cestr log, Action action = Action(Action::All));
    void warning(cstr func, cestr log, Action action = Action(Action::All));
    void error(cstr func, cestr log, Action action = Action(Action::All));
    void debug(cstr func, cestr log, Action action = Action(Action::All));
    void raw(cstr func, cestr log, Action action = Action(Action::All));

    void trace(cstr file, cstr func, int line, const void *ptr, cestr args);

    static estr asprintf(const char *text, ...);
    static estr asprintf(cestr text, ...);

    cstr getCurrentSession() const;
    // const LogSession &getCurrentSession() const;

private:
    static void openFile(const char *directory, cstr fileName, std::ofstream &file);
    void logInfoAboutLogProperties();
    void logInfoAboutTraceProperties();
    static std::string time(bool simpleSeparators = false);
    static std::string buildPrefix(Type logType, cstr funName);
    static std::string buildStartPrefix();

    static size_t computeMaxNumberFromNumberLength(int length);
    // static size_t increaseNumberToClosestTwoSquare(size_t number);

    void log(Type logType, cstr funName, cstr log, Action action = Action::All);
    void safeLog(Type logType, cstr funName, cstr log, Action action = Action::All);
    void print(cstr content, bool newLine = true);
    void saveDebugLogFile(cstr content);
    void saveTraceLogFile(cstr content);
    void addSession(cstr content, bool newLine = true);
    // void addSession(Type logType, cstr funName, cstr message);

public:
    class Convert{
    public:
#if USE_QT_SUPPORT
        static QString listUrlToString(QList<QUrl> list);
        static QString listStrToString(QList<QString> list);
#else
        static std::string vectorToString(std::vector<std::string> list);
#endif /// USE_QT_SUPPORT
    };

private:
    // LogSession m_currentSession;
    std::string m_currentSession;

    std::string m_startTime;

    std::ofstream m_debugLogFile;
    std::ofstream m_traceLogFile;

    std::unordered_map<std::string, int> m_filesPaths;

#if ENABLE_MANAGING_LOG_INSTANCE_LIFE_TIME
    static Log* instance;
    friend class SingletonManager;
#endif
};

#endif // LOG_H
