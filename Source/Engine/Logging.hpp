// Copyright(c) 2022, KaoruXun All rights reserved.

// Logging is based on https://github.com/emilk/loguru by ernerfeldt

#if defined(METADOT_LOGGING_IMPLEMENTATION)
#error "You are defining METADOT_LOGGING_IMPLEMENTATION. This is for older versions of Loguru. You should now instead include loguru.cpp (or build it and link with it)"
#endif

// Disable all warnings from gcc/clang:
#if defined(__clang__)
#pragma clang system_header
#elif defined(__GNUC__)
#pragma GCC system_header
#endif

#ifndef METADOT_LOGGING_HAS_DECLARED_FORMAT_HEADER
#define METADOT_LOGGING_HAS_DECLARED_FORMAT_HEADER

// Semantic versioning. Loguru version can be printed with printf("%d.%d.%d", METADOT_LOGGING_VERSION_MAJOR, METADOT_LOGGING_VERSION_MINOR, METADOT_LOGGING_VERSION_PATCH);
#define METADOT_LOGGING_VERSION_MAJOR 2
#define METADOT_LOGGING_VERSION_MINOR 1
#define METADOT_LOGGING_VERSION_PATCH 0

#if defined(_MSC_VER)
#include <sal.h>// Needed for _In_z_ etc annotations
#endif

#if defined(__linux__) || defined(__APPLE__)
#define METADOT_LOGGING_SYSLOG 1
#else
#define METADOT_LOGGING_SYSLOG 0
#endif

// ----------------------------------------------------------------------------

#ifndef METADOT_LOGGING_EXPORT
// Define to your project's export declaration if needed for use in a shared library.
#define METADOT_LOGGING_EXPORT
#endif

#ifndef METADOT_LOGGING_SCOPE_TEXT_SIZE
// Maximum length of text that can be printed by a LOG_SCOPE.
// This should be long enough to get most things, but short enough not to clutter the stack.
#define METADOT_LOGGING_SCOPE_TEXT_SIZE 196
#endif

#ifndef METADOT_LOGGING_FILENAME_WIDTH
// Width of the column containing the file name
#define METADOT_LOGGING_FILENAME_WIDTH 18
#endif

#ifndef METADOT_LOGGING_THREADNAME_WIDTH
// Width of the column containing the thread name
#define METADOT_LOGGING_THREADNAME_WIDTH 14
#endif

#ifndef METADOT_LOGGING_SCOPE_TIME_PRECISION
// Resolution of scope timers. 3=ms, 6=us, 9=ns
#define METADOT_LOGGING_SCOPE_TIME_PRECISION 3
#endif

#ifdef METADOT_LOGGING_CATCH_SIGABRT
#error "You are defining METADOT_LOGGING_CATCH_SIGABRT. This is for older versions of Loguru. You should now instead set the options passed to Logging::init"
#endif

#ifndef METADOT_LOGGING_VERBOSE_SCOPE_ENDINGS
// Show milliseconds and scope name at end of scope.
#define METADOT_LOGGING_VERBOSE_SCOPE_ENDINGS 1
#endif

#ifndef METADOT_LOGGING_REDEFINE_ASSERT
#define METADOT_LOGGING_REDEFINE_ASSERT 0
#endif

#ifndef METADOT_LOGGING_WITH_STREAMS
#define METADOT_LOGGING_WITH_STREAMS 0
#endif

#ifndef METADOT_LOGGING_REPLACE_GLOG
#define METADOT_LOGGING_REPLACE_GLOG 0
#endif

#if METADOT_LOGGING_REPLACE_GLOG
#undef METADOT_LOGGING_WITH_STREAMS
#define METADOT_LOGGING_WITH_STREAMS 1
#endif

#if defined(METADOT_LOGGING_UNSAFE_SIGNAL_HANDLER)
#error "You are defining METADOT_LOGGING_UNSAFE_SIGNAL_HANDLER. This is for older versions of Loguru. You should now instead set the unsafe_signal_handler option when you call Logging::init."
#endif

#if METADOT_LOGGING_IMPLEMENTATION
#undef METADOT_LOGGING_WITH_STREAMS
#define METADOT_LOGGING_WITH_STREAMS 1
#endif

#ifndef METADOT_LOGGING_USE_FMTLIB
#define METADOT_LOGGING_USE_FMTLIB 1
#endif

#ifndef METADOT_LOGGING_USE_LOCALE
#define METADOT_LOGGING_USE_LOCALE 0
#endif

#ifndef METADOT_LOGGING_WITH_FILEABS
#define METADOT_LOGGING_WITH_FILEABS 0
#endif

#ifndef METADOT_LOGGING_RTTI
#if defined(__clang__)
#if __has_feature(cxx_rtti)
#define METADOT_LOGGING_RTTI 1
#endif
#elif defined(__GNUG__)
#if defined(__GXX_RTTI)
#define METADOT_LOGGING_RTTI 1
#endif
#elif defined(_MSC_VER)
#if defined(_CPPRTTI)
#define METADOT_LOGGING_RTTI 1
#endif
#endif
#endif

#ifdef METADOT_LOGGING_USE_ANONYMOUS_NAMESPACE
#define METADOT_LOGGING_ANONYMOUS_NAMESPACE_BEGIN namespace {
#define METADOT_LOGGING_ANONYMOUS_NAMESPACE_END }
#else
#define METADOT_LOGGING_ANONYMOUS_NAMESPACE_BEGIN
#define METADOT_LOGGING_ANONYMOUS_NAMESPACE_END
#endif

// --------------------------------------------------------------------
// Utility macros

#define METADOT_LOGGING_CONCATENATE_IMPL(s1, s2) s1##s2
#define METADOT_LOGGING_CONCATENATE(s1, s2) METADOT_LOGGING_CONCATENATE_IMPL(s1, s2)

#ifdef __COUNTER__
#define METADOT_LOGGING_ANONYMOUS_VARIABLE(str) METADOT_LOGGING_CONCATENATE(str, __COUNTER__)
#else
#define METADOT_LOGGING_ANONYMOUS_VARIABLE(str) METADOT_LOGGING_CONCATENATE(str, __LINE__)
#endif

#if defined(__clang__) || defined(__GNUC__)
// Helper macro for declaring functions as having similar signature to printf.
// This allows the compiler to catch format errors at compile-time.
#define METADOT_LOGGING_PRINTF_LIKE(fmtarg, firstvararg)                                           \
    __attribute__((__format__(__printf__, fmtarg, firstvararg)))
#define METADOT_LOGGING_FORMAT_STRING_TYPE const char *
#elif defined(_MSC_VER)
#define METADOT_LOGGING_PRINTF_LIKE(fmtarg, firstvararg)
#define METADOT_LOGGING_FORMAT_STRING_TYPE _In_z_ _Printf_format_string_ const char *
#else
#define METADOT_LOGGING_PRINTF_LIKE(fmtarg, firstvararg)
#define METADOT_LOGGING_FORMAT_STRING_TYPE const char *
#endif

// Used to mark log_and_abort for the benefit of the static analyzer and optimizer.
#if defined(_MSC_VER)
#define METADOT_LOGGING_NORETURN __declspec(noreturn)
#else
#define METADOT_LOGGING_NORETURN __attribute__((noreturn))
#endif

#if defined(_MSC_VER)
#define METADOT_LOGGING_PREDICT_FALSE(x) (x)
#define METADOT_LOGGING_PREDICT_TRUE(x) (x)
#else
#define METADOT_LOGGING_PREDICT_FALSE(x) (__builtin_expect(x, 0))
#define METADOT_LOGGING_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#endif

#if METADOT_LOGGING_USE_FMTLIB
#include <fmt/format.h>
#define METADOT_LOGGING_FMT(x) "{:" #x "}"
#else
#define METADOT_LOGGING_FMT(x) "%" #x
#endif

#ifdef _WIN32
#define STRDUP(str) _strdup(str)
#else
#define STRDUP(str) strdup(str)
#endif

