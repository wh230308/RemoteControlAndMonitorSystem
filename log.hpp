#ifndef BASE_LOG_LOG_HPP
#define BASE_LOG_LOG_HPP

#include <stdarg.h>

namespace base {
namespace log {
enum Level        // 日志输出级别
{
    kStartup   = 0x01,  // 启动级别
    kShutdown  = 0x02,  // 关闭级别
    kInfo      = 0x04,  // 普通信息级别
    kWarning   = 0x08,  // 警告级别
    kTrace     = 0x10,  // 跟踪级别
    kError     = 0x20,  // 错误级别
    kDebug     = 0x40,  // 调试级别

    kLevelMask = 0x7F,  // mask
};

enum StorageType        // 日志输出类型
{
    kScreen    = 0x01,  // 屏幕输出
    kDiskFile  = 0x02,  // 日志文件输出
    kSysLog    = 0x04,  // TODO: 系统日志，未实现
    kLogServer = 0x08,  // TODO: 日志服务器，未实现

    kStorageTypeMask = 0x0F,  // mask
};

enum FieldType          // 字段类型
{
    kTimestamp = 0x01,  // 输出时间标签
    kThreadId  = 0x02,  // 输出线程Id标签
    kFileLine  = 0x04,  // 输出在源代码文件和行数标签
    kFunction  = 0x08,  // 输出在源代码的函数名称

    kFieldTypeMask = 0x0F,  // mask
};

void print_v(
    unsigned int level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const char*  format,
    va_list ap);

void print(
    unsigned int level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const char*  format,
    ...);

void hexdump(
    unsigned int level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const void*  data,
    int size);

void set_directory(const char* path);
void set_prefix_name(const char* prefix_name);
void set_level(unsigned int level);
void set_storage_type(unsigned int storage_type);
void set_field_type(unsigned int field_type);
void set_reserve_days(int days);      // 日志保留天数，默认为30天

}  // namespace log
}  // namespace base

enum LogLevel  // 日志输出级别
{
    kStartup  = base::log::kStartup,   // 启动级别
    kShutdown = base::log::kShutdown,  // 关闭级别
    kInfo     = base::log::kInfo,      // 普通信息级别
    kWarning  = base::log::kWarning,   // 警告级别
    kTrace    = base::log::kTrace,     // 跟踪级别
    kError    = base::log::kError,     // 错误级别
    kDebug    = base::log::kDebug      // 调试级别
};

#define LOG_PRINT(level, ...) \
    base::log::print(level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_SHUTDOWN(...) \
    base::log::print(kShutdown, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_STARTUP(...) \
    base::log::print(kStartup, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_INFO(...) \
    base::log::print(kInfo, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_WARN(...) \
    base::log::print(kWarning, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_TRACE(...) \
    base::log::print(kTrace, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_ERROR(...) \
    base::log::print(kError, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_DEBUG(...) \
    base::log::print(kDebug, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LogHex(level, data, size) \
    base::log::hexdump(level, __FILE__, __LINE__, __FUNCTION__, data, size)

#endif  // BASE_LOG_LOG_HPP
