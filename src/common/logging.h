#ifndef NULLWARE_COMMON_LOGGING_H_
#define NULLWARE_COMMON_LOGGING_H_

#include <string>
#include <sstream>
#include <memory>
#include <mutex>

// Log levels
enum class LogLevel {
    VERBOSE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// Log output targets
enum class LogTarget {
    CONSOLE,
    FILE,
    SYSLOG,
    ALL
};

namespace nullware {

class Logger {
public:
    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }
    
    // Initialize the logger
    void Initialize(LogLevel level, LogTarget target, const std::string& log_file = "");
    
    // Set log level
    void SetLevel(LogLevel level);
    
    // Set log target
    void SetTarget(LogTarget target);
    
    // Set log file
    void SetLogFile(const std::string& filename);
    
    // Log messages
    void Log(LogLevel level, const std::string& message);
    void Log(LogLevel level, const std::ostringstream& message);
    
    // Convenience methods
    void Verbose(const std::string& message);
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
    void Fatal(const std::string& message);
    
    // Check if a log level is enabled
    bool IsLevelEnabled(LogLevel level) const;
    
    // Flush the log output
    void Flush();
    
private:
    Logger();
    ~Logger();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void WriteToConsole(const std::string& message);
    void WriteToFile(const std::string& message);
    void WriteToSyslog(const std::string& message);
    
    std::string FormatMessage(LogLevel level, const std::string& message);
    std::string LevelToString(LogLevel level);
    
    LogLevel level_;
    LogTarget target_;
    std::string log_file_;
    std::mutex mutex_;
    FILE* file_handle_;
};

// Logging macros for convenience
#define LOG(level) \
    if (nullware::Logger::GetInstance().IsLevelEnabled(level)) \
    nullware::Logger::GetInstance().Log(level, std::ostringstream().str())

#define LOG_VERBOSE(message) \
    if (nullware::Logger::GetInstance().IsLevelEnabled(nullware::LogLevel::VERBOSE)) \
    nullware::Logger::GetInstance().Verbose(message)

#define LOG_DEBUG(message) \
    if (nullware::Logger::GetInstance().IsLevelEnabled(nullware::LogLevel::DEBUG)) \
    nullware::Logger::GetInstance().Debug(message)

#define LOG_INFO(message) \
    if (nullware::Logger::GetInstance().IsLevelEnabled(nullware::LogLevel::INFO)) \
    nullware::Logger::GetInstance().Info(message)

#define LOG_WARNING(message) \
    if (nullware::Logger::GetInstance().IsLevelEnabled(nullware::LogLevel::WARNING)) \
    nullware::Logger::GetInstance().Warning(message)

#define LOG_ERROR(message) \
    if (nullware::Logger::GetInstance().IsLevelEnabled(nullware::LogLevel::ERROR)) \
    nullware::Logger::GetInstance().Error(message)

#define LOG_FATAL(message) \
    if (nullware::Logger::GetInstance().IsLevelEnabled(nullware::LogLevel::FATAL)) \
    nullware::Logger::GetInstance().Fatal(message)

// Stream-based logging
class LogStream {
public:
    LogStream(LogLevel level) : level_(level) {}
    
    ~LogStream() {
        if (nullware::Logger::GetInstance().IsLevelEnabled(level_)) {
            nullware::Logger::GetInstance().Log(level_, stream_.str());
        }
    }
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        stream_ << value;
        return *this;
    }
    
    LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
        stream_ << manip;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream stream_;
};

#define VLOG nullware::LogStream(nullware::LogLevel::VERBOSE)
#define DLOG nullware::LogStream(nullware::LogLevel::DEBUG)
#define ILOG nullware::LogStream(nullware::LogLevel::INFO)
#define WLOG nullware::LogStream(nullware::LogLevel::WARNING)
#define ELOG nullware::LogStream(nullware::LogLevel::ERROR)
#define FLOG nullware::LogStream(nullware::LogLevel::FATAL)

}  // namespace nullware

#endif  // NULLWARE_COMMON_LOGGING_H_