#include <stdarg.h>

// --------------------------------------------------------------------
METADOT_LOGGING_ANONYMOUS_NAMESPACE_BEGIN

namespace Logging {
    // Simple RAII ownership of a char*.
    class METADOT_LOGGING_EXPORT Text {
    public:
        explicit Text(char *owned_str) : _str(owned_str) {}
        ~Text();
        Text(Text &&t) {
            _str = t._str;
            t._str = nullptr;
        }
        Text(Text &t) = delete;
        Text &operator=(Text &t) = delete;
        void operator=(Text &&t) = delete;

        const char *c_str() const { return _str; }
        bool empty() const { return _str == nullptr || *_str == '\0'; }

        char *release() {
            auto result = _str;
            _str = nullptr;
            return result;
        }

    private:
        char *_str;
    };

    // Like printf, but returns the formated text.
#if METADOT_LOGGING_USE_FMTLIB
    METADOT_LOGGING_EXPORT
    Text vtextprintf(const char *format, fmt::format_args args);

    template<typename... Args>
    METADOT_LOGGING_EXPORT Text textprintf(METADOT_LOGGING_FORMAT_STRING_TYPE format,
                                           const Args &...args) {
        return vtextprintf(format, fmt::make_format_args(args...));
    }
#else
    METADOT_LOGGING_EXPORT
    Text textprintf(METADOT_LOGGING_FORMAT_STRING_TYPE format, ...)
            METADOT_LOGGING_PRINTF_LIKE(1, 2);
#endif

    // Overloaded for variadic template matching.
    METADOT_LOGGING_EXPORT
    Text textprintf();

    using Verbosity = int;

#undef FATAL
#undef ERROR
#undef WARNING
#undef INFO
#undef MAX

    enum NamedVerbosity : Verbosity {
        // Used to mark an invalid verbosity. Do not log to this level.
        Verbosity_INVALID = -10,// Never do LOG_F(INVALID)

        // You may use Verbosity_OFF on g_stderr_verbosity, but for nothing else!
        Verbosity_OFF = -9,// Never do LOG_F(OFF)

        // Prefer to use ABORT_F or ABORT_S over LOG_F(FATAL) or LOG_S(FATAL).
        Verbosity_FATAL = -3,
        Verbosity_ERROR = -2,
        Verbosity_WARNING = -1,

        // Normal messages. By default written to stderr.
        Verbosity_INFO = 0,

        // Same as Verbosity_INFO in every way.
        Verbosity_0 = 0,

        // Verbosity levels 1-9 are generally not written to stderr, but are written to file.
        Verbosity_1 = +1,
        Verbosity_2 = +2,
        Verbosity_3 = +3,
        Verbosity_4 = +4,
        Verbosity_5 = +5,
        Verbosity_6 = +6,
        Verbosity_7 = +7,
        Verbosity_8 = +8,
        Verbosity_9 = +9,

        // Do not use higher verbosity levels, as that will make grepping log files harder.
        Verbosity_MAX = +9,
    };

    struct Message
    {
        // You would generally print a Message by just concatenating the buffers without spacing.
        // Optionally, ignore preamble and indentation.
        Verbosity verbosity;    // Already part of preamble
        const char *filename;   // Already part of preamble
        unsigned line;          // Already part of preamble
        const char *preamble;   // Date, time, uptime, thread, file:line, verbosity.
        const char *indentation;// Just a bunch of spacing.
        const char *prefix;     // Assertion failure info goes here (or "").
        const char *message;    // User message goes here.
    };

    /* Everything with a verbosity equal or greater than g_stderr_verbosity will be
	written to stderr. You can set this in code or via the -v argument.
	Set to Logging::Verbosity_OFF to write nothing to stderr.
	Default is 0, i.e. only log ERROR, WARNING and INFO are written to stderr.
	*/
    METADOT_LOGGING_EXPORT extern Verbosity g_stderr_verbosity;
    METADOT_LOGGING_EXPORT extern bool g_colorlogtostderr;     // True by default.
    METADOT_LOGGING_EXPORT extern unsigned g_flush_interval_ms;// 0 (unbuffered) by default.
    METADOT_LOGGING_EXPORT extern bool
            g_preamble_header;// Prepend each log start by a descriptions line with all columns name? True by default.
    METADOT_LOGGING_EXPORT extern bool
            g_preamble;// Prefix each log line with date, time etc? True by default.

    /* Specify the verbosity used by loguru to log its info messages including the header
	logged when logged::init() is called or on exit. Default is 0 (INFO).
	*/
    METADOT_LOGGING_EXPORT extern Verbosity g_internal_verbosity;

    // Turn off individual parts of the preamble
    METADOT_LOGGING_EXPORT extern bool g_preamble_date;  // The date field
    METADOT_LOGGING_EXPORT extern bool g_preamble_time;  // The time of the current day
    METADOT_LOGGING_EXPORT extern bool g_preamble_uptime;// The time since init call
    METADOT_LOGGING_EXPORT extern bool g_preamble_thread;// The logging thread
    METADOT_LOGGING_EXPORT extern bool
            g_preamble_file;// The file from which the log originates from
    METADOT_LOGGING_EXPORT extern bool g_preamble_verbose;// The verbosity field
    METADOT_LOGGING_EXPORT extern bool g_preamble_pipe;// The pipe symbol right before the message

    // May not throw!
    typedef void (*log_handler_t)(void *user_data, const Message &message);
    typedef void (*close_handler_t)(void *user_data);
    typedef void (*flush_handler_t)(void *user_data);

    // May throw if that's how you'd like to handle your errors.
    typedef void (*fatal_handler_t)(const Message &message);

    // Given a verbosity level, return the level's name or nullptr.
    typedef const char *(*verbosity_to_name_t)(Verbosity verbosity);

    // Given a verbosity level name, return the verbosity level or
    // Verbosity_INVALID if name is not recognized.
    typedef Verbosity (*name_to_verbosity_t)(const char *name);

    struct SignalOptions
    {
        /// Make Loguru try to do unsafe but useful things,
        /// like printing a stack trace, when catching signals.
        /// This may lead to bad things like deadlocks in certain situations.
        bool unsafe_signal_handler = true;

        /// Should Loguru catch SIGABRT ?
        bool sigabrt = true;

        /// Should Loguru catch SIGBUS ?
        bool sigbus = true;

        /// Should Loguru catch SIGFPE ?
        bool sigfpe = true;

        /// Should Loguru catch SIGILL ?
        bool sigill = true;

        /// Should Loguru catch SIGINT ?
        bool sigint = true;

        /// Should Loguru catch SIGSEGV ?
        bool sigsegv = true;

        /// Should Loguru catch SIGTERM ?
        bool sigterm = true;

        static SignalOptions none() {
            SignalOptions options;
            options.unsafe_signal_handler = false;
            options.sigabrt = false;
            options.sigbus = false;
            options.sigfpe = false;
            options.sigill = false;
            options.sigint = false;
            options.sigsegv = false;
            options.sigterm = false;
            return options;
        }
    };

    // Runtime options passed to Logging::init
    struct Options
    {
        // This allows you to use something else instead of "-v" via verbosity_flag.
        // Set to nullptr if you don't want Loguru to parse verbosity from the args.
        const char *verbosity_flag = "-v";

        // Logging::init will set the name of the calling thread to this.
        // If you don't want Loguru to set the name of the main thread,
        // set this to nullptr.
        // NOTE: on SOME platforms Logging::init will only overwrite the thread name
        // if a thread name has not already been set.
        // To always set a thread name, use Logging::set_thread_name instead.
        const char *main_thread_name = "Main Thread";

        SignalOptions signal_options;
    };

