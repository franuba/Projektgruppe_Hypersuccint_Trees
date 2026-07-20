#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_

#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "pht/timer.h"
#include "pht/pht_export.h"

#ifdef PHT_LOGGER_QUIET
#define PHT_LOGGER_DEBUG(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_DEBUG,   tag, __FILE__, __LINE__, __func__,true)
#define PHT_LOGGER_INFO(tag)    pht::Logger::log(pht::Logger::LogLevel::PHT_INFO,    tag, __FILE__, __LINE__, __func__,true)
#define PHT_LOGGER_WARNING(tag) pht::Logger::log(pht::Logger::LogLevel::PHT_WARNING, tag, __FILE__, __LINE__, __func__,true)
#define PHT_LOGGER_ERROR(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_ERROR,   tag, __FILE__, __LINE__, __func__,true)
#define PHT_LOGGER_FATAL(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_FATAL,   tag, __FILE__, __LINE__, __func__,true)
#else
#define PHT_LOGGER_DEBUG(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_DEBUG,   tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_INFO(tag)    pht::Logger::log(pht::Logger::LogLevel::PHT_INFO,    tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_WARNING(tag) pht::Logger::log(pht::Logger::LogLevel::PHT_WARNING, tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_ERROR(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_ERROR,   tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_FATAL(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_FATAL,   tag, __FILE__, __LINE__, __func__)
#endif

namespace pht {
    /**
     * This class allows the printing of neatly formated, information-enriched log messages. 
     * Example usage:
     * PHT_LOGGER_INFO("Test") << "This is a test" << pht::Logger::endl();
     */
    class PHT_EXPORT Logger {
        friend class LogStream;

    public:
        /**
         * This enum defines the different LogLevels. More important log-levels will supress lower-level messages if used as filter. 
         */
        enum class LogLevel {
            PHT_DEBUG = 0, PHT_INFO = 1, PHT_WARNING = 2, PHT_ERROR = 3, PHT_FATAL = 4
        };

    public:

        /**
         * This class allows the printing of messages as stream instead of function-arguments. 
         * DO NOT save objects of this class. 
         * DO NOT use/instantiate/... this class expect for their standard use in Logger. 
         */
        //TODO Change to better process of stream-printing messages. 
        class PHT_EXPORT LogStream {
            friend class Logger;

        public:
            /**
             * Prohibit copying of LogStreams. 
             * @param copy The stream to copy. 
             */
            LogStream(const LogStream& copy) = delete;
            /**
             * Prohibit copying of LogStreams. 
             * @param copy The stream to copy. 
             */
            LogStream& operator=(const LogStream& copy) = delete;
            /**
             * Allow only moving of LogStreams. 
             * @param move The stream to move. 
             */
            LogStream(LogStream&& move) {
                std::exchange(move.exists, false);
            }
            /**
             * Allow only moving of LogStreams. 
             * @param move The stream to move. 
             */
            LogStream& operator=(LogStream&& move) {
                std::exchange(move.exists, false);
                return *this;
            }

            /**
             * Flush stream and log message if the stream gets deleted. 
             * @param move The stream to move. 
             */
            ~LogStream() {
                if(exists) {
                    if(!quiet) {
                        Logger::_log(level, tag, content.str(), file, line, func);
                    }
                    content.clear();
                }
            }

            /**
             * Append a value to the current message. Must be terminated with pht::Logger::endl() to be printed. 
             * This method will fail if this stream is not the most current, i.e. another logging call was made before this on was finished or this stream was already printed. 
             * @tparam T A printable type. 
             * @param value The value to print. 
             * @return LogStream& This stream, for call-chaining. 
             */
            template<class T> LogStream& operator<<(const T& value) {
                content << value;
                return *this;
            }

            /**
             * Template specialization for backward-compatibility, DO NOT USE.
             * @param value The value to capture. 
             * @return LogStream& This stream, for call-chaining. 
             */
            template<> [[deprecated]] LogStream& operator<<(void* const& value) {
                if(value != Logger::endl()) {
                    content << value;
                }
                return *this;
            }

        private:
            #ifdef _MSC_VER
            #pragma warning(disable:4251)
            #endif
            std::stringstream content;
            LogLevel level;
            const std::string tag;
            const std::string file;
            uint32_t line;
            const std::string func;
            bool quiet;
            bool exists;
            #ifdef _MSC_VER
            #pragma warning(default:4251)
            #endif

            LogStream(LogLevel level, std::string  tag, std::string  file, uint32_t line, std::string  func, bool quiet, bool exists) : level(level), tag(std::move(tag)), file(std::move(file)), line(line), func(std::move(func)), quiet(quiet), exists(exists) {}
        };

    public:
        /**
         * Create a new log stream to print a message to.  
         * DO NOT CALL THIS FUNCTION DIRECTLY!!!
         * Use the PHT_LOGGER_... macros intead. 
         * @param level The log level to use for this message/log-stream. 
         * @param tag The tag (category) of this message/log-stream. 
         * @param file The file from which this message/log-stream originates. 
         * @param line The line in the file from which this message/log-stream originates.
         * @param func The function from which this message/log-stream originates. 
         * @param quiet Disable printing. 
         * @return LogStream& A stream to print a message to. 
         */
        static LogStream log(LogLevel level, const std::string& tag, const std::string& file, uint32_t line, const std::string& func, bool quiet = false);

        /**
         * Returns the current log-level used in filtering messages.
         * @return LogLevel The current log-level. 
         */
        static LogLevel getLogLevel();

        /**
         * Sets the log-level to filter messages.
         * The default level is DEBUG. 
         * @param level The new level. 
         */
        static void setLogLevel(LogLevel level);

        /**
         * Allows disabling of the standard output. 
         * @param enabled The new state of the standard output.
         */
        static void setStdOutEnabled(bool enabled);

        /**
         * Used to end logging messages. 
         * Must be appended to every message to print it. 
         * DEPRECATED, DO NOT USE ANYMORE, it is not necessary!
         * @return void* A marker value to end a log-stream and print its value. 
         */
        [[deprecated]] static void* endl();

    private:
        class PHT_EXPORT LoggerStatics {
            friend class Logger;
        private:
            #ifdef _MSC_VER
            #pragma warning(disable:4251)
            #endif
            LogLevel logLevel;
            Timer timer;
            std::regex unifyLinebreakRegex;
            std::unique_ptr<std::ostream> stdOutWrapper;
            std::streambuf* stdoutBuf;
            #ifdef _MSC_VER
            #pragma warning(default:4251)
            #endif

            LoggerStatics();
        };

        static LoggerStatics& getStatics();

        static void _log(LogLevel level, const std::string& tag, const std::string& message, const std::string& file, uint32_t line, const std::string& func);
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_
