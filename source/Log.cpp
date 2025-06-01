#include "Log.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>
#include <cstring>
#include <filesystem>

const char *version = "v1.2.0";
const char *outputDirectory = "logs/";

#if ENABLE_MANAGING_LOG_INSTANCE_LIFE_TIME
Log *Log::instance = nullptr;
#endif

const char *Log::logTypeToStr(Type type)
{
    switch (type) {
    case Log::Type::Info:       return "Log::Type::Info";
    case Log::Type::Warning:    return "Log::Type::Warning";
    case Log::Type::Error:      return "Log::Type::Error";
    case Log::Type::Debug:      return "Log::Type::Debug";
    case Log::Type::Raw:        return "Log::Type::Raw";
    }

    fprintf(stderr, "unknown Log::Type: %d\n", static_cast<int>(type));
    fflush(stderr);

    return "<unknown Log::Type>";
}

const char *Log::logActionToStr(Action action)
{
    switch (action) {
    case Log::Action::None:             return "Log::Action::None";
    case Log::Action::Save:             return "Log::Action::Save";
    case Log::Action::Print:            return "Log::Action::Print";
    case Log::Action::Session:          return "Log::Action::Session";
    case Log::Action::SavePrint:        return "Log::Action::SavePrint";
    case Log::Action::SaveSession:      return "Log::Action::SaveSession";
    case Log::Action::PrintSession:     return "Log::Action::PrintSession";
    case Log::Action::All:              return "Log::Action::All";
    }

    fprintf(stderr, "unknown Log::Action: %d\n", static_cast<int>(action));
    fflush(stderr);

    return "<unknown Log::Action>";
}

Log *Log::getInstance()
{
#if ENABLE_MANAGING_LOG_INSTANCE_LIFE_TIME
    return Log::instance; /// handled by SingletonManager
#else
    static Log log;
    return &log;
#endif
}

void Log::info(cstr func, cstr log, Log::Action action)
{
    this->safeLog(Type::Info, func, log, action);
}

void Log::warning(cstr func, cstr log, Log::Action action)
{
    this->safeLog(Type::Warning, func, log, action);
}

void Log::error(cstr func, cstr log, Log::Action action)
{
    this->safeLog(Type::Error, func, log, action);
}

void Log::debug(cstr func, cstr log, Log::Action action)
{
    this->safeLog(Type::Debug, func, log, action);
}

void Log::raw(cstr func, cstr log, Action action)
{
    this->safeLog(Type::Raw, func, log, action);
}

void Log::trace(std::string file, cstr func, int line)
{
    std::string time;

    try{
        time = "[" + this->time() +  "]" + " ";
    }
    catch (const std::exception &e) {
        fprintf(stderr, "creating time prefix failed, reason: %s\n", e.what());
        fflush(stderr);
    }


    /// assert path for the project (while compilation) not contains '|' sign
    /// or in worst case at least not contains pattern like "| 0123 |" - start and end with '|', and numers with space inside
    /// but better change any occurance to #, because path is not that required
    for(int i=0; i<file.size(); i++)
    {
        if(file[i] == '|')
            file[i] = '#';
    }

    std::string strLine = asprintf("%6d", line); /// assert that any file not contains more than 1 milion lines
    std::string traceText = "" + file + "|" + strLine + "|" + func;

    /// to create algorithm reading trace path:
    /// 1. find "] T " pattern
    /// 2. copy letters to path string until '|' occur
    /// 3. copy letters to line string until '|' occur again (string shoud be trimmed from all space characters)
    /// 4. copy letters that left to function name string

    try{
        this->saveFile(time + "T " + traceText);
    }
    catch (const std::exception &e) {
        fprintf(stderr, "saving trace failed, reason: %s\n", e.what());
        fflush(stderr);
    }
}

std::string Log::asprintf(const char *text, ...)
{
    va_list args;
    va_start(args, text);

    va_list argsCopy;
    va_copy(argsCopy, args);
    const int size = vsnprintf(nullptr, 0, text, argsCopy);
    va_end(argsCopy);

    if (size <= 0) {
        va_end(args);
        return "";
    }


    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size+1);
    vsnprintf(buffer.get(), size+1, text, args);
    va_end(args);

    return std::string(buffer.get(), size);
}

std::string Log::asprintf(cstr text, ...)
{
    va_list args;
    #pragma clang diagnostic ignored "-Wvarargs" /// qtcreator uses clang
    #pragma GCC diagnostic ignored "-Wvarargs" /// compiler uses gcc
    va_start(args, text.c_str());

    std::string str = Log::asprintf(text.c_str(), args);

    va_end(args);

    return str;
}

const std::string &Log::getCurrentSession() const
{
    return m_currentSession;
}

// const LogSession &Log::getCurrentSession() const
// {
//     return m_currentSession;
// }

std::string Log::time(bool simpleSeparators)
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) % 1000;

    std::tm tm = *std::localtime(&now_time_t);

    std::ostringstream oss;
    if(simpleSeparators)
        oss << std::put_time(&tm, "%Y%m%d_%H%M%S_")
            << std::setfill('0') << std::setw(3) << now_ms.count();
    else
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S.")
            << std::setfill('0') << std::setw(3) << now_ms.count();

    return oss.str();
}