    /*  Should be called from the main thread.
		You don't *need* to call this, but if you do you get:
			* Signal handlers installed
			* Program arguments logged
			* Working dir logged
			* Optional -v verbosity flag parsed
			* Main thread name set to "main thread"
			* Explanation of the preamble (date, thread name, etc) logged

		Logging::init() will look for arguments meant for loguru and remove them.
		Arguments meant for loguru are:
			-v n   Set Logging::g_stderr_verbosity level. Examples:
				-v 3        Show verbosity level 3 and lower.
				-v 0        Only show INFO, WARNING, ERROR, FATAL (default).
				-v INFO     Only show INFO, WARNING, ERROR, FATAL (default).
				-v WARNING  Only show WARNING, ERROR, FATAL.
				-v ERROR    Only show ERROR, FATAL.
				-v FATAL    Only show FATAL.
				-v OFF      Turn off logging to stderr.

		Tip: You can set g_stderr_verbosity before calling Logging::init.
		That way you can set the default but have the user override it with the -v flag.
		Note that -v does not affect file logging (see Logging::add_file).

		You can you something other than the -v flag by setting the verbosity_flag option.
	*/
    METADOT_LOGGING_EXPORT
    void init(int &argc, char *argv[], const Options &options = {});

    // Will call remove_all_callbacks(). After calling this, logging will still go to stderr.
    // You generally don't need to call this.
    METADOT_LOGGING_EXPORT
    void shutdown();

    // What ~ will be replaced with, e.g. "/home/your_user_name/"
    METADOT_LOGGING_EXPORT
    const char *home_dir();

    /* Returns the name of the app as given in argv[0] but without leading path.
	   That is, if argv[0] is "../foo/app" this will return "app".
	*/
    METADOT_LOGGING_EXPORT
    const char *argv0_filename();

    // Returns all arguments given to Logging::init(), but escaped with a single space as separator.
    METADOT_LOGGING_EXPORT
    const char *arguments();

    // Returns the path to the current working dir when Logging::init() was called.
    METADOT_LOGGING_EXPORT
    const char *current_dir();

    // Returns the part of the path after the last / or \ (if any).
    METADOT_LOGGING_EXPORT
    const char *filename(const char *path);

    // e.g. "foo/bar/baz.ext" will create the directories "foo/" and "foo/bar/"
    METADOT_LOGGING_EXPORT
    bool create_directories(const char *file_path_const);

    // Writes date and time with millisecond precision, e.g. "20151017_161503.123"
    METADOT_LOGGING_EXPORT
    void write_date_time(char *buff, unsigned long long buff_size);

    // Helper: thread-safe version strerror
    METADOT_LOGGING_EXPORT
    Text errno_as_text();

    /* Given a prefix of e.g. "~/loguru/" this might return
	   "/home/your_username/loguru/app_name/20151017_161503.123.log"

	   where "app_name" is a sanitized version of argv[0].
	*/
    METADOT_LOGGING_EXPORT
    void suggest_log_path(const char *prefix, char *buff, unsigned long long buff_size);

    enum FileMode {
        Truncate,
        Append
    };

    /*  Will log to a file at the given path.
		Any logging message with a verbosity lower or equal to
		the given verbosity will be included.
		The function will create all directories in 'path' if needed.
		If path starts with a ~, it will be replaced with Logging::home_dir()
		To stop the file logging, just call Logging::remove_callback(path) with the same path.
	*/
    METADOT_LOGGING_EXPORT
    bool add_file(const char *path, FileMode mode, Verbosity verbosity);

    METADOT_LOGGING_EXPORT
    // Send logs to syslog with LOG_USER facility (see next call)
    bool add_syslog(const char *app_name, Verbosity verbosity);
    METADOT_LOGGING_EXPORT
    // Send logs to syslog with your own choice of facility (LOG_USER, LOG_AUTH, ...)
    // see loguru.cpp: syslog_log() for more details.
    bool add_syslog(const char *app_name, Verbosity verbosity, int facility);

    /*  Will be called right before abort().
		You can for instance use this to print custom error messages, or throw an exception.
		Feel free to call LOG:ing function from this, but not FATAL ones! */
    METADOT_LOGGING_EXPORT
    void set_fatal_handler(fatal_handler_t handler);

    // Get the current fatal handler, if any. Default value is nullptr.
    METADOT_LOGGING_EXPORT
    fatal_handler_t get_fatal_handler();

    /*  Will be called on each log messages with a verbosity less or equal to the given one.
		Useful for displaying messages on-screen in a game, for example.
		The given on_close is also expected to flush (if desired).
	*/
    METADOT_LOGGING_EXPORT
    void add_callback(const char *id, log_handler_t callback, void *user_data, Verbosity verbosity,
                      close_handler_t on_close = nullptr, flush_handler_t on_flush = nullptr);

    /*  Set a callback that returns custom verbosity level names. If callback
		is nullptr or returns nullptr, default log names will be used.
	*/
    METADOT_LOGGING_EXPORT
    void set_verbosity_to_name_callback(verbosity_to_name_t callback);

    /*  Set a callback that returns the verbosity level matching a name. The
		callback should return Verbosity_INVALID if the name is not
		recognized.
	*/
    METADOT_LOGGING_EXPORT
    void set_name_to_verbosity_callback(name_to_verbosity_t callback);

    /*  Get a custom name for a specific verbosity, if one exists, or nullptr. */
    METADOT_LOGGING_EXPORT
    const char *get_verbosity_name(Verbosity verbosity);

    /*  Get the verbosity enum value from a custom 4-character level name, if one exists.
		If the name does not match a custom level name, Verbosity_INVALID is returned.
	*/
    METADOT_LOGGING_EXPORT
    Verbosity get_verbosity_from_name(const char *name);

    // Returns true iff the callback was found (and removed).
    METADOT_LOGGING_EXPORT
    bool remove_callback(const char *id);

    // Shut down all file logging and any other callback hooks installed.
    METADOT_LOGGING_EXPORT
    void remove_all_callbacks();

    // Returns the maximum of g_stderr_verbosity and all file/custom outputs.
    METADOT_LOGGING_EXPORT
    Verbosity current_verbosity_cutoff();

#if METADOT_LOGGING_USE_FMTLIB
    // Internal functions
    METADOT_LOGGING_EXPORT
    void vlog(Verbosity verbosity, const char *file, unsigned line,
              METADOT_LOGGING_FORMAT_STRING_TYPE format, fmt::format_args args);
    METADOT_LOGGING_EXPORT
    void raw_vlog(Verbosity verbosity, const char *file, unsigned line,
                  METADOT_LOGGING_FORMAT_STRING_TYPE format, fmt::format_args args);

    // Actual logging function. Use the LOG macro instead of calling this directly.
    template<typename... Args>
    METADOT_LOGGING_EXPORT void log(Verbosity verbosity, const char *file, unsigned line,
                                    METADOT_LOGGING_FORMAT_STRING_TYPE format,
                                    const Args &...args) {
        vlog(verbosity, file, line, format, fmt::make_format_args(args...));
    }

    // Log without any preamble or indentation.
    template<typename... Args>
    METADOT_LOGGING_EXPORT void raw_log(Verbosity verbosity, const char *file, unsigned line,
                                        METADOT_LOGGING_FORMAT_STRING_TYPE format,
                                        const Args &...args) {
        raw_vlog(verbosity, file, line, format, fmt::make_format_args(args...));
    }
#else // METADOT_LOGGING_USE_FMTLIB?
    // Actual logging function. Use the LOG macro instead of calling this directly.
    METADOT_LOGGING_EXPORT
    void log(Verbosity verbosity, const char *file, unsigned line,
             METADOT_LOGGING_FORMAT_STRING_TYPE format, ...) METADOT_LOGGING_PRINTF_LIKE(4, 5);

    // Actual logging function.
    METADOT_LOGGING_EXPORT
    void vlog(Verbosity verbosity, const char *file, unsigned line,
              METADOT_LOGGING_FORMAT_STRING_TYPE format, va_list) METADOT_LOGGING_PRINTF_LIKE(4, 0);

    // Log without any preamble or indentation.
    METADOT_LOGGING_EXPORT
    void raw_log(Verbosity verbosity, const char *file, unsigned line,
                 METADOT_LOGGING_FORMAT_STRING_TYPE format, ...) METADOT_LOGGING_PRINTF_LIKE(4, 5);
#endif// !METADOT_LOGGING_USE_FMTLIB

