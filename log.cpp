#include "log.hpp"
#include <mutex>
#include <vector>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#include <share.h>
#else
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif

namespace base {
namespace log {
namespace os {
struct SystemTime
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int milli_second;
};

#ifdef _WIN32
SystemTime get_local_time()
{
    SYSTEMTIME now;
    ::GetLocalTime(&now);
    return SystemTime{
        now.wYear,
        now.wMonth,
        now.wDay,
        now.wHour,
        now.wMinute,
        now.wSecond,
        now.wMilliseconds
    };
}

SystemTime get_local_time(time_t seconds)
{
    struct tm tm_now;
    localtime_s(&tm_now, &seconds);

    return SystemTime{
        tm_now.tm_year + 1900,
        tm_now.tm_mon + 1,
        tm_now.tm_mday,
        tm_now.tm_hour,
        tm_now.tm_min,
        tm_now.tm_sec,
        0
    };
}
#else
SystemTime get_local_time()
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);

    struct tm tm_now;
    localtime_r(&tv_now.tv_sec, &tm_now);

    return SystemTime{
        tm_now.tm_year + 1900,
        tm_now.tm_mon + 1,
        tm_now.tm_mday,
        tm_now.tm_hour,
        tm_now.tm_min,
        tm_now.tm_sec,
        (int)tv_now.tv_usec / 1000
    };
}

SystemTime get_local_time(time_t seconds)
{
    struct tm tm_now;
    localtime_r(&seconds, &tm_now);

    return SystemTime{
        tm_now.tm_year + 1900,
        tm_now.tm_mon + 1,
        tm_now.tm_mday,
        tm_now.tm_hour,
        tm_now.tm_min,
        tm_now.tm_sec,
        0
    };
}
#endif

#ifdef _WIN32
bool make_directory(const char* directory)
{
    return (0 == _mkdir(directory));
}
#else
bool make_directory(const char* directory)
{
    return (0 == mkdir(directory, 0777));
}
#endif

uint32_t get_thread_id()
{
#ifdef _WIN32
    return ::GetCurrentThreadId();
#else
    return ::syscall(SYS_gettid);
#endif
}

int vsnprintf(char *buffer, size_t size, const char *format, va_list ap)
{
#ifdef _WIN32
    return ::vsnprintf_s(buffer, size, _TRUNCATE, format, ap);
#else
    return ::vsnprintf(buffer, size, format, ap);
#endif
}

#ifdef _WIN32
const char kSystemSlash = '\\';
#else
const char kSystemSlash = '/';
#endif

#ifdef _WIN32
#define snprintf(buffer, size, ...) \
    _snprintf_s(buffer, size, _TRUNCATE, __VA_ARGS__)
#endif

}  // namespace os

namespace detail
{

const int  kMaxBufferSize         = 2048;
const char kHeaderTimestamp[]     = "[%02u:%02u:%02u.%03u]|";
const char kHeaderThreadId[]      = "[%04x]|";
const char kHeaderFileAndLine[]   = "[%s:%d]|";
const char kHeaderFunction[]      = "[%s]|";
const char kHeaderLogLevel[]      = "[%s]|";
const char kLogFilenameSuffix[]   = "%04u%02u%02u.log";
const int  kLogFilenameSuffixSize = 16;


class Log
{
private:
    Log();
    ~Log();

public:
    static Log& instance();

public:
    void set_directory(const char* path);
    void set_prefix_name(const char* prefix_name);
    void set_level(unsigned int level);
    void set_storage_type(unsigned int storage_type);
    void set_field_type(unsigned int field_type);
    void set_reserve_days(int days);

