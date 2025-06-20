#include "Log.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>
#include <cmath>
#include <cstring>
#include <filesystem>

const char *version = "v1.8.0";
const char *debugLogsOutputDirectory = "logs/debug/";
const char *traceLogsOutputDirectory = "logs/trace/";
const char *traceLogsInfoFileName = "_program_start_time---program_end_time_.null";

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

Log::Log()
    : m_startTime{ this->time(true) }
{
    constexpr int fileLen = MAX_FILES_IN_PROJECT_COUNT_NUMBER_LENGTH;
    const int filesCount = Log::computeMaxNumberFromNumberLength(fileLen);
    m_filesPaths.reserve(filesCount);

    std::string logFilesName = m_startTime + ".log";
    Log::openFile(debugLogsOutputDirectory, logFilesName, m_debugLogFile);
    Log::openFile(traceLogsOutputDirectory, logFilesName, m_traceLogFile);
    this->logInfoAboutTraceProperties();

    /// create temporary file to inform about naming
    std::ofstream tmpFile;
    Log::openFile(traceLogsOutputDirectory, traceLogsInfoFileName, tmpFile);
    tmpFile.close();
}

Log::~Log()
{
    m_debugLogFile.close();
    m_traceLogFile.close();

    /// at the end rename files from start to range
    /// it allows to easy check time range in which application was open
    /// end as benefit it allows to view if application crashed (if there is no end time)

    std::string oldLogFilesName = m_startTime + ".log";

    std::string endTime = this->time(true);
    std::string newLogFilesName = m_startTime + "-" + endTime + ".log";

    std::filesystem::rename(debugLogsOutputDirectory + oldLogFilesName, debugLogsOutputDirectory + newLogFilesName);
    std::filesystem::rename(traceLogsOutputDirectory + oldLogFilesName, traceLogsOutputDirectory + newLogFilesName);
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

void Log::info(cstr func, cestr log, Log::Action action)
{
#if USE_QT_SUPPORT
    this->safeLog(Type::Info, func, log.toStdString(), action);
#else
    this->safeLog(Type::Info, func, log, action);
#endif /// USE_QT_SUPPORT
}

void Log::warning(cstr func, cestr log, Log::Action action)
{
#if USE_QT_SUPPORT
    this->safeLog(Type::Warning, func, log.toStdString(), action);
#else
    this->safeLog(Type::Warning, func, log, action);
#endif /// USE_QT_SUPPORT
}

void Log::error(cstr func, cestr log, Log::Action action)
{
#if USE_QT_SUPPORT
    this->safeLog(Type::Error, func, log.toStdString(), action);
#else
    this->safeLog(Type::Error, func, log, action);
#endif /// USE_QT_SUPPORT
}

void Log::debug(cstr func, cestr log, Log::Action action)
{
#if USE_QT_SUPPORT
    this->safeLog(Type::Debug, func, log.toStdString(), action);
#else
    this->safeLog(Type::Debug, func, log, action);
#endif /// USE_QT_SUPPORT
}

void Log::raw(cstr func, cestr log, Action action)
{
#if USE_QT_SUPPORT
    this->safeLog(Type::Raw, func, log.toStdString(), action);
#else
    this->safeLog(Type::Raw, func, log, action);
#endif /// USE_QT_SUPPORT
}

void Log::trace(cstr file, cstr func, int line, void *ptr, cestr args)
{
    std::string time;

    try{
        time = "[" + Log::time() +  "]" + " ";
    }
    catch (const std::exception &e) {
        fprintf(stderr, "creating time prefix failed, reason: %s\n", e.what());
        fflush(stderr);
    }

    /// create displayed format
    static constexpr int lineLen = MAX_LINE_INDEX_NUMBER_LENGTH_IN_TRACE_LOG;
    static constexpr int fileLen = MAX_FILES_IN_PROJECT_COUNT_NUMBER_LENGTH;
    const std::string lineFormat = "%" + std::to_string(lineLen) + "d";
    const std::string fileFormat = "%" + std::to_string(fileLen) + "d";
    std::string format = "T " +fileFormat+ "|" +lineFormat+ "|%p|%s|[%s]";

    /// compute index from file name - save space in trace file
    int filePathIndex = 0;
    std::string newFilePathInfo;
    auto fIt = m_filesPaths.find(file);
    if(fIt != m_filesPaths.end())
    {
        filePathIndex = fIt->second;
    }
    else
    {
        filePathIndex = m_filesPaths.size() +1;
        m_filesPaths[file] = filePathIndex;

#if USE_QT_SUPPORT
        newFilePathInfo = asprintf((fileFormat + "|%s\n").c_str(), filePathIndex, file.c_str()).toStdString();
#else
        newFilePathInfo = asprintf((fileFormat + "|%s\n").c_str(), filePathIndex, file.c_str());
#endif /// USE_QT_SUPPORT
    }

/// create trace line
#if USE_QT_SUPPORT
    std::string traceLine = asprintf(format.c_str(), filePathIndex, line, ptr, func.c_str(), args.toStdString().c_str()).toStdString();
#else
    std::string traceLine = asprintf(format.c_str(), filePathIndex, line, ptr, func.c_str(), args.c_str());
#endif /// USE_QT_SUPPORT

    try{
        this->saveTraceLogFile(newFilePathInfo + time + traceLine);
    }
    catch (const std::exception &e) {
        fprintf(stderr, "saving trace failed, reason: %s\n", e.what());
        fflush(stderr);
    }
}

estr Log::asprintf(const char *text, ...)
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

#if USE_QT_SUPPORT
    return std::string(buffer.get(), size).c_str();
#else
    return std::string(buffer.get(), size);
#endif /// USE_QT_SUPPORT
}

