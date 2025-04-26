#include "Log.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>
#include <cstring>

const char *logFile = "application_logs.log";

bool Log::firstLog = true;

// LogSession Log::currentSession = LogSession();
std::string Log::currentSession = std::string();
std::ofstream Log::outFile;

const char *Log::logTypeToStr(Type type)
{
    switch (type) {
    case Log::Type::Info:       return "Log::Type::Info";
    case Log::Type::Warning:    return "Log::Type::Warning";
    case Log::Type::Error:      return "Log::Type::Error";
    case Log::Type::Debug:      return "Log::Type::Debug";
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

void Log::info(cstr func, cstr log, Log::Action action)
{
    Log::safeLog(Type::Info, func, log, action);
}

void Log::warning(cstr func, cstr log, Log::Action action)
{
    Log::safeLog(Type::Warning, func, log, action);
}

void Log::error(cstr func, cstr log, Log::Action action)
{
    Log::safeLog(Type::Error, func, log, action);
}

void Log::debug(cstr func, cstr log, Log::Action action)
{
    Log::safeLog(Type::Debug, func, log, action);
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
    va_start(args, text.c_str());

    std::string str = Log::asprintf(text.c_str(), args);

    va_end(args);

    return str;
}


std::string Log::time()
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) % 1000;

    std::tm tm = *std::localtime(&now_time_t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << now_ms.count();

    return oss.str();
}

std::string Log::buildPrefix(Log::Type logType, cstr function, bool time)
{
    std::string prefix;

    // set time
    if(time)
        prefix = "[" + Log::time() +  "]" + " ";

    // set type
    switch (logType) {
    case Log::Type::Info:       prefix += "I ";         break;
    case Log::Type::Warning:    prefix += "W ###";      break;
    case Log::Type::Error:      prefix += "E ### ###";  break;
    case Log::Type::Debug:      prefix += "D ";         break;
    default:
        fprintf(stderr, "unknown type %s, using: '?'\n", Log::logTypeToStr(logType));
        fflush(stderr);
        prefix += "? ";
        break;
    }

    // set function name
    if(function.length() >= EST_FUNCTION_LENGTH)
        prefix += function;
    else
    {
        size_t fill = EST_FUNCTION_LENGTH - function.length() - prefix.size();
        prefix += std::string(fill, SHORTER_FUNCTION_FILL_CHARACTER);
        prefix += function;
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
    static const char *startText = "--- [APPLICATION STARTED] ---";
    static const std::string spaceText =
        std::string(EST_FUNCTION_LENGTH -8 -28 +3, '-');

    std::string prefix;
    prefix = "\n\n" "[" + Log::time() +  "]";

    return prefix + spaceText + startText + spaceText;
}

void Log::log(Log::Type logType, cstr function, cstr log, Log::Action action)
{
    Action limitedAction = Action( (action | Log::actionForceLowest) & Log::actionForceHighest );

    std::string logWithTime = buildPrefix(logType, function, true) + log;
    std::string logWithoutTime = buildPrefix(logType, function, false) + log;

    if(limitedAction & Action::Print)
        Log::print(logWithoutTime);

    if(limitedAction & Action::Save)
        Log::saveFile(logWithTime);

    try{
        if(limitedAction & Action::Session)
        {
            Log::addSession(logWithoutTime);
            // Log::addSession(logType, function, log);
        }
    }
    catch (...) {
        fprintf(stderr, "adding log to session failed\n");
        fflush(stderr);
    }

    if(firstLog)
        firstLog = false;
}

void Log::safeLog(Log::Type logType, cstr function, cstr log, Action action)
{
    try {
        Log::log(logType, function, log, action);
    } catch (...) {
        fprintf(stderr, "logging failed\n");
        fflush(stderr);
    }
}

void Log::print(cstr content)
{
    fprintf(stdout, "%s\n", content.c_str());
    fflush(stdout);
}

void Log::saveFile(cstr content)
{
    if(firstLog)
    {
        outFile.open(logFile, std::ios::app);
        if(!outFile.is_open())
        {
            fprintf(stderr, "Error while creating log file!\n");
            fflush(stderr);
            return;
        }

        outFile << Log::buildStartPrefix() << "\n";
    }

    outFile << content << "\n";
    // outFile.flush();
}

void Log::addSession(cstr content)
{
    Log::currentSession += content + "\n";
}

// void Log::addSession(Log::Type logType, const QString &function, const QString &message)
// {
//     Log::currentSession.addPart(logType, function, message);
// }

std::string Log::Convert::vectorToString(std::vector<std::string> list)
{
    std::string str("[");
    for(const auto &i : list)
        str += "\"" + i + "\", ";
    return str + "\b\b]";
}