    // Helper class for LOG_SCOPE_F
    class METADOT_LOGGING_EXPORT LogScopeRAII {
    public:
        LogScopeRAII() : _file(nullptr) {}// No logging
        LogScopeRAII(Verbosity verbosity, const char *file, unsigned line,
                     METADOT_LOGGING_FORMAT_STRING_TYPE format, va_list vlist)
                METADOT_LOGGING_PRINTF_LIKE(5, 0);
        LogScopeRAII(Verbosity verbosity, const char *file, unsigned line,
                     METADOT_LOGGING_FORMAT_STRING_TYPE format, ...)
                METADOT_LOGGING_PRINTF_LIKE(5, 6);
        ~LogScopeRAII();

        void Init(METADOT_LOGGING_FORMAT_STRING_TYPE format, va_list vlist)
                METADOT_LOGGING_PRINTF_LIKE(2, 0);

#if defined(_MSC_VER) && _MSC_VER > 1800
        // older MSVC default move ctors close the scope on move. See
        // issue #43
        LogScopeRAII(LogScopeRAII &&other)
            : _verbosity(other._verbosity), _file(other._file), _line(other._line),
              _indent_stderr(other._indent_stderr), _start_time_ns(other._start_time_ns) {
            // Make sure the tmp object's destruction doesn't close the scope:
            other._file = nullptr;

            for (unsigned int i = 0; i < METADOT_LOGGING_SCOPE_TEXT_SIZE; ++i) {
                _name[i] = other._name[i];
            }
        }
#else
        LogScopeRAII(LogScopeRAII &&) = default;
#endif

    private:
        LogScopeRAII(const LogScopeRAII &) = delete;
        LogScopeRAII &operator=(const LogScopeRAII &) = delete;
        void operator=(LogScopeRAII &&) = delete;

        Verbosity _verbosity;
        const char *_file;// Set to null if we are disabled due to verbosity
        unsigned _line;
        bool _indent_stderr;// Did we?
        long long _start_time_ns;
        char _name[METADOT_LOGGING_SCOPE_TEXT_SIZE];
    };

    // Marked as 'noreturn' for the benefit of the static analyzer and optimizer.
    // stack_trace_skip is the number of extrace stack frames to skip above log_and_abort.
#if METADOT_LOGGING_USE_FMTLIB
    METADOT_LOGGING_EXPORT
    METADOT_LOGGING_NORETURN void vlog_and_abort(int stack_trace_skip, const char *expr,
                                                 const char *file, unsigned line,
                                                 METADOT_LOGGING_FORMAT_STRING_TYPE format,
                                                 fmt::format_args);
    template<typename... Args>
    METADOT_LOGGING_EXPORT METADOT_LOGGING_NORETURN void log_and_abort(
            int stack_trace_skip, const char *expr, const char *file, unsigned line,
            METADOT_LOGGING_FORMAT_STRING_TYPE format, const Args &...args) {
        vlog_and_abort(stack_trace_skip, expr, file, line, format, fmt::make_format_args(args...));
    }
#else
    METADOT_LOGGING_EXPORT
    METADOT_LOGGING_NORETURN void log_and_abort(int stack_trace_skip, const char *expr,
                                                const char *file, unsigned line,
                                                METADOT_LOGGING_FORMAT_STRING_TYPE format, ...)
            METADOT_LOGGING_PRINTF_LIKE(5, 6);
#endif
    METADOT_LOGGING_EXPORT
    METADOT_LOGGING_NORETURN void log_and_abort(int stack_trace_skip, const char *expr,
                                                const char *file, unsigned line);

    // Flush output to stderr and files.
    // If g_flush_interval_ms is set to non-zero, this will be called automatically this often.
    // If not set, you do not need to call this at all.
    METADOT_LOGGING_EXPORT
    void flush();

    template<class T>
    inline Text format_value(const T &) {
        return textprintf("N/A");
    }
    template<>
    inline Text format_value(const char &v) {
        return textprintf(METADOT_LOGGING_FMT(c), v);
    }
    template<>
    inline Text format_value(const int &v) {
        return textprintf(METADOT_LOGGING_FMT(d), v);
    }
    template<>
    inline Text format_value(const float &v) {
        return textprintf(METADOT_LOGGING_FMT(f), v);
    }
    template<>
    inline Text format_value(const double &v) {
        return textprintf(METADOT_LOGGING_FMT(f), v);
    }

#if METADOT_LOGGING_USE_FMTLIB
    template<>
    inline Text format_value(const unsigned int &v) {
        return textprintf(METADOT_LOGGING_FMT(d), v);
    }
    template<>
    inline Text format_value(const long &v) {
        return textprintf(METADOT_LOGGING_FMT(d), v);
    }
    template<>
    inline Text format_value(const unsigned long &v) {
        return textprintf(METADOT_LOGGING_FMT(d), v);
    }
    template<>
    inline Text format_value(const long long &v) {
        return textprintf(METADOT_LOGGING_FMT(d), v);
    }
    template<>
    inline Text format_value(const unsigned long long &v) {
        return textprintf(METADOT_LOGGING_FMT(d), v);
    }
#else
    template<>
    inline Text format_value(const unsigned int &v) {
        return textprintf(METADOT_LOGGING_FMT(u), v);
    }
    template<>
    inline Text format_value(const long &v) {
        return textprintf(METADOT_LOGGING_FMT(lu), v);
    }
    template<>
    inline Text format_value(const unsigned long &v) {
        return textprintf(METADOT_LOGGING_FMT(ld), v);
    }
    template<>
    inline Text format_value(const long long &v) {
        return textprintf(METADOT_LOGGING_FMT(llu), v);
    }
    template<>
    inline Text format_value(const unsigned long long &v) {
        return textprintf(METADOT_LOGGING_FMT(lld), v);
    }
#endif

    /* Thread names can be set for the benefit of readable logs.
	   If you do not set the thread name, a hex id will be shown instead.
	   These thread names may or may not be the same as the system thread names,
	   depending on the system.
	   Try to limit the thread name to 15 characters or less. */
    METADOT_LOGGING_EXPORT
    void set_thread_name(const char *name);

    /* Returns the thread name for this thread.
	   On most *nix systems this will return the system thread name (settable from both within and without Loguru).
	   On other systems it will return whatever you set in `set_thread_name()`;
	   If no thread name is set, this will return a hexadecimal thread id.
	   `length` should be the number of bytes available in the buffer.
	   17 is a good number for length.
	   `right_align_hex_id` means any hexadecimal thread id will be written to the end of buffer.
	*/
    METADOT_LOGGING_EXPORT
    void get_thread_name(char *buffer, unsigned long long length, bool right_align_hex_id);

    /* Generates a readable stacktrace as a string.
	   'skip' specifies how many stack frames to skip.
	   For instance, the default skip (1) means:
	   don't include the call to Logging::stacktrace in the stack trace. */
    METADOT_LOGGING_EXPORT
    Text stacktrace(int skip = 1);

    /*  Add a string to be replaced with something else in the stack output.

		For instance, instead of having a stack trace look like this:
			0x41f541 some_function(std::basic_ofstream<char, std::char_traits<char> >&)
		You can clean it up with:
			auto verbose_type_name = Logging::demangle(typeid(std::ofstream).name());
			Logging::add_stack_cleanup(verbose_type_name.c_str(); "std::ofstream");
		So the next time you will instead see:
			0x41f541 some_function(std::ofstream&)

		`replace_with_this` must be shorter than `find_this`.
	*/
    METADOT_LOGGING_EXPORT
    void add_stack_cleanup(const char *find_this, const char *replace_with_this);

    // Example: demangle(typeid(std::ofstream).name()) -> "std::basic_ofstream<char, std::char_traits<char> >"
    METADOT_LOGGING_EXPORT
    Text demangle(const char *name);

