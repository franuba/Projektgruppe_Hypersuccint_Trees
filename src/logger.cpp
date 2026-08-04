#include "pht/logger.h"

#include <filesystem>
#include <sstream>

#define PHT_LOGGER_TAG_WIDTH 10
#define PHT_LOGGER_MSG_WIDTH 150
#define PHT_LOGGER_MSG_OFFSET 22+PHT_LOGGER_TAG_WIDTH
#define PHT_LOGGER_COLORIZE

pht::Logger::LogStream pht::Logger::log(pht::Logger::LogLevel level, const std::string& tag, const std::string& file, uint32_t line, const std::string& func, bool quiet) {
    return LogStream(level, tag, file, line, func, quiet, true);
}

void pht::Logger::_log(pht::Logger::LogLevel level, const std::string& tag, const std::string& message, const std::string& file, uint32_t line, const std::string& func) {
    if(getStatics().logLevel > level) {
        return;
    }

    std::string msg = std::regex_replace(message, getStatics().unifyLinebreakRegex, "\n");

    #ifdef PHT_LOGGER_COLORIZE
    switch(level) {
        case LogLevel::PHT_DEBUG:   *getStatics().stdOutWrapper << "\033[3m"; break;
        case LogLevel::PHT_INFO:    *getStatics().stdOutWrapper << "\033[1m"; break;
        case LogLevel::PHT_WARNING: *getStatics().stdOutWrapper << "\033[1;33m"; break;
        case LogLevel::PHT_ERROR:   *getStatics().stdOutWrapper << "\033[1;31m"; break;
        case LogLevel::PHT_FATAL:   *getStatics().stdOutWrapper << "\033[1;37;41m"; break;
    }
    #endif

    getStatics().timer.stop();
    *getStatics().stdOutWrapper << getStatics().timer;

    *getStatics().stdOutWrapper << " [";
    switch(level) {
        case LogLevel::PHT_DEBUG:   *getStatics().stdOutWrapper << "DBG"; break;
        case LogLevel::PHT_INFO:    *getStatics().stdOutWrapper << "INF"; break;
        case LogLevel::PHT_WARNING: *getStatics().stdOutWrapper << "WRN"; break;
        case LogLevel::PHT_ERROR:   *getStatics().stdOutWrapper << "ERR"; break;
        case LogLevel::PHT_FATAL:   *getStatics().stdOutWrapper << "FAT"; break;
        default:                    *getStatics().stdOutWrapper << "???"; break;
    }
    *getStatics().stdOutWrapper << "]";

    std::string tagFill(tag.length()>PHT_LOGGER_TAG_WIDTH?0:PHT_LOGGER_TAG_WIDTH-tag.length(), ' ');
    *getStatics().stdOutWrapper << "[" << tag.substr(0, tag.length()>PHT_LOGGER_TAG_WIDTH?PHT_LOGGER_TAG_WIDTH-3:tag.length()) << (tag.length()>PHT_LOGGER_TAG_WIDTH?"...":"") << "]: " << tagFill;

    std::istringstream ss(msg);
    std::string msgLine;
    bool firstLine = true;
    size_t charCount = 0; //HACK Fixes last-empty-line-skip-bug
    while(std::getline(ss, msgLine)) {
        if(firstLine) {
            firstLine = false;
        } else {
            *getStatics().stdOutWrapper << "\n" << std::string(PHT_LOGGER_MSG_OFFSET, ' ');
        }
        *getStatics().stdOutWrapper << msgLine.substr(0, msgLine.length()>PHT_LOGGER_MSG_WIDTH?PHT_LOGGER_MSG_WIDTH-3:msgLine.length()) << (msgLine.length()>PHT_LOGGER_MSG_WIDTH?"...":"");
        charCount += msgLine.length()+1;
    }
    charCount--;
    if(charCount != msg.length() && !msg.empty() && msg.at(msg.length()-1) == '\n') {
        *getStatics().stdOutWrapper << "\n" << std::string(PHT_LOGGER_MSG_OFFSET, ' ');
    }
    if(msgLine.length() < PHT_LOGGER_MSG_WIDTH) {
        *getStatics().stdOutWrapper << std::string(PHT_LOGGER_MSG_WIDTH-msgLine.length(), ' ');
    }

    if(!file.empty()) {
        std::filesystem::path filePath = file;
        *getStatics().stdOutWrapper << " (" << filePath.filename().string() << ":" << line << ":" << func << "(...))";
    } else {
        *getStatics().stdOutWrapper << " (" << "???" << ":" << line << ":" << func << "(...))";
    }

    #ifdef PHT_LOGGER_COLORIZE
    *getStatics().stdOutWrapper << "\033[0m" << std::endl;
    #endif
}

pht::Logger::LogLevel pht::Logger::getLogLevel() {
    return getStatics().logLevel;
}

void pht::Logger::setLogLevel(pht::Logger::LogLevel level) {
    getStatics().logLevel = level;
}

void pht::Logger::setStdOutEnabled(bool enabled) {
    if(enabled) {
        std::cout.rdbuf(getStatics().stdoutBuf);
    } else {
        getStatics().stdoutBuf = std::cout.rdbuf();
        std::cout.rdbuf(nullptr);
    }
}

pht::Logger::LoggerStatics& pht::Logger::getStatics() {
    static LoggerStatics statics;
    return statics;
}

pht::Logger::LoggerStatics::LoggerStatics() {
    logLevel = pht::Logger::LogLevel::PHT_DEBUG;
    timer = pht::Timer();
    unifyLinebreakRegex = std::regex("\r\n?");
    stdoutBuf = std::cout.rdbuf();
    stdOutWrapper = std::make_unique<std::ostream>(stdoutBuf);
}
