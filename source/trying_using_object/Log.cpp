#include "Log.h"

Log::Log()
    : m_logFileName{LOG_FILE}
    , m_maxLogFileSize{MAX_LOG_FILE_SIZE}
    , m_logTrimFileSize{TRIM_LOG_FILE_SIZE}
    , m_estFunctionSize{EST_FUNCTION_LENGTH}
    , m_shorterFunctionFillCharacter{SHORTER_FUNCTION_FILL_CHARACTER}
    , m_spaceAfterFunctionNameEnabled{SPACE_AFTER_FUNCTION_NAME_ENABLED}
    , m_spaceAfterFunctionNameSize{SPACE_AFTER_FUNCTION_NAME_SIZE}
    , m_spaceAfterFunctionNameCharacter{SPACE_AFTER_FUNCTION_NAME_CHARACTER}
    , m_firstLog{true}
{

    m_firstLog = true
}

Log::~Log()
{

}

Log *Log::getInstance()
{
    static Log log;
    return &log;
}



void Log::warning(cstr func, cstr log, Action action)
{
    Log::safeLog(Type::Warning, func, log, action);
}

void Log::error(cstr func, cstr log, Action action)
{
    Log::safeLog(Type::Error, func, log, action);
}

void Log::debug(cstr func, cstr log, Action action)
{
    Log::safeLog(Type::Debug, func, log, action);
}

std::string Log::time()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    return currentDateTime.toString("yyyy-MM-dd HH:mm:ss.zzz");
}

std::string Log::buildPrefix(Type logType, cstr function, bool time)
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

std::string Log::buildStartPrefix()
{
    QString prefix;
    // set time
    prefix = "[" + Log::time() +  "]" + "   ";

    return prefix + "--- [APPLICATION STARTED] ---";
}

const std::string &Log::getLogFile() const
{
    return m_logFileName;
}

size_t Log::getMaxLogFileSize() const
{
    return m_maxLogFileSize;
}

size_t Log::getTrimLogFileSize() const
{
    return m_logTrimFileSize;
}

int Log::getEstimatedFunctionLength() const
{
    return m_estFunctionSize;
}

char Log::getShorterFunctionFillCharacter() const
{
    return m_shorterFunctionFillCharacter;
}

bool Log::getSpaceAfterFunctionNameEnable() const
{
    return m_spaceAfterFunctionNameEnabled;
}

int Log::getSpaceAfterFunctionNameSize() const
{
    return m_spaceAfterFunctionNameSize;
}

char Log::getSpaceAfterFunctionNameCharacter() const
{
    return m_spaceAfterFunctionNameCharacter;
}

void Log::setLogFile(const std::string &filePath)
{
    m_logFileName = filePath;
}

void Log::setMaxLogFileSize(size_t fileSize)
{
    m_maxLogFileSize = fileSize;
}

void Log::setTrimLogFileSize(size_t fileSize)
{
    m_logTrimFileSize = fileSize;
}

void Log::setEstimatedFunctionLength(int length)
{
    m_estFunctionSize = length;
}

void Log::setShorterFunctionFillCharacter(char fillCharacter)
{
    m_shorterFunctionFillCharacter = fillCharacter;
}

void Log::setSpaceAfterFunctionNameEnable(bool enable)
{
    m_spaceAfterFunctionNameEnabled = enable;
}

void Log::setSpaceAfterFunctionNameSize(int length)
{
    m_spaceAfterFunctionNameSize = length;
}

void Log::setSpaceAfterFunctionNameCharacter(char gapCharacter)
{
    m_spaceAfterFunctionNameCharacter = gapCharacter;
}



