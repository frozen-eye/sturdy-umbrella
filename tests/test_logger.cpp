#include <iostream>
#include <cassert>
#include <gtest/gtest.h>
#include "../include/logger.hpp"

TEST(LoggerTest, CreateConsoleLoggerWithLevel)
{
    std::unique_ptr<ILogger> logger = LoggerFactory::createConsoleLoggerWithLevel();
    ASSERT_NE(logger, nullptr);
    // Add additional assertions to test the behavior of the logger
}

TEST(LoggerTest, CreateFileLoggerWithLevelFilter)
{
    std::string filename = "test.log";
    std::unique_ptr<LevelFilterDecorator> logger = LoggerFactory::createFileLoggerWithLevelFilter(filename);
    ASSERT_NE(logger, nullptr);
    logger->log(LogLevel::Info, "This message will be filtered out");
    logger->setMinLevel(LogLevel::Info);
    // Add additional assertions to test the behavior of the logger
}


TEST(LoggerTest, CreateFileLoggerWithLevelAndTimestamp)
{
    std::string filename = "test.log";
    std::unique_ptr<ILogger> logger = LoggerFactory::createFileLoggerWithLevelAndTimestamp(filename);
    ASSERT_NE(logger, nullptr);
    // Add additional assertions to test the behavior of the logger
}

TEST(LoggerTest, ConsoleLoggerOutput)
{
    // Arrange
    std::stringstream output;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(output.rdbuf()); // Redirect cout to stringstream

    std::unique_ptr<ILogger> logger = std::make_unique<Logger>(std::make_unique<ConsoleOutput>());
    std::string message = "Test message";

    // Act
    logger->log(LogLevel::Info, message);

    // Assert
    std::cout.rdbuf(oldCoutBuffer); // Restore cout buffer
    ASSERT_EQ(output.str(), message + "\n");
}

TEST(LoggerTest, FileLoggerOutput)
{
    // Arrange
    std::string filename = "test.log";
    std::ofstream file(filename);
    std::unique_ptr<ILogger> logger = std::make_unique<Logger>(std::make_unique<FileOutput>(filename));
    std::string message = "Test message";

    // Act
    logger->log(LogLevel::Info, message);
    file.close();

    // Assert
    std::ifstream inputFile(filename);
    std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    ASSERT_EQ(fileContent, message + "\n");
}

TEST(LoggerTest, LevelFilterLoggerOutput)
{
    // Arrange
    std::stringstream output;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(output.rdbuf()); // Redirect cout to stringstream

                                      

    std::unique_ptr<ILogger> logger = std::make_unique<LevelFilterDecorator>(std::make_unique<Logger>(std::make_unique<ConsoleOutput>()), LogLevel::Warning);
    std::string message1 = "Info message";
    std::string message2 = "Warning message";

    // Act
    logger->log(LogLevel::Info, message1);
    logger->log(LogLevel::Warning, message2);

    // Assert
    std::cout.rdbuf(oldCoutBuffer); // Restore cout buffer
    ASSERT_EQ(output.str(), message2 + "\n");
}

TEST(LoggerTest, TimestampLoggerOutput)
{
    // Arrange
    std::stringstream output;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(output.rdbuf()); // Redirect cout to stringstream

    std::unique_ptr<ILogger> logger = std::make_unique<TimestampDecorator>(std::make_unique<Logger>(std::make_unique<ConsoleOutput>()));
    std::string message = "Test message";

    // Act
    logger->log(LogLevel::Info, message);

    // Assert
    std::cout.rdbuf(oldCoutBuffer); // Restore cout buffer
    ASSERT_TRUE(output.str().find("[") != std::string::npos);
    ASSERT_TRUE(output.str().find("]") != std::string::npos);
    ASSERT_TRUE(output.str().find(message) != std::string::npos);
}

TEST(LoggerTest, MultiOutputLogger)
{
    // Arrange
    std::stringstream consoleOutput;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(consoleOutput.rdbuf()); // Redirect cout to stringstream

    std::string filename = "test.log";
    std::ofstream file(filename);
    std::unique_ptr<IOutputStrategy> consoleOutputStrategy = std::make_unique<ConsoleOutput>();
    std::unique_ptr<IOutputStrategy> fileOutputStrategy = std::make_unique<FileOutput>(filename);
    std::vector<std::unique_ptr<IOutputStrategy>> outputs;
    outputs.push_back(std::move(consoleOutputStrategy));
    outputs.push_back(std::move(fileOutputStrategy));

    auto outputStrategy = std::make_unique<MultiOutput>(std::move(outputs));
    std::string message = "Test message";

    // Act
    auto multi_logger = std::make_unique<Logger>(std::move(outputStrategy));
    multi_logger->log(LogLevel::Info, message);
    file.close();

    // Assert
    std::cout.rdbuf(oldCoutBuffer); // Restore cout buffer
    ASSERT_EQ(consoleOutput.str(), message + "\n");

    std::ifstream inputFile(filename);
    std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    ASSERT_EQ(fileContent, message + "\n");
}
