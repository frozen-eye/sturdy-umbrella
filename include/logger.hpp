/**
 * @file logger.hpp
 * @brief This file contains the declaration and implementation of classes related to logging functionality.
 */

#pragma once
#include <chrono>
#include <algorithm>
#include <iostream>
#include <memory>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

// Log Level
enum class LogLevel {
    Fatal,
    Error,
    Warning,
    Info,
    Debug,
    Noise
};

/**
 * @brief Represents a network service for sending messages.
 */
class NetworkService {
public:
    /**
     * @brief Constructs a NetworkService object with the specified service URL.
     * 
     * @param serviceUrl The URL of the network service.
     */
    NetworkService(const std::string& serviceUrl) {}

    /**
     * @brief Sends a message through the network service.
     * 
     * @param message The message to be sent.
     */
    void send(const std::string& message) {}
};

/**
 * @brief The ILogger class is an interface for logging messages with different log levels.
 */
class ILogger {
public:
    virtual ~ILogger() = default;

    /**
     * @brief Logs a message with the specified log level.
     * 
     * @param level The log level of the message.
     * @param message The message to be logged.
     */
    virtual void log(LogLevel level, const std::string& message) = 0;
};

/**
 * @brief Interface for output strategies.
 * 
 * This interface defines the contract for output strategies in the logger module.
 * Any class that implements this interface must provide an implementation for the
 * output() function, which is responsible for outputting a message.
 */
class IOutputStrategy {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~IOutputStrategy() = default;

    /**
     * @brief Outputs a message.
     * 
     * This function is responsible for outputting the given message.
     * 
     * @param message The message to be outputted.
     */
    virtual void output(const std::string& message) = 0;
};

/**
    * @brief Outputs the given message to the console.
    * 
    * @param message The message to be outputted.
    */
class ConsoleOutput : public IOutputStrategy {
public:
    void output(const std::string& message) override {
        std::cout << message << std::endl;
    }
};

/**
 * @brief A class that represents a file output strategy for logging.
 * 
 * This class inherits from the IOutputStrategy interface and provides
 * functionality to output log messages to a file.
 */
class FileOutput : public IOutputStrategy {
private:
    std::ofstream file;
public:
    /**
     * @brief Constructs a FileOutput object with the specified filename.
     * 
     * @param filename The name of the file to output log messages to.
     * @throws std::runtime_error if the file cannot be opened.
     */
    FileOutput(const std::string& filename) : file(filename, std::ios::app) {
        if (!file) {
            throw std::runtime_error("Unable to open file");
        }
    }

    /**
     * @brief Outputs the specified message to the file.
     * 
     * @param message The log message to be outputted.
     */
    void output(const std::string& message) override {
        file << message << std::endl;
    }
};

/**
 * @brief A class that represents an output strategy for sending log messages to a network service.
 * 
 * This class inherits from the IOutputStrategy interface and uses a NetworkService object to send messages to a network service.
 */
class NetworkOutput : public IOutputStrategy {
private:
    NetworkService networkService; /**< The NetworkService object used to send messages to a network service. */
public:
    /**
     * @brief Constructs a NetworkOutput object with the specified service URL.
     * 
     * @param serviceUrl The URL of the network service.
     */
    NetworkOutput(const std::string& serviceUrl) : networkService(serviceUrl) {}

    /**
     * @brief Sends the specified message to the network service.
     * 
     * @param message The log message to be sent.
     */
    void output(const std::string& message) override {
        networkService.send(message);
    }
};

// Composite Output Strategy
/**
 * @brief A class that represents a multi-output strategy for logging.
 * 
 * This class inherits from the IOutputStrategy interface and allows multiple output strategies to be used simultaneously.
 * When the `output` function is called, it forwards the log message to all the registered output strategies.
 */
class MultiOutput : public IOutputStrategy {
private:
    std::vector<std::unique_ptr<IOutputStrategy>> outputs; /**< The vector of output strategies. */
public:
    /**
     * @brief Constructs a MultiOutput object with the given output strategies.
     * 
     * @param outputs The vector of output strategies.
     */
    MultiOutput(std::vector<std::unique_ptr<IOutputStrategy>> outputs) : outputs(std::move(outputs)) {}

    /**
     * @brief Outputs the log message to all the registered output strategies.
     * 
     * @param message The log message to be outputted.
     */
    void output(const std::string& message) override {
        for (auto& output : outputs) {
            output->output(message);
        }
    }
};

/**
 * @brief The base class for logger decorators.
 * 
 * This class is derived from the ILogger interface and provides a common base for all logger decorators.
 * Logger decorators can be used to add additional functionality to an existing logger.
 */
class ILoggerDecorator : public ILogger {
protected:
    std::unique_ptr<ILogger> logger;
public:
    /**
     * @brief Constructs an ILoggerDecorator object with the specified logger.
     * 
     * @param logger The logger to be decorated.
     */
    ILoggerDecorator(std::unique_ptr<ILogger> logger) : logger(std::move(logger)) {}

    /**
     * @brief Default destructor.
     */
    virtual ~ILoggerDecorator() = default;
};

// Level Logger Decorator class
/**
 * @brief Decorator class that adds log level information to the log message.
 */
class LogLevelDecorator : public ILoggerDecorator {
public:
    /**
     * @brief Constructs a LogLevelDecorator object with the specified logger.
     * @param logger The logger to be decorated.
     */
    LogLevelDecorator(std::unique_ptr<ILogger> logger) : ILoggerDecorator(std::move(logger)) {}

