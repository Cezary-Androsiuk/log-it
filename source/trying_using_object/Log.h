#ifndef LOG_H
#define LOG_H

#include <string>

/////////////////// DEBUG MACROS ///////////////////
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
////////////////////////////////////////////////////


/// DEFAULT DATA
///    - can be changed using setters in Log class
#define LOG_FILE "LogHistory.log"

/// size of file above what file will be trimmed to "trimLogFileSize" size
#define MAX_LOG_FILE_SIZE 209'715'200

/// size to what file will be trimmed, if crosses "maxLogFileSize" size
#define TRIM_LOG_FILE_SIZE 10'485'760

/// estimated function name length what will be reserved while creating log
#define EST_FUNCTION_LENGTH 70

/// characters that fills area before function name to fit estimated function name length
#define SHORTER_FUNCTION_FILL_CHARACTER ' '

/// creates spaces between space: "x ........ y" instead of "x........y"
#define SPACE_AFTER_FUNCTION_NAME_ENABLED true

/// space between function name and content
#define SPACE_AFTER_FUNCTION_NAME_SIZE 10

/// characters that fills space between function name and content
#define SPACE_AFTER_FUNCTION_NAME_CHARACTER ' '


class Log
{
    Log();
    ~Log();

public:
    static Log *getInstance();

    enum class Type{
        Info,
        Warning,
        Error,
        Debug,
    };

    enum class Action: int{
        None = 0,
        Save,
        Print,
        Session,
        SavePrint,
        SaveSession,
        PrintSession,
        All,
    };
    // typedef std::underlying_type_t<Action> ActionType; // just an elegant way

    typedef const std::string &cstr;

    static constexpr Action actionForceHighest = Action::All; // set highest ( will be compared with & sign )
    static constexpr Action actionForceLowest = Action::None;  // set lowest ( will be compared with | sign )

    void info(cstr func, cstr log, Action action = Action(Action::All));
    void warning(cstr func, cstr log, Action action = Action(Action::All));
    void error(cstr func, cstr log, Action action = Action(Action::All));
    void debug(cstr func, cstr log, Action action = Action(Action::All));

private:
    static std::string time();
    static std::string buildPrefix(Type logType, cstr function, bool time = false);
    static std::string buildStartPrefix();

    static void log(Type logType, cstr function, cstr log, Action action = Action::All);
    static void safeLog(Type logType, cstr function, cstr log, Action action = Action::All);
    static void print(cstr content);
    static void saveFile(cstr content);
    static void addSession(cstr content);
    static void addSessionSafe(cstr content);
    // static void addSession(Type logType, const QString &function, const QString &message);

public:
    const std::string &getLogFile() const;
    size_t getMaxLogFileSize() const;
    size_t getTrimLogFileSize() const;
    int getEstimatedFunctionLength() const;
    char getShorterFunctionFillCharacter() const;
    bool getSpaceAfterFunctionNameEnable() const;
    int getSpaceAfterFunctionNameSize() const;
    char getSpaceAfterFunctionNameCharacter() const;

    void setLogFile(const std::string &filePath);
    void setMaxLogFileSize(size_t fileSize);
    void setTrimLogFileSize(size_t fileSize);
    void setEstimatedFunctionLength(int length);
    void setShorterFunctionFillCharacter(char fillCharacter);
    void setSpaceAfterFunctionNameEnable(bool enable);
    void setSpaceAfterFunctionNameSize(int length);
    void setSpaceAfterFunctionNameCharacter(char gapCharacter);


private:
    std::string m_logFileName;
    size_t m_maxLogFileSize;
    size_t m_logTrimFileSize;

    int m_estFunctionSize;
    char m_shorterFunctionFillCharacter;
    bool m_spaceAfterFunctionNameEnabled;
    int m_spaceAfterFunctionNameSize;
    char m_spaceAfterFunctionNameCharacter;

    bool m_firstLog;

    std::string m_currentSession;
};

Log::Action operator |(Log::Action a, Log::Action b);
Log::Action operator &(Log::Action a, Log::Action b);

#endif // LOG_H