    // ------------------------------------------------------------------------
    /*
	Not all terminals support colors, but if they do, and g_colorlogtostderr
	is set, Loguru will write them to stderr to make errors in red, etc.

	You also have the option to manually use them, via the function below.

	Note, however, that if you do, the color codes could end up in your logfile!

	This means if you intend to use them functions you should either:
		* Use them on the stderr/stdout directly (bypass Loguru).
		* Don't add file outputs to Loguru.
		* Expect some \e[1m things in your logfile.

	Usage:
		printf("%sRed%sGreen%sBold green%sClear again\n",
			   Logging::terminal_red(), Logging::terminal_green(),
			   Logging::terminal_bold(), Logging::terminal_reset());

	If the terminal at hand does not support colors the above output
	will just not have funky \e[1m things showing.
	*/

    // Do the output terminal support colors?
    METADOT_LOGGING_EXPORT
    bool terminal_has_color();

    // Colors
    METADOT_LOGGING_EXPORT const char *terminal_black();
    METADOT_LOGGING_EXPORT const char *terminal_red();
    METADOT_LOGGING_EXPORT const char *terminal_green();
    METADOT_LOGGING_EXPORT const char *terminal_yellow();
    METADOT_LOGGING_EXPORT const char *terminal_blue();
    METADOT_LOGGING_EXPORT const char *terminal_purple();
    METADOT_LOGGING_EXPORT const char *terminal_cyan();
    METADOT_LOGGING_EXPORT const char *terminal_light_gray();
    METADOT_LOGGING_EXPORT const char *terminal_light_red();
    METADOT_LOGGING_EXPORT const char *terminal_white();

    // Formating
    METADOT_LOGGING_EXPORT const char *terminal_bold();
    METADOT_LOGGING_EXPORT const char *terminal_underline();

    // You should end each line with this!
    METADOT_LOGGING_EXPORT const char *terminal_reset();

    // --------------------------------------------------------------------
    // Error context related:

    struct StringStream;

    // Use this in your EcEntryBase::print_value overload.
    METADOT_LOGGING_EXPORT
    void stream_print(StringStream &out_string_stream, const char *text);

    class METADOT_LOGGING_EXPORT EcEntryBase {
    public:
        EcEntryBase(const char *file, unsigned line, const char *descr);
        ~EcEntryBase();
        EcEntryBase(const EcEntryBase &) = delete;
        EcEntryBase(EcEntryBase &&) = delete;
        EcEntryBase &operator=(const EcEntryBase &) = delete;
        EcEntryBase &operator=(EcEntryBase &&) = delete;

        virtual void print_value(StringStream &out_string_stream) const = 0;

        EcEntryBase *previous() const { return _previous; }

        // private:
        const char *_file;
        unsigned _line;
        const char *_descr;
        EcEntryBase *_previous;
    };

    template<typename T>
    class EcEntryData : public EcEntryBase {
    public:
        using Printer = Text (*)(T data);

        EcEntryData(const char *file, unsigned line, const char *descr, T data, Printer &&printer)
            : EcEntryBase(file, line, descr), _data(data), _printer(printer) {}

        virtual void print_value(StringStream &out_string_stream) const override {
            const auto str = _printer(_data);
            stream_print(out_string_stream, str.c_str());
        }

    private:
        T _data;
        Printer _printer;
    };

    // template<typename Printer>
    // class EcEntryLambda : public EcEntryBase
    // {
    // public:
    // 	EcEntryLambda(const char* file, unsigned line, const char* descr, Printer&& printer)
    // 		: EcEntryBase(file, line, descr), _printer(std::move(printer)) {}

    // 	virtual void print_value(StringStream& out_string_stream) const override
    // 	{
    // 		const auto str = _printer();
    // 		stream_print(out_string_stream, str.c_str());
    // 	}

    // private:
    // 	Printer _printer;
    // };

    // template<typename Printer>
    // EcEntryLambda<Printer> make_ec_entry_lambda(const char* file, unsigned line, const char* descr, Printer&& printer)
    // {
    // 	return {file, line, descr, std::move(printer)};
    // }

    template<class T>
    struct decay_char_array
    {
        using type = T;
    };

    template<unsigned long long N>
    struct decay_char_array<const char (&)[N]>
    {
        using type = const char *;
    };

    template<class T>
    struct make_const_ptr
    {
        using type = T;
    };

    template<class T>
    struct make_const_ptr<T *>
    {
        using type = const T *;
    };

    template<class T>
    struct make_ec_type
    {
        using type = typename make_const_ptr<typename decay_char_array<T>::type>::type;
    };

/* 	A stack trace gives you the names of the function at the point of a crash.
		With ERROR_CONTEXT, you can also get the values of select local variables.
		Usage:

		void process_customers(const std::string& filename)
		{
			ERROR_CONTEXT("Processing file", filename.c_str());
			for (int customer_index : ...)
			{
				ERROR_CONTEXT("Customer index", customer_index);
				...
			}
		}

		The context is in effect during the scope of the ERROR_CONTEXT.
		Use Logging::get_error_context() to get the contents of the active error contexts.

		Example result:

		------------------------------------------------
		[ErrorContext]                main.cpp:416   Processing file:    "customers.json"
		[ErrorContext]                main.cpp:417   Customer index:     42
		------------------------------------------------

		Error contexts are printed automatically on crashes, and only on crashes.
		This makes them much faster than logging the value of a variable.
	*/
#define ERROR_CONTEXT(descr, data)                                                                 \
    const Logging::EcEntryData<Logging::make_ec_type<decltype(data)>::type>                        \
    METADOT_LOGGING_ANONYMOUS_VARIABLE(error_context_scope_)(                                      \
            __FILE__, __LINE__, descr, data,                                                       \
            static_cast<                                                                           \
                    Logging::EcEntryData<Logging::make_ec_type<decltype(data)>::type>::Printer>(   \
                    Logging::ec_to_text))// For better error messages

    /*
	#define ERROR_CONTEXT(descr, data)                                 \
		const auto METADOT_LOGGING_ANONYMOUS_VARIABLE(error_context_scope_)(    \
			Logging::make_ec_entry_lambda(__FILE__, __LINE__, descr,    \
				[=](){ return Logging::ec_to_text(data); }))
*/

    using EcHandle = const EcEntryBase *;

    /*
		Get a light-weight handle to the error context stack on this thread.
		The handle is valid as long as the current thread has no changes to its error context stack.
		You can pass the handle to Logging::get_error_context on another thread.
		This can be very useful for when you have a parent thread spawning several working threads,
		and you want the error context of the parent thread to get printed (too) when there is an
		error on the child thread. You can accomplish this thusly:

		void foo(const char* parameter)
		{
			ERROR_CONTEXT("parameter", parameter)
			const auto parent_ec_handle = Logging::get_thread_ec_handle();

			std::thread([=]{
				Logging::set_thread_name("child thread");
				ERROR_CONTEXT("parent context", parent_ec_handle);
				dangerous_code();
			}.join();
		}

	*/
    METADOT_LOGGING_EXPORT
    EcHandle get_thread_ec_handle();

    // Get a string describing the current stack of error context. Empty string if there is none.
    METADOT_LOGGING_EXPORT
    Text get_error_context();

    // Get a string describing the error context of the given thread handle.
    METADOT_LOGGING_EXPORT
    Text get_error_context_for(EcHandle ec_handle);

    // ------------------------------------------------------------------------

    METADOT_LOGGING_EXPORT Text ec_to_text(const char *data);
    METADOT_LOGGING_EXPORT Text ec_to_text(char data);
    METADOT_LOGGING_EXPORT Text ec_to_text(int data);
    METADOT_LOGGING_EXPORT Text ec_to_text(unsigned int data);
    METADOT_LOGGING_EXPORT Text ec_to_text(long data);
    METADOT_LOGGING_EXPORT Text ec_to_text(unsigned long data);
    METADOT_LOGGING_EXPORT Text ec_to_text(long long data);
    METADOT_LOGGING_EXPORT Text ec_to_text(unsigned long long data);
    METADOT_LOGGING_EXPORT Text ec_to_text(float data);
    METADOT_LOGGING_EXPORT Text ec_to_text(double data);
    METADOT_LOGGING_EXPORT Text ec_to_text(long double data);
    METADOT_LOGGING_EXPORT Text ec_to_text(EcHandle);