std::string Log::buildPrefix(Log::Type logType, cstr funName)
{
    std::string prefix;

    // set type
    switch (logType) {
    case Log::Type::Info:       prefix += "I ";         break;
    case Log::Type::Warning:    prefix += "W ###";      break;
    case Log::Type::Error:      prefix += "E ### ###";  break;
    case Log::Type::Debug:      prefix += "D ";         break;
    case Log::Type::Raw:        prefix += "R ";         break;
    default:
        fprintf(stderr, "unknown type %s, using: '?'\n", Log::logTypeToStr(logType));
        fflush(stderr);
        prefix += "? ";
        break;
    }

    // set function name
    if(funName.length() >= EST_FUNCTION_LENGTH)
        prefix += funName;
    else
    {
        int fill = EST_FUNCTION_LENGTH - funName.length() - prefix.size();
        if(fill < 1)
            fill = 1;
        prefix += std::string(fill, SHORTER_FUNCTION_FILL_CHARACTER);
        prefix += funName;
    }

#if SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT
    prefix += " ";
#endif

    // set space between content
    prefix += std::string(CONTENT_SPACE, CONTENT_SPACE_CHARACTER);

#if SPACE_BETWEEN_CONTENT_SPACE_AND_CONTENT
    prefix += " ";
#endif

    return prefix;
}

std::string Log::buildStartPrefix()
{
    static const char startText[] = "--- [APPLICATION STARTED] ---";
    static const std::string spaceText =
        std::string(EST_FUNCTION_LENGTH - sizeof(startText)/2 + 3 + 4, '-');

    std::string prefix;
    prefix = /*"\n\n"*/ "[" + this->time() +  "]";

    return prefix + spaceText + startText + spaceText;
}

void Log::log(Log::Type logType, cstr funName, cstr log, Log::Action action)
{
    Action limitedAction = Action( (action | Log::actionForceLowest) & Log::actionForceHighest );

    std::string time;
    std::string prefix;

    try{
        time = "[" + this->time() +  "]" + " ";
    }
    catch (const std::exception &e) {
        fprintf(stderr, "creating time prefix failed, reason: %s\n", e.what());
        fflush(stderr);
    }

    try{
        prefix = buildPrefix(logType, funName);
    }
    catch (const std::exception &e) {
        fprintf(stderr, "creating log prefix failed, reason: %s\n", e.what());
        fflush(stderr);
    }

    bool isRaw = logType == Log::Type::Raw;
    try{
        if(isRaw)
        {
            if(limitedAction & Action::Print)
                this->print(log, false);
        }
        else
        {
            if(limitedAction & Action::Print)
                this->print(prefix + log, true);
        }
    }
    catch (const std::exception &e) {
        fprintf(stderr, "printing log failed, reason: %s\n", e.what());
        fflush(stderr);
    }

    try{
        if(isRaw)
        {
            if(limitedAction & Action::Save)
                this->saveFile(time + prefix + "\n""<<START RAW>>""\n" + log + "\n""<<END RAW>>");
        }
        else
        {
            if(limitedAction & Action::Save)
                this->saveFile(time + prefix + log);
        }
    }
    catch (const std::exception &e) {
        fprintf(stderr, "saving log to file failed, reason: %s\n", e.what());
        fflush(stderr);
    }

    try{
        if(isRaw)
        {
            if(limitedAction & Action::Session)
            {
                this->addSession(log, false);
                // this->addSession(logType, funName, log);
            }
        }
        else
        {
            if(limitedAction & Action::Session)
            {
                this->addSession(prefix + log, true);
                // this->addSession(logType, funName, log);
            }
        }
    }
    catch (const std::exception &e) {
        fprintf(stderr, "adding log to session failed, reason: %s\n", e.what());
        fflush(stderr);
    }
}

void Log::safeLog(Log::Type logType, cstr funName, cstr log, Action action)
{
    try {
        this->log(logType, funName, log, action);
    } catch (const std::exception &e) {
        fprintf(stderr, "logging failed, reason: %s\n", e.what());
        fflush(stderr);
    }
}

void Log::print(cstr content, bool newLine)
{
    fprintf(stdout, "%s%s", content.c_str(), newLine ? "\n" : "");
    fflush(stdout);
}

void Log::saveFile(cstr content)
{
    if(!m_outFile.is_open())
    {
        if(!std::filesystem::exists(outputDirectory))
        {
            if(!std::filesystem::create_directory(outputDirectory))
            {
                fprintf(stderr, "cannot create '%s' output directory\n", outputDirectory);
                fflush(stderr);
                return;
            }
        }

        m_fileName = outputDirectory + this->time(true) + ".log";

        m_outFile.open(m_fileName, std::ios::app);
        if(!m_outFile.is_open())
        {
            fprintf(stderr, "Error while creating log file!\n");
            fflush(stderr);
            return;
        }

        m_outFile << this->buildStartPrefix() << "\n";
    }

    m_outFile << content << "\n";
    m_outFile.flush(); /// required to save logs if application crashes
}

void Log::addSession(cstr content, bool newLine)
{
    m_currentSession += content + (newLine ? "\n" : "");
}

// void Log::addSession(Log::Type logType, const QString &funName, const QString &message)
// {
//     m_currentSession.addPart(logType, funName, message);
// }

std::string Log::Convert::vectorToString(std::vector<std::string> list)
{
    std::string str("[");
    for(const auto &i : list)
        str += "\"" + i + "\", ";
    return str + "\b\b]";
}
