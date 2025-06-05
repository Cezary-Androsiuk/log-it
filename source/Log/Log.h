#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <cstdarg> // va_list

// #include "LogSession.h"

extern const char *version;
extern const char *debugLogsOutputDirectory;
extern const char *traceLogsOutputDirectory;

#define ENABLE_MANAGING_LOG_INSTANCE_LIFE_TIME false /// decides if 'SingletonManager' class is a friend and could manage singleton life time by Log::instance
#define EST_FUNCTION_LENGTH 70 /// estimated function name length what will be reserved while creating log
#define SHORTER_FUNCTION_FILL_CHARACTER ' ' /// characters that fills area before function name to fit estimated function name length
#define CONTENT_SPACE 10 /// space between function name and content
#define CONTENT_SPACE_CHARACTER ' ' /// characters that fills space between function name and content
#define SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT true /// creates spaces between space: "x ........ y" instead of "x........y"
#define DISPLAY_OBJECT_LIFE_TIME true /// decides if Display Object Life Time (DOLT) is enabled
#define ENABLE_TRACE_LOGGING true /// decides if trace logging is enabled and functions names will be saved to logs


/// String As PrintF
#define SAPF(...) Log::asprintf(__VA_ARGS__)

/// DOLT wtih 'Force' allows to disable all other DOLT and focus on only one class
/// DOLT Variable (DOLTV) allows to pass arguments that are used in constructor/destructor

/// Display Object Life Time Variable - Force
#define DOLTV_F(ptr, argsStr) {                                             \
    std::string f_name(__FUNCTION__);                                       \
    if(f_name.empty())      f_name = "unknown action";                      \
    if(f_name[0] == '~')    f_name = "Destroying " + f_name;                \
    else                    f_name = "Creating " + f_name;                  \
    std::string qargsStr(argsStr);                                          \
    if(!qargsStr.empty()) qargsStr = "(" + qargsStr + ")";                  \
    DA(Log::Action::SaveSession, f_name + qargsStr + SAPF(": %p", ptr));    \
}

/// Display Object Life Time - Force
#define DOLT_F(ptr) DOLTV_F(ptr, "")

#if DISPLAY_OBJECT_LIFE_TIME
    /// Display Object Life Time Variable
    #define DOLTV(ptr, argsStr) DOLTV_F(ptr, argsStr)

    /// Display Object Life Time
    #define DOLT(ptr) DOLTV(ptr, "")
#else
    /// Display Object Life Time Variable
    #define DOLTV(ptr, argsStr)

    /// Display Object Life Time
    #define DOLT(ptr)
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

#if ENABLE_TRACE_LOGGING
#define TR Log::getInstance()->trace        (__FILE__, /*__FUNCTION__,*/ __PRETTY_FUNCTION__, __LINE__); /// trace
#else
#define TR
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

    typedef const std::string &cstr;

    static constexpr Action actionForceHighest = Action::All; /// set highest ( will be compared with & sign )
    static constexpr Action actionForceLowest = Action::None;  /// set lowest ( will be compared with | sign )

private:
    Log();
    ~Log();
public:


    static Log *getInstance();

    void info(cstr func, cstr log, Action action = Action(Action::All));
    void warning(cstr func, cstr log, Action action = Action(Action::All));
    void error(cstr func, cstr log, Action action = Action(Action::All));
    void debug(cstr func, cstr log, Action action = Action(Action::All));
    void raw(cstr func, cstr log, Action action = Action(Action::All));

    void trace(std::string file, cstr func, int line);

    static std::string asprintf(const char *text, ...);
    static std::string asprintf(cstr text, ...);

    const std::string &getCurrentSession() const;
    // const LogSession &getCurrentSession() const;

private:
    static void openFile(const char *directory, cstr fileName, std::ofstream &file);
    static std::string time(bool simpleSeparators = false);
    static std::string buildPrefix(Type logType, cstr funName);
    static std::string buildStartPrefix();

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
        static std::string vectorToString(std::vector<std::string> list);
    };

private:
    // LogSession m_currentSession;
    std::string m_currentSession;

    std::string m_startTime;

    std::ofstream m_debugLogFile;
    std::ofstream m_traceLogFile;

#if ENABLE_MANAGING_LOG_INSTANCE_LIFE_TIME
    static Log* instance;
    friend class SingletonManager;
#endif
};

#endif // LOG_H