estr Log::asprintf(cestr text, ...)
{
#if USE_QT_SUPPORT
    const str _text = text.toStdString();
#else
    const str &_text = text;
#endif /// USE_QT_SUPPORT

    va_list args;
#pragma clang diagnostic ignored "-Wvarargs" /// qtcreator uses clang
#pragma GCC diagnostic ignored "-Wvarargs" /// compiler uses gcc
    va_start(args, _text.c_str());

    const estr str = Log::asprintf(_text.c_str(), args);

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

void Log::openFile(const char *directory, cstr fileName, std::ofstream &file)
{
    if(!std::filesystem::exists(directory))
    {
        if(!std::filesystem::create_directories(directory))
        {
            fprintf(stderr, "cannot create '%s' output directory\n", directory);
            fflush(stderr);
            return;
        }
    }

    std::string debugLogFilePath = directory + fileName;

    file.open(debugLogFilePath, std::ios::app);
    if(!file.is_open())
    {
        fprintf(stderr, "Error while creating log file!\n");
        fflush(stderr);
        return;
    }

    file << Log::buildStartPrefix() << "\n";
}

void Log::logInfoAboutTraceProperties()
{
    if(!m_traceLogFile.is_open())
        return;

    m_traceLogFile << MAX_LINE_INDEX_NUMBER_LENGTH_IN_TRACE_LOG
                   << " # MAX_LINE_INDEX_NUMBER_LENGTH_IN_TRACE_LOG\n"
                   << MAX_FILES_IN_PROJECT_COUNT_NUMBER_LENGTH
                   << " # MAX_FILES_IN_PROJECT_COUNT_NUMBER_LENGTH\n";
}

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
    prefix = /*"\n\n"*/ "[" + Log::time() +  "]";

    return prefix + spaceText + startText + spaceText;
}

size_t Log::computeMaxNumberFromNumberLength(int length)
{
    return static_cast<size_t>( std::pow(10, length) ) -1;
}

// size_t Log::increaseNumberToClosestTwoSquare(size_t number)
// {
//     if(!number || number == 1) return 1;
//     -- number; /// handle case where number is already the power of 2

//     static constexpr size_t st_max = static_cast<size_t>(-1);
//     size_t returnNumber = ~(st_max >> 1);

//     if(returnNumber & number) /// handle case where number is above largest power of 2 that can be storred
//     {
//         fprintf(stderr, "\n\n" "Can't process %llu number in %s" "\n\n\n", number, __PRETTY_FUNCTION__);
//         fflush(stderr);
//         return number;
//     }
//     // number >>= 1;

//     // while(returnNumber>0)
//     // {
//     //     if(returnNumber & number)
//     //         return number << 1;
//     // }

//     number |= number >> 1;
//     number |= number >> 2;
//     number |= number >> 4;
//     number |= number >> 8;
//     number |= number >> 16;
//     if constexpr (sizeof(size_t) > 4) {
//         number |= number >> 32;
//     }

//     return number + 1;
// }

void Log::log(Log::Type logType, cstr funName, cstr log, Log::Action action)
{
    Action limitedAction = Action( (action | Log::actionForceLowest) & Log::actionForceHighest );

    std::string time;
    std::string prefix;

    try{
        time = "[" + Log::time() +  "]" + " ";
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
                this->saveDebugLogFile(time + prefix + "\n""<<START RAW>>""\n" + log + "\n""<<END RAW>>");
        }
        else
        {
            if(limitedAction & Action::Save)
                this->saveDebugLogFile(time + prefix + log);
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

void Log::saveDebugLogFile(cstr content)
{
    if(!m_debugLogFile.is_open())
        return;

    m_debugLogFile << content << "\n";
    m_debugLogFile.flush(); /// required to save logs if application crashes
}

void Log::saveTraceLogFile(cstr content)
{
    if(!m_traceLogFile.is_open())
        return;

    m_traceLogFile << content << "\n";
    m_traceLogFile.flush(); /// required to save logs if application crashes
}

void Log::addSession(cstr content, bool newLine)
{
    m_currentSession += content + (newLine ? "\n" : "");
}

// void Log::addSession(Log::Type logType, const QString &funName, const QString &message)
// {
//     m_currentSession.addPart(logType, funName, message);
// }

#if USE_QT_SUPPORT
QString Log::Convert::listUrlToString(QList<QUrl> list)
{
    QString str("[");
    for(const auto &i : list)
        str += "\"" + i.toLocalFile() + "\", ";
    return str + "\b\b]";
}

QString Log::Convert::listStrToString(QList<QString> list)
{
    QString str("[");
    for(const auto &i : list)
        str += "\"" + i + "\", ";
    return str + "\b\b]";
}
#else
std::string Log::Convert::vectorToString(std::vector<std::string> list)
{
    std::string str("[");
    for(const auto &i : list)
        str += "\"" + i + "\", ";
    return str + "\b\b]";
}
#endif /// USE_QT_SUPPORT