    /**
     * @brief Logs the message with the specified log level.
     * @param level The log level.
     * @param message The log message.
     */
    void log(LogLevel level, const std::string& message) override {
        // Add log level to message
        std::string leveledMessage = "[" + std::to_string(static_cast<int>(level)) + "] " + message;
        logger->log(level, leveledMessage);
    }
};

// Level Filter Logger Decorator class
/**
 * @brief Decorator class that filters log messages based on the minimum log level.
 * 
 * The LevelFilterDecorator class is a concrete implementation of the ILoggerDecorator interface.
 * It decorates an ILogger object and filters log messages based on the minimum log level specified.
 * Log messages with a level lower than or equal to the minimum log level will be passed to the decorated logger.
 */
class LevelFilterDecorator : public ILoggerDecorator {
private:
    LogLevel minLevel; /**< The minimum log level for filtering log messages. */
public:
    /**
     * @brief Constructs a LevelFilterDecorator object with the specified logger and minimum log level.
     * 
     * @param logger The logger to be decorated.
     * @param minLevel The minimum log level for filtering log messages. Default is LogLevel::Info.
     */
    LevelFilterDecorator(std::unique_ptr<ILogger> logger, LogLevel minLevel = LogLevel::Info) 
        : ILoggerDecorator(std::move(logger)), minLevel(minLevel) {}

    /**
     * @brief Logs a message with the specified log level, if it meets the minimum log level requirement.
     * 
     * This function checks if the specified log level is lower than or equal to the minimum log level.
     * If it is, the log message is passed to the decorated logger for logging.
     * 
     * @param level The log level of the message.
     * @param message The log message to be logged.
     */
    void log(LogLevel level, const std::string& message) override {
        if (static_cast<int>(level) <= static_cast<int>(minLevel)) {
            logger->log(level, message);
        }
    }

    /**
     * @brief Sets the minimum log level for filtering log messages.
     * 
     * @param newMinLevel The new minimum log level to be set.
     */
    void setMinLevel(LogLevel newMinLevel) {
        minLevel = newMinLevel;
    }
};

// Timestamp Logger Decorator class
/**
 * @brief Decorator class that adds a timestamp to log messages.
 * 
 * This class inherits from the ILoggerDecorator interface and wraps an existing ILogger object.
 * It adds a timestamp to the log messages before forwarding them to the wrapped logger.
 */
class TimestampDecorator : public ILoggerDecorator {
public:
    /**
     * @brief Constructs a TimestampDecorator object with the specified logger.
     * 
     * @param logger The logger object to be wrapped.
     */
    TimestampDecorator(std::unique_ptr<ILogger> logger) : ILoggerDecorator(std::move(logger)) {}

    /**
     * @brief Logs a message with the specified log level and adds a timestamp to it.
     * 
     * @param level The log level of the message.
     * @param message The message to be logged.
     */
    void log(LogLevel level, const std::string& message) override {
        // Get current time
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream timestamp;
        timestamp << std::put_time(std::localtime(&now_c), "%F %T");

        // Add timestamp to message
        std::string timestampedMessage = "[" + timestamp.str() + "] " + message;
        logger->log(level, timestampedMessage);
    }
};

// Logger with output strategy
/**
 * @brief The Logger class is responsible for logging messages using a specified output strategy.
 */
class Logger : public ILogger {
private:
    std::unique_ptr<IOutputStrategy> outputStrategy;
public:
    /**
     * @brief Constructs a Logger object with the specified output strategy.
     * 
     * @param outputStrategy The output strategy to be used for logging.
     */
    Logger(std::unique_ptr<IOutputStrategy> outputStrategy) : outputStrategy(std::move(outputStrategy)) {}

    /**
     * @brief Logs a message with the specified log level.
     * 
     * @param level The log level of the message.
     * @param message The message to be logged.
     */
    void log(LogLevel level, const std::string& message) override {
        outputStrategy->output(message);
    }
};

// Logger Factory
/**
 * @brief The LoggerFactory class is responsible for creating instances of ILogger.
 * 
 * It provides static methods to create different types of loggers with various decorators.
 */
class LoggerFactory {
public:
    /**
     * @brief Creates a console logger with level decorator.
     * 
     * @return A unique pointer to the created ILogger instance.
     */
    static std::unique_ptr<ILogger> createConsoleLoggerWithLevel()
    {
        return std::make_unique<LogLevelDecorator>(std::make_unique<Logger>(std::make_unique<ConsoleOutput>()));
    }

    /**
     * @brief Creates a file logger with level filter decorator.
     * 
     * @param filename The name of the file to log to.
     * @return A unique pointer to the created ILogger instance.
     */
    static std::unique_ptr<LevelFilterDecorator> createFileLoggerWithLevelFilter(std::string filename)
    {
        return std::make_unique<LevelFilterDecorator>(std::make_unique<Logger>(std::make_unique<FileOutput>(filename)));
    }

    /**
     * @brief Creates a file logger with level and timestamp decorators.
     * 
     * @param filename The name of the file to log to.
     * @return A unique pointer to the created ILogger instance.
     */
    static std::unique_ptr<ILoggerDecorator> createFileLoggerWithLevelAndTimestamp(std::string filename)
    {
        return std::make_unique<TimestampDecorator>(std::make_unique<Logger>(std::make_unique<FileOutput>(filename)));
    }
};
