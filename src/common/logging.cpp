#include "src/common/logging.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef NULLWARE_PLATFORM_WINDOWS
#include <windows.h>
#elif defined(NULLWARE_PLATFORM_LINUX)
#include <syslog.h>
#endif

namespace nullware {

Logger::Logger() 
    : level_(LogLevel::INFO), 
      target_(LogTarget::CONSOLE), 
      file_handle_(nullptr) {}

Logger::~Logger() {
    if (file_handle_) {
        fclose(file_handle_);
        file_handle_ = nullptr;
    }
}

void Logger::Initialize(LogLevel level, LogTarget target, const std::string& log_file) {
    level_ = level;
    target_ = target;
    
    if (!log_file.empty()) {
        SetLogFile(log_file);
    }
    
    LOG_INFO("Logger initialized with level: " + LevelToString(level));
}

void Logger::SetLevel(LogLevel level) {
    level_ = level;
}

void Logger::SetTarget(LogTarget target) {
    target_ = target;
}

void Logger::SetLogFile(const std::string& filename) {
    if (file_handle_) {
        fclose(file_handle_);
        file_handle_ = nullptr;
    }
    
    log_file_ = filename;
    file_handle_ = fopen(filename.c_str(), "a");
    
    if (file_handle_) {
        LOG_INFO("Logging to file: " + filename);
    } else {
        LOG_WARNING("Failed to open log file: " + filename);
    }
}

void Logger::Log(LogLevel level, const std::string& message) {
    if (!IsLevelEnabled(level)) return;
    
    std::string formatted = FormatMessage(level, message);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    switch (target_) {
        case LogTarget::CONSOLE:
            WriteToConsole(formatted);
            break;
        case LogTarget::FILE:
            WriteToFile(formatted);
            break;
        case LogTarget::SYSLOG:
            WriteToSyslog(formatted);
            break;
        case LogTarget::ALL:
            WriteToConsole(formatted);
            WriteToFile(formatted);
            WriteToSyslog(formatted);
            break;
    }
}

void Logger::Log(LogLevel level, const std::ostringstream& message) {
    Log(level, message.str());
}

void Logger::Verbose(const std::string& message) {
    Log(LogLevel::VERBOSE, message);
}

void Logger::Debug(const std::string& message) {
    Log(LogLevel::DEBUG, message);
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::INFO, message);
}

void Logger::Warning(const std::string& message) {
    Log(LogLevel::WARNING, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::ERROR, message);
}

void Logger::Fatal(const std::string& message) {
    Log(LogLevel::FATAL, message);
}

bool Logger::IsLevelEnabled(LogLevel level) const {
    return level >= level_;
}

void Logger::Flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (file_handle_) {
        fflush(file_handle_);
    }
    
    if (target_ == LogTarget::CONSOLE || target_ == LogTarget::ALL) {
        std::cout.flush();
        std::cerr.flush();
    }
}

void Logger::WriteToConsole(const std::string& message) {
    if (level_ <= LogLevel::WARNING) {
        std::cerr << message << std::endl;
    } else {
        std::cout << message << std::endl;
    }
}

void Logger::WriteToFile(const std::string& message) {
    if (file_handle_) {
        fprintf(file_handle_, "%s\n", message.c_str());
        fflush(file_handle_);
    }
}

void Logger::WriteToSyslog(const std::string& message) {
#ifdef NULLWARE_PLATFORM_LINUX
    int priority = LOG_INFO;
    
    // Map log level to syslog priority
    switch (level_) {
        case LogLevel::VERBOSE:
        case LogLevel::DEBUG:
            priority = LOG_DEBUG;
            break;
        case LogLevel::INFO:
            priority = LOG_INFO;
            break;
        case LogLevel::WARNING:
            priority = LOG_WARNING;
            break;
        case LogLevel::ERROR:
            priority = LOG_ERR;
            break;
        case LogLevel::FATAL:
            priority = LOG_CRIT;
            break;
    }
    
    syslog(priority, "%s", message.c_str());
#elif defined(NULLWARE_PLATFORM_WINDOWS)
    // Windows Event Log
    HANDLE hEventLog = RegisterEventSource(NULL, L"NullWare");
    if (hEventLog) {
        WORD type = EVENTLOG_INFORMATION_TYPE;
        
        switch (level_) {
            case LogLevel::WARNING:
                type = EVENTLOG_WARNING_TYPE;
                break;
            case LogLevel::ERROR:
            case LogLevel::FATAL:
                type = EVENTLOG_ERROR_TYPE;
                break;
            default:
                type = EVENTLOG_INFORMATION_TYPE;
                break;
        }
        
        std::wstring wide_message(message.begin(), message.end());
        ReportEvent(hEventLog, type, 0, 0, NULL, 1, 0, 
                   reinterpret_cast<LPCWSTR>(const_cast<wchar_t*>(wide_message.c_str())), NULL);
        DeregisterEventSource(hEventLog);
    }
#endif
}

std::string Logger::FormatMessage(LogLevel level, const std::string& message) {
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    
    std::tm tm_struct;
#ifdef NULLWARE_PLATFORM_WINDOWS
    localtime_s(&tm_struct, &now_time);
#else
    localtime_r(&now_time, &tm_struct);
#endif
    
    std::ostringstream time_stream;
    time_stream << std::put_time(&tm_struct, "%Y-%m-%d %H:%M:%S");
    
    // Get thread ID
    std::ostringstream thread_stream;
#ifdef NULLWARE_PLATFORM_WINDOWS
    thread_stream << std::this_thread::get_id();
#else
    thread_stream << std::this_thread::get_id();
#endif
    
    return "[" + time_stream.str() + "] [" + LevelToString(level) + "] [" + 
           thread_stream.str() + "] " + message;
}

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::VERBOSE: return "VERBOSE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

}  // namespace nullware