    /*
	You can add ERROR_CONTEXT support for your own types by overloading ec_to_text. Here's how:

	some.hpp:
		namespace loguru {
			Text ec_to_text(MySmallType data)
			Text ec_to_text(const MyBigType* data)
		} // namespace loguru

	some.cpp:
		namespace loguru {
			Text ec_to_text(MySmallType small_value)
			{
				// Called only when needed, i.e. on a crash.
				std::string str = small_value.as_string(); // Format 'small_value' here somehow.
				return Text{STRDUP(str.c_str())};
			}

			Text ec_to_text(const MyBigType* big_value)
			{
				// Called only when needed, i.e. on a crash.
				std::string str = big_value->as_string(); // Format 'big_value' here somehow.
				return Text{STRDUP(str.c_str())};
			}
		} // namespace loguru

	Any file that include some.hpp:
		void foo(MySmallType small, const MyBigType& big)
		{
			ERROR_CONTEXT("Small", small); // Copy ´small` by value.
			ERROR_CONTEXT("Big",   &big);  // `big` should not change during this scope!
			....
		}
	*/
}// namespace Logging

METADOT_LOGGING_ANONYMOUS_NAMESPACE_END

// --------------------------------------------------------------------
// Logging macros

// LOG_F(2, "Only logged if verbosity is 2 or higher: %d", some_number);
#define VLOG_F(verbosity, ...)                                                                     \
    ((verbosity) > Logging::current_verbosity_cutoff())                                            \
            ? (void) 0                                                                             \
            : Logging::log(verbosity, __FILE__, __LINE__, __VA_ARGS__)

// LOG_F(INFO, "Foo: %d", some_number);
#define LOG_F(verbosity_name, ...) VLOG_F(Logging::Verbosity_##verbosity_name, __VA_ARGS__)

#define VLOG_IF_F(verbosity, cond, ...)                                                            \
    ((verbosity) > Logging::current_verbosity_cutoff() || (cond) == false)                         \
            ? (void) 0                                                                             \
            : Logging::log(verbosity, __FILE__, __LINE__, __VA_ARGS__)

