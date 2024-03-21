#include <cstdlib>
#include <vector>

#include "include/logger.hpp"

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    auto logger = std::make_unique<LevelFilterDecorator>(LoggerFactory::createConsoleLoggerWithLevel(), LogLevel::Warning);
    logger->log(LogLevel::Info, "This message will be filtered out");
    logger->setMinLevel(LogLevel::Info);
    logger->log(LogLevel::Info, "This message will now be logged");

    // Create a MultiOutput strategy
    std::vector<std::unique_ptr<IOutputStrategy>> outputs;
    outputs.push_back(std::make_unique<ConsoleOutput>());
    outputs.push_back(std::make_unique<FileOutput>("log.txt"));
    outputs.push_back(std::make_unique<NetworkOutput>("syslog://localhost:514"));
    auto outputStrategy = std::make_unique<MultiOutput>(std::move(outputs));

    // Create a logger with the MultiOutput strategy
    auto multi_logger = std::make_unique<Logger>(std::move(outputStrategy));

    // log the message
    multi_logger->log(LogLevel::Info, "This message will be logged to the console and a file");

    // exit the applilcation
    return EXIT_SUCCESS;
}