    void print(unsigned int level, const char* filename, unsigned int line,
        const char* function, const char* format, va_list ap);
    void hexdump(unsigned int level, const char* filename, unsigned int line,
        const char* function, const void* data, int size);

private:
    void write(unsigned int level, const char* message, int size, 
        const os::SystemTime& system_time);
    void write_to_screen(unsigned int level, const char *message, int size);
    void write_to_stderror(unsigned int level, const char *message, int size);
    void wirte_to_file(const char* message, const os::SystemTime& system_time);
    void write_to_server(const char* message, int size);
    void clear_log_file(unsigned int reserve_day);

private:
    int format_line_head(
        char* buffer,
        int size,
        unsigned int level,
        const char*  filename,
        unsigned int line,
        const char*  function,
        const os::SystemTime& system_time);
    const char* level_string(unsigned int level);
    const char* trim_filename(const char* filename);
    int sprintf_filename_suffix(
        char* buffer,
        int buffer_size,
        const os::SystemTime& system_time);
    int sprintf_timestamp(
        unsigned int head_type,
        char* buffer,
        int buffer_size,
        const os::SystemTime& system_time);
    int sprintf_thread_id(
        unsigned int head_type,
        char* buffer,
        int buffer_size,
        unsigned int thread_id);
    int sprintf_file_and_line(
        unsigned int head_type,
        char* buffer,
        int buffer_size,
        const char* filename,
        unsigned int line);
    int sprintf_level(
        unsigned int head_type,
        char* buffer,
        int buffer_size,
        unsigned int level);
    int sprintf_function(
        unsigned int head_type,
        char* buffer,
        int buffer_size,
        const char* function);

private:
    std::string get_log_filename(const char* suffix);
    std::string get_log_filename(time_t log_time);

private:
    std::mutex lock_;
    unsigned int storage_type_ = kStorageTypeMask;
    unsigned int field_type_   = kFieldTypeMask;
    unsigned int level_ = kLevelMask;
    unsigned int reserve_days_ = 30;  // 日志保留天数，默认30天
    int filename_depth_ = 1;
    std::string log_directory_ = "./log";
    std::string prefix_name_ = "N_A";
    char old_suffix_[kLogFilenameSuffixSize];
    char new_suffix_[kLogFilenameSuffixSize];
    FILE* file_ = NULL;
};

Log::Log()
{
}

Log::~Log()
{
}

Log& Log::instance()
{
    static Log log;
    return log;
}

void Log::set_directory(const char* path)
{
    log_directory_.assign(path);
    if (*log_directory_.rbegin() != os::kSystemSlash)
    {
        log_directory_.push_back(os::kSystemSlash);
    }
    os::make_directory(log_directory_.c_str());
}

void Log::set_prefix_name(const char* prefix_name)
{
    prefix_name_ = prefix_name;
}

void Log::set_level(unsigned int level)
{
    level_ = level;
}

void Log::set_storage_type(unsigned int storage_type)
{
    storage_type_ = storage_type;
}

void Log::set_field_type(unsigned int field_type)
{
    field_type_ = field_type;
}

void Log::set_reserve_days(int days)
{
    reserve_days_ = days;
}

void Log::print(
    unsigned int level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const char*  format,
    va_list ap)
{
    if ((level_ & level) == 0) return;

    // 日志时间
    os::SystemTime system_time = os::get_local_time();

    // 初始化行头
    char buffer[kMaxBufferSize + 2];  // 结尾2个字符'\n\0'
    int pos = format_line_head(buffer, kMaxBufferSize, level, filename,
        line, function, system_time);

    // 格式化打印数据
    pos += os::vsnprintf(buffer + pos, kMaxBufferSize - pos, format, ap);
	if (pos > kMaxBufferSize) pos = kMaxBufferSize - 1;
    buffer[pos++] = '\n';
    buffer[pos++] = 0;

    // 打印输出
    write(level, buffer, pos, system_time);
}

void Log::hexdump(
    unsigned int level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const void*  data,
    int size)
{
    if (size <= 0) return ;
    if ((level_ & level) == 0) return;

    // 日志时间
    os::SystemTime system_time = os::get_local_time();

    // 初始化行头
    std::vector<char> buffer(size * 3 + 2);
    uint32_t pos = format_line_head(buffer.data(), buffer.size(), level, filename,
        line, function, system_time);
    buffer[pos++] = '\n';

    // 格式化打印数据
    for (int i = 0; i < size; i++)
    {
        if (pos + 3 > buffer.size()) break;
        pos += snprintf(buffer.data() + pos, buffer.size() - pos, "%02x", 
            *((unsigned char*)data + i));
        if (((i + 1) % 32) == 0 && pos < buffer.size())
		{
            buffer[pos++] = '\n'; 
			continue;
		}
        if (((i + 1) % 4) == 0 && pos < buffer.size())
		{
            buffer[pos++] = ' ';
		}
        if (((i + 1) % 16) == 0 && pos < buffer.size())
		{
            buffer[pos++] = ' ';
		}
    }

    if (buffer[pos - 1] != '\n')
        buffer[pos++] = '\n';
    buffer[pos++] = 0;

    // 打印输出
    write(level, buffer.data(), pos, system_time);
}

void Log::write(
    unsigned int level,
    const char* message,
    int size,
    const os::SystemTime& system_time)
{
    std::lock_guard<std::mutex> guard(lock_);

    if (storage_type_ & kScreen)
        write_to_screen(level, message, size);
    if (storage_type_ & kDiskFile)
        wirte_to_file(message, system_time);
    if (storage_type_ & kLogServer)
        write_to_server(message, size);
}

void Log::write_to_screen(unsigned int level, const char *message, int size)
{
    if (level == kError || level == kWarning) {
        write_to_stderror(level, message, size);
    }
    else {
        fwrite(message, size - 1, 1, stderr);
    }
}

void Log::write_to_stderror(unsigned int level, const char *message, int size)
{
    const HANDLE stderr_handle = GetStdHandle(STD_ERROR_HANDLE);

    // Gets the current text color.
    CONSOLE_SCREEN_BUFFER_INFO buffer_info;
    GetConsoleScreenBufferInfo(stderr_handle, &buffer_info);
    const WORD old_color_attrs = buffer_info.wAttributes;

    // We need to flush the stream buffers into the console before each
    // SetConsoleTextAttribute call lest it affect the text that is already
    // printed but has not yet reached the console.
    WORD color = 0;
    if (level == kError) {
        color = FOREGROUND_RED;
    }
    else if (level == kWarning) {
        color = FOREGROUND_GREEN | FOREGROUND_RED;
    }
    fflush(stderr);
    SetConsoleTextAttribute(stderr_handle, color | FOREGROUND_INTENSITY);
    fwrite(message, size-1, 1, stderr);
    fflush(stderr);
    // Restores the text color.
    SetConsoleTextAttribute(stderr_handle, old_color_attrs);
}

void Log::wirte_to_file(const char* message, const os::SystemTime& system_time)
{
    sprintf_filename_suffix(new_suffix_, sizeof(new_suffix_), system_time);
    if (strcmp(old_suffix_, new_suffix_) != 0)
    {
        if (file_ != NULL) fclose(file_);
        clear_log_file(reserve_days_);
        std::string path = get_log_filename(new_suffix_);
#ifdef _WIN32
        file_ = _fsopen(path.c_str(), "a+", _SH_DENYWR);
#else
        file_ = fopen(path.c_str(), "a+");
#endif
        memcpy(old_suffix_, new_suffix_, sizeof(old_suffix_));
    }
    if (file_ != NULL)
    {
        fprintf(file_, "%s", message);
        fflush(file_);
    }
}

void Log::write_to_server(const char* /*message*/, int /*size*/)
{
}

void Log::clear_log_file(unsigned int reserve_day)
{
    time_t seconds = time(NULL) - reserve_day * 86400;
    for (seconds -= 24 * 3600; ; seconds -= 24 * 3600)
    {
        std::string filename = get_log_filename(seconds);
        if (::remove(filename.c_str()) != 0)
        {
            break;
        }
    }
}

int Log::format_line_head(
    char* buffer,
    int size,
    unsigned int level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const os::SystemTime& system_time)
{
    int pos = 0;
    pos += sprintf_timestamp(field_type_, buffer + pos,
        size - pos, system_time);
	if (pos >= size) return size;
    pos += sprintf_thread_id(field_type_, buffer + pos,
        size - pos, os::get_thread_id());
	if (pos >= size) return size;
    pos += sprintf_level(field_type_, buffer + pos,
        size - pos, level);
	if (pos >= size) return size;
    pos += sprintf_file_and_line(field_type_, buffer + pos,
        size - pos, filename, line);
	if (pos >= size) return size;
    pos += sprintf_function(field_type_, buffer + pos,
        size - pos, function);
	if (pos >= size) return size;
    return pos;
}

const char* Log::level_string(unsigned int level)
{
    static const char* level_strings[] =
    {
        "Startup",
        "Shutdown",
        "Info",
        "Warning",
        "Trace",
        "Error",
        "Debug",
        "Unknown"
    };

    switch (level)
    {
    case kStartup:
        return level_strings[0];
    case kShutdown:
        return level_strings[1];
    case kInfo:
        return level_strings[2];
    case kWarning:
        return level_strings[3];
    case kTrace:
        return level_strings[4];
    case kError:
        return level_strings[5];
    case kDebug:
        return level_strings[6];
    }

    return level_strings[7];
}

int Log::sprintf_filename_suffix(
    char* buffer,
    int buffer_size,
    const os::SystemTime& system_time)
{
    return snprintf(
        buffer,
        buffer_size,
        kLogFilenameSuffix,
        system_time.year,
        system_time.month,
        system_time.day);
}

int Log::sprintf_timestamp(
    unsigned int head_type,
    char* buffer,
    int buffer_size,
    const os::SystemTime& system_time)
{
    if (head_type & kTimestamp)
    {
        return snprintf(
            (char*)buffer,
            buffer_size,
            kHeaderTimestamp,
            system_time.hour,
            system_time.minute,
            system_time.second,
            system_time.milli_second);
    }
    return 0;
}

int Log::sprintf_thread_id(
    unsigned int head_type,
    char* buffer,
    int buffer_size,
    unsigned int thread_id)
{
    if (head_type & kThreadId)
    {
        return snprintf(
            buffer,
            buffer_size,
            kHeaderThreadId,
            thread_id);
    }
    return 0;
}

const char* Log::trim_filename(const char* filename)
{
    // 只取最后filename_depth_级目录
    const char* r = filename + strlen(filename) - 1;
    int slash_count = 0;
    while (r-- != filename)
    {
        if (*r == os::kSystemSlash) slash_count++;
        if (slash_count == filename_depth_ + 1) return ++r;
    }
    return filename;
}

int Log::sprintf_file_and_line(
    unsigned int head_type,
    char* buffer,
    int buffer_size,
    const char* filename,
    unsigned int line)
{
    if (head_type & kFileLine)
    {
        return snprintf(buffer, buffer_size, kHeaderFileAndLine,
            trim_filename(filename), line);
    }
    return 0;
}

int Log::sprintf_level(
    unsigned int /*head_type*/,
    char* buffer,
    int buffer_size,
    unsigned int level)
{
    return snprintf(
        buffer,
        buffer_size,
        kHeaderLogLevel,
        level_string(level));
}

int Log::sprintf_function(
    unsigned int head_type,
    char* buffer,
    int buffer_size,
    const char* function)
{
    if (head_type & kFunction)
    {
        return snprintf(
            buffer,
            buffer_size,
            kHeaderFunction,
            function);
    }
    return 0;
}

std::string Log::get_log_filename(const char* suffix)
{
    return log_directory_ + prefix_name_ + suffix;
}

std::string Log::get_log_filename(time_t seconds)
{
    os::SystemTime system_time = os::get_local_time(seconds);
    char suffix[kLogFilenameSuffixSize];
    sprintf_filename_suffix(suffix, sizeof(suffix), system_time);
    return get_log_filename(suffix);
}

}  // namespace detail

void print_v(
    unsigned int level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const char*  format,
    va_list ap)
{
    detail::Log::instance().print(level, filename, line, function, format, ap);
}

void print(
    unsigned int level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const char*  format,
    ...)
{
    va_list ap;
    va_start(ap, format);
    print_v(level, filename, line, function, format, ap);
    va_end(ap);
}

void hexdump(
    unsigned int level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const void*  data,
    int size)
{
    detail::Log::instance().hexdump(level, filename, line, function, 
        data, size);
}

void set_directory(const char* path)
{
    detail::Log::instance().set_directory(path);
}

void set_prefix_name(const char* prefix_name)
{
    detail::Log::instance().set_prefix_name(prefix_name);
}

void set_level(unsigned int level)
{
    detail::Log::instance().set_level(level);
}

void set_storage_type(unsigned int storage_type)
{
    detail::Log::instance().set_storage_type(storage_type);
}

void set_field_type(unsigned int field_type)
{
    detail::Log::instance().set_field_type(field_type);
}

void set_reserve_days(int days)
{
    if (days < 1) days = 1;
    detail::Log::instance().set_reserve_days(days);
}

}  // namespace log
}  // namespace base