#define LOG_IF_F(verbosity_name, cond, ...)                                                        \
    VLOG_IF_F(Logging::Verbosity_##verbosity_name, cond, __VA_ARGS__)

#define VLOG_SCOPE_F(verbosity, ...)                                                               \
    Logging::LogScopeRAII METADOT_LOGGING_ANONYMOUS_VARIABLE(error_context_RAII_) =                \
            ((verbosity) > Logging::current_verbosity_cutoff())                                    \
                    ? Logging::LogScopeRAII()                                                      \
                    : Logging::LogScopeRAII(verbosity, __FILE__, __LINE__, __VA_ARGS__)

// Raw logging - no preamble, no indentation. Slightly faster than full logging.
#define RAW_VLOG_F(verbosity, ...)                                                                 \
    ((verbosity) > Logging::current_verbosity_cutoff())                                            \
            ? (void) 0                                                                             \
            : Logging::raw_log(verbosity, __FILE__, __LINE__, __VA_ARGS__)

#define RAW_LOG_F(verbosity_name, ...) RAW_VLOG_F(Logging::Verbosity_##verbosity_name, __VA_ARGS__)

// Use to book-end a scope. Affects logging on all threads.
#define LOG_SCOPE_F(verbosity_name, ...)                                                           \
    VLOG_SCOPE_F(Logging::Verbosity_##verbosity_name, __VA_ARGS__)

#define LOG_SCOPE_FUNCTION(verbosity_name) LOG_SCOPE_F(verbosity_name, __func__)

// -----------------------------------------------
// ABORT_F macro. Usage:  ABORT_F("Cause of error: %s", error_str);

// Message is optional
#define ABORT_F(...) Logging::log_and_abort(0, "ABORT: ", __FILE__, __LINE__, __VA_ARGS__)

// --------------------------------------------------------------------
// CHECK_F macros:

#define CHECK_WITH_INFO_F(test, info, ...)                                                         \
    METADOT_LOGGING_PREDICT_TRUE((test) == true)                                                   \
            ? (void) 0                                                                             \
            : Logging::log_and_abort(0, "CHECK FAILED:  " info "  ", __FILE__, __LINE__,           \
                                     ##__VA_ARGS__)

/* Checked at runtime too. Will print error, then call fatal_handler (if any), then 'abort'.
   Note that the test must be boolean.
   CHECK_F(ptr); will not compile, but CHECK_F(ptr != nullptr); will. */
#define CHECK_F(test, ...) CHECK_WITH_INFO_F(test, #test, ##__VA_ARGS__)

#define CHECK_NOTNULL_F(x, ...) CHECK_WITH_INFO_F((x) != nullptr, #x " != nullptr", ##__VA_ARGS__)

#define CHECK_OP_F(expr_left, expr_right, op, ...)                                                                       \
    do {                                                                                                                 \
        auto val_left = expr_left;                                                                                       \
        auto val_right = expr_right;                                                                                     \
        if (!METADOT_LOGGING_PREDICT_TRUE(val_left op val_right)) {                                                      \
            auto str_left = Logging::format_value(val_left);                                                             \
            auto str_right = Logging::format_value(val_right);                                                           \
            auto fail_info = Logging::textprintf(                                                                        \
                    "CHECK FAILED:  " METADOT_LOGGING_FMT(s) " " METADOT_LOGGING_FMT(s) " " METADOT_LOGGING_FMT(         \
                            s) "  (" METADOT_LOGGING_FMT(s) " " METADOT_LOGGING_FMT(s) " " METADOT_LOGGING_FMT(s) ")  ", \
                    #expr_left, #op, #expr_right, str_left.c_str(), #op, str_right.c_str());                             \
            auto user_msg = Logging::textprintf(__VA_ARGS__);                                                            \
            Logging::log_and_abort(0, fail_info.c_str(), __FILE__, __LINE__,                                             \
                                   METADOT_LOGGING_FMT(s), user_msg.c_str());                                            \
        }                                                                                                                \
    } while (false)

#ifndef METADOT_LOGGING_DEBUG_LOGGING
#ifndef NDEBUG
#define METADOT_LOGGING_DEBUG_LOGGING 1
#else
#define METADOT_LOGGING_DEBUG_LOGGING 0
#endif
#endif

#if METADOT_LOGGING_DEBUG_LOGGING
// Debug logging enabled:
#define DLOG_F(verbosity_name, ...) LOG_F(verbosity_name, __VA_ARGS__)
#define DVLOG_F(verbosity, ...) VLOG_F(verbosity, __VA_ARGS__)
#define DLOG_IF_F(verbosity_name, ...) LOG_IF_F(verbosity_name, __VA_ARGS__)
#define DVLOG_IF_F(verbosity, ...) VLOG_IF_F(verbosity, __VA_ARGS__)
#define DRAW_LOG_F(verbosity_name, ...) RAW_LOG_F(verbosity_name, __VA_ARGS__)
#define DRAW_VLOG_F(verbosity, ...) RAW_VLOG_F(verbosity, __VA_ARGS__)
#else
// Debug logging disabled:
#define DLOG_F(verbosity_name, ...)
#define DVLOG_F(verbosity, ...)
#define DLOG_IF_F(verbosity_name, ...)
#define DVLOG_IF_F(verbosity, ...)
#define DRAW_LOG_F(verbosity_name, ...)
#define DRAW_VLOG_F(verbosity, ...)
#endif

#define CHECK_EQ_F(a, b, ...) CHECK_OP_F(a, b, ==, ##__VA_ARGS__)
#define CHECK_NE_F(a, b, ...) CHECK_OP_F(a, b, !=, ##__VA_ARGS__)
#define CHECK_LT_F(a, b, ...) CHECK_OP_F(a, b, <, ##__VA_ARGS__)
#define CHECK_GT_F(a, b, ...) CHECK_OP_F(a, b, >, ##__VA_ARGS__)
#define CHECK_LE_F(a, b, ...) CHECK_OP_F(a, b, <=, ##__VA_ARGS__)
#define CHECK_GE_F(a, b, ...) CHECK_OP_F(a, b, >=, ##__VA_ARGS__)

#ifndef METADOT_LOGGING_DEBUG_CHECKS
#ifndef NDEBUG
#define METADOT_LOGGING_DEBUG_CHECKS 1
#else
#define METADOT_LOGGING_DEBUG_CHECKS 0
#endif
#endif

#if METADOT_LOGGING_DEBUG_CHECKS
// Debug checks enabled:
#define DCHECK_F(test, ...) CHECK_F(test, ##__VA_ARGS__)
#define DCHECK_NOTNULL_F(x, ...) CHECK_NOTNULL_F(x, ##__VA_ARGS__)
#define DCHECK_EQ_F(a, b, ...) CHECK_EQ_F(a, b, ##__VA_ARGS__)
#define DCHECK_NE_F(a, b, ...) CHECK_NE_F(a, b, ##__VA_ARGS__)
#define DCHECK_LT_F(a, b, ...) CHECK_LT_F(a, b, ##__VA_ARGS__)
#define DCHECK_LE_F(a, b, ...) CHECK_LE_F(a, b, ##__VA_ARGS__)
#define DCHECK_GT_F(a, b, ...) CHECK_GT_F(a, b, ##__VA_ARGS__)
#define DCHECK_GE_F(a, b, ...) CHECK_GE_F(a, b, ##__VA_ARGS__)
#else
// Debug checks disabled:
#define DCHECK_F(test, ...)
#define DCHECK_NOTNULL_F(x, ...)
#define DCHECK_EQ_F(a, b, ...)
#define DCHECK_NE_F(a, b, ...)
#define DCHECK_LT_F(a, b, ...)
#define DCHECK_LE_F(a, b, ...)
#define DCHECK_GT_F(a, b, ...)
#define DCHECK_GE_F(a, b, ...)
#endif// NDEBUG

#if METADOT_LOGGING_REDEFINE_ASSERT
#undef assert
#ifndef NDEBUG
// Debug:
#define assert(test) CHECK_WITH_INFO_F(!!(test), #test)// HACK
#else
#define assert(test)
#endif
#endif// METADOT_LOGGING_REDEFINE_ASSERT

#endif// METADOT_LOGGING_HAS_DECLARED_FORMAT_HEADER

// ----------------------------------------------------------------------------
// .dP"Y8 888888 88""Yb 888888    db    8b    d8 .dP"Y8
// `Ybo."   88   88__dP 88__     dPYb   88b  d88 `Ybo."
// o.`Y8b   88   88"Yb  88""    dP__Yb  88YbdP88 o.`Y8b
// 8bodP'   88   88  Yb 888888 dP""""Yb 88 YY 88 8bodP'

#if METADOT_LOGGING_WITH_STREAMS
#ifndef METADOT_LOGGING_HAS_DECLARED_STREAMS_HEADER
#define METADOT_LOGGING_HAS_DECLARED_STREAMS_HEADER

/* This file extends loguru to enable std::stream-style logging, a la Glog.
   It's an optional feature behind the METADOT_LOGGING_WITH_STREAMS settings
   because including it everywhere will slow down compilation times.
*/

#include <cstdarg>
#include <sstream>// Adds about 38 kLoC on clang.
#include <string>

METADOT_LOGGING_ANONYMOUS_NAMESPACE_BEGIN

namespace Logging {
    // Like sprintf, but returns the formated text.
    METADOT_LOGGING_EXPORT
    std::string strprintf(METADOT_LOGGING_FORMAT_STRING_TYPE format, ...)
            METADOT_LOGGING_PRINTF_LIKE(1, 2);

    // Like vsprintf, but returns the formated text.
    METADOT_LOGGING_EXPORT
    std::string vstrprintf(METADOT_LOGGING_FORMAT_STRING_TYPE format, va_list)
            METADOT_LOGGING_PRINTF_LIKE(1, 0);

    class METADOT_LOGGING_EXPORT StreamLogger {
    public:
        StreamLogger(Verbosity verbosity, const char *file, unsigned line)
            : _verbosity(verbosity), _file(file), _line(line) {}
        ~StreamLogger() noexcept(false);

        template<typename T>
        StreamLogger &operator<<(const T &t) {
            _ss << t;
            return *this;
        }

        // std::endl and other iomanip:s.
        StreamLogger &operator<<(std::ostream &(*f)(std::ostream &) ) {
            f(_ss);
            return *this;
        }

    private:
        Verbosity _verbosity;
        const char *_file;
        unsigned _line;
        std::ostringstream _ss;
    };

    class METADOT_LOGGING_EXPORT AbortLogger {
    public:
        AbortLogger(const char *expr, const char *file, unsigned line)
            : _expr(expr), _file(file), _line(line) {}
        METADOT_LOGGING_NORETURN ~AbortLogger() noexcept(false);

        template<typename T>
        AbortLogger &operator<<(const T &t) {
            _ss << t;
            return *this;
        }

        // std::endl and other iomanip:s.
        AbortLogger &operator<<(std::ostream &(*f)(std::ostream &) ) {
            f(_ss);
            return *this;
        }

    private:
        const char *_expr;
        const char *_file;
        unsigned _line;
        std::ostringstream _ss;
    };

    class METADOT_LOGGING_EXPORT Voidify {
    public:
        Voidify() {}
        // This has to be an operator with a precedence lower than << but higher than ?:
        void operator&(const StreamLogger &) {}
        void operator&(const AbortLogger &) {}
    };

/*  Helper functions for CHECK_OP_S macro.
		GLOG trick: The (int, int) specialization works around the issue that the compiler
		will not instantiate the template version of the function on values of unnamed enum type. */
#define DEFINE_CHECK_OP_IMPL(name, op)                                                             \
    template<typename T1, typename T2>                                                             \
    inline std::string *name(const char *expr, const T1 &v1, const char *op_str, const T2 &v2) {   \
        if (METADOT_LOGGING_PREDICT_TRUE(v1 op v2)) { return NULL; }                               \
        std::ostringstream ss;                                                                     \
        ss << "CHECK FAILED:  " << expr << "  (" << v1 << " " << op_str << " " << v2 << ")  ";     \
        return new std::string(ss.str());                                                          \
    }                                                                                              \
    inline std::string *name(const char *expr, int v1, const char *op_str, int v2) {               \
        return name<int, int>(expr, v1, op_str, v2);                                               \
    }

    DEFINE_CHECK_OP_IMPL(check_EQ_impl, ==)
    DEFINE_CHECK_OP_IMPL(check_NE_impl, !=)
    DEFINE_CHECK_OP_IMPL(check_LE_impl, <=)
    DEFINE_CHECK_OP_IMPL(check_LT_impl, <)
    DEFINE_CHECK_OP_IMPL(check_GE_impl, >=)
    DEFINE_CHECK_OP_IMPL(check_GT_impl, >)
#undef DEFINE_CHECK_OP_IMPL

    /*  GLOG trick: Function is overloaded for integral types to allow static const integrals
		declared in classes and not defined to be used as arguments to CHECK* macros. */
    template<class T>
    inline const T &referenceable_value(const T &t) {
        return t;
    }
    inline char referenceable_value(char t) { return t; }
    inline unsigned char referenceable_value(unsigned char t) { return t; }
    inline signed char referenceable_value(signed char t) { return t; }
    inline short referenceable_value(short t) { return t; }
    inline unsigned short referenceable_value(unsigned short t) { return t; }
    inline int referenceable_value(int t) { return t; }
    inline unsigned int referenceable_value(unsigned int t) { return t; }
    inline long referenceable_value(long t) { return t; }
    inline unsigned long referenceable_value(unsigned long t) { return t; }
    inline long long referenceable_value(long long t) { return t; }
    inline unsigned long long referenceable_value(unsigned long long t) { return t; }
}// namespace Logging

METADOT_LOGGING_ANONYMOUS_NAMESPACE_END

// -----------------------------------------------
// Logging macros:

// usage:  LOG_STREAM(INFO) << "Foo " << std::setprecision(10) << some_value;
#define VLOG_IF_S(verbosity, cond)                                                                 \
    ((verbosity) > Logging::current_verbosity_cutoff() || (cond) == false)                         \
            ? (void) 0                                                                             \
            : Logging::Voidify() & Logging::StreamLogger(verbosity, __FILE__, __LINE__)
#define LOG_IF_S(verbosity_name, cond) VLOG_IF_S(Logging::Verbosity_##verbosity_name, cond)
#define VLOG_S(verbosity) VLOG_IF_S(verbosity, true)
#define LOG_S(verbosity_name) VLOG_S(Logging::Verbosity_##verbosity_name)

// -----------------------------------------------
// ABORT_S macro. Usage:  ABORT_S() << "Causo of error: " << details;

#define ABORT_S() Logging::Voidify() & Logging::AbortLogger("ABORT: ", __FILE__, __LINE__)

// -----------------------------------------------
// CHECK_S macros:

#define CHECK_WITH_INFO_S(cond, info)                                                              \
    METADOT_LOGGING_PREDICT_TRUE((cond) == true)                                                   \
    ? (void) 0                                                                                     \
    : Logging::Voidify() & Logging::AbortLogger("CHECK FAILED:  " info "  ", __FILE__, __LINE__)

#define CHECK_S(cond) CHECK_WITH_INFO_S(cond, #cond)
#define CHECK_NOTNULL_S(x) CHECK_WITH_INFO_S((x) != nullptr, #x " != nullptr")

#define CHECK_OP_S(function_name, expr1, op, expr2)                                                \
    while (auto error_string = Logging::function_name(#expr1 " " #op " " #expr2,                   \
                                                      Logging::referenceable_value(expr1), #op,    \
                                                      Logging::referenceable_value(expr2)))        \
    Logging::AbortLogger(error_string->c_str(), __FILE__, __LINE__)

#define CHECK_EQ_S(expr1, expr2) CHECK_OP_S(check_EQ_impl, expr1, ==, expr2)
#define CHECK_NE_S(expr1, expr2) CHECK_OP_S(check_NE_impl, expr1, !=, expr2)
#define CHECK_LE_S(expr1, expr2) CHECK_OP_S(check_LE_impl, expr1, <=, expr2)
#define CHECK_LT_S(expr1, expr2) CHECK_OP_S(check_LT_impl, expr1, <, expr2)
#define CHECK_GE_S(expr1, expr2) CHECK_OP_S(check_GE_impl, expr1, >=, expr2)
#define CHECK_GT_S(expr1, expr2) CHECK_OP_S(check_GT_impl, expr1, >, expr2)

#if METADOT_LOGGING_DEBUG_LOGGING
// Debug logging enabled:
#define DVLOG_IF_S(verbosity, cond) VLOG_IF_S(verbosity, cond)
#define DLOG_IF_S(verbosity_name, cond) LOG_IF_S(verbosity_name, cond)
#define DVLOG_S(verbosity) VLOG_S(verbosity)
#define DLOG_S(verbosity_name) LOG_S(verbosity_name)
#else
// Debug logging disabled:
#define DVLOG_IF_S(verbosity, cond)                                                                \
    (true || (verbosity) > Logging::current_verbosity_cutoff() || (cond) == false)                 \
            ? (void) 0                                                                             \
            : Logging::Voidify() & Logging::StreamLogger(verbosity, __FILE__, __LINE__)

#define DLOG_IF_S(verbosity_name, cond) DVLOG_IF_S(Logging::Verbosity_##verbosity_name, cond)
#define DVLOG_S(verbosity) DVLOG_IF_S(verbosity, true)
#define DLOG_S(verbosity_name) DVLOG_S(Logging::Verbosity_##verbosity_name)
#endif

#if METADOT_LOGGING_DEBUG_CHECKS
// Debug checks enabled:
#define DCHECK_S(cond) CHECK_S(cond)
#define DCHECK_NOTNULL_S(x) CHECK_NOTNULL_S(x)
#define DCHECK_EQ_S(a, b) CHECK_EQ_S(a, b)
#define DCHECK_NE_S(a, b) CHECK_NE_S(a, b)
#define DCHECK_LT_S(a, b) CHECK_LT_S(a, b)
#define DCHECK_LE_S(a, b) CHECK_LE_S(a, b)
#define DCHECK_GT_S(a, b) CHECK_GT_S(a, b)
#define DCHECK_GE_S(a, b) CHECK_GE_S(a, b)
#else
// Debug checks disabled:
#define DCHECK_S(cond) CHECK_S(true || (cond))
#define DCHECK_NOTNULL_S(x) CHECK_S(true || (x) != nullptr)
#define DCHECK_EQ_S(a, b) CHECK_S(true || (a) == (b))
#define DCHECK_NE_S(a, b) CHECK_S(true || (a) != (b))
#define DCHECK_LT_S(a, b) CHECK_S(true || (a) < (b))
#define DCHECK_LE_S(a, b) CHECK_S(true || (a) <= (b))
#define DCHECK_GT_S(a, b) CHECK_S(true || (a) > (b))
#define DCHECK_GE_S(a, b) CHECK_S(true || (a) >= (b))
#endif

#if METADOT_LOGGING_REPLACE_GLOG
#undef LOG
#undef VLOG
#undef LOG_IF
#undef VLOG_IF
#undef CHECK
#undef CHECK_NOTNULL
#undef CHECK_EQ
#undef CHECK_NE
#undef CHECK_LT
#undef CHECK_LE
#undef CHECK_GT
#undef CHECK_GE
#undef DLOG
#undef DVLOG
#undef DLOG_IF
#undef DVLOG_IF
#undef DCHECK
#undef DCHECK_NOTNULL
#undef DCHECK_EQ
#undef DCHECK_NE
#undef DCHECK_LT
#undef DCHECK_LE
#undef DCHECK_GT
#undef DCHECK_GE
#undef VLOG_IS_ON

#define LOG LOG_S
#define VLOG VLOG_S
#define LOG_IF LOG_IF_S
#define VLOG_IF VLOG_IF_S
#define CHECK(cond) CHECK_S(!!(cond))
#define CHECK_NOTNULL CHECK_NOTNULL_S
#define CHECK_EQ CHECK_EQ_S
#define CHECK_NE CHECK_NE_S
#define CHECK_LT CHECK_LT_S
#define CHECK_LE CHECK_LE_S
#define CHECK_GT CHECK_GT_S
#define CHECK_GE CHECK_GE_S
#define DLOG DLOG_S
#define DVLOG DVLOG_S
#define DLOG_IF DLOG_IF_S
#define DVLOG_IF DVLOG_IF_S
#define DCHECK DCHECK_S
#define DCHECK_NOTNULL DCHECK_NOTNULL_S
#define DCHECK_EQ DCHECK_EQ_S
#define DCHECK_NE DCHECK_NE_S
#define DCHECK_LT DCHECK_LT_S
#define DCHECK_LE DCHECK_LE_S
#define DCHECK_GT DCHECK_GT_S
#define DCHECK_GE DCHECK_GE_S
#define VLOG_IS_ON(verbosity) ((verbosity) <= Logging::current_verbosity_cutoff())

#endif// METADOT_LOGGING_REPLACE_GLOG

#endif// METADOT_LOGGING_WITH_STREAMS

#endif// METADOT_LOGGING_HAS_DECLARED_STREAMS_HEADER