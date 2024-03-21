# C++ Logger Test (not for embedded)

This project contains unit tests for a simple C++ logger.

## Project Structure

The project is structured as follows:

- `tests/`: This directory contains the unit tests for the logger.

## Logger

The logger is a simple collection of C++ classes that provide logging functionality. It supports different log levels (e.g., INFO, WARNING, ERROR) and writes log messages to a different output streams like stdout, file.

## Unit Tests

The unit tests are written using the Google Test framework. They cover all major functionalities of the logger.

## Building the Project

To build the project, run the following commands in the terminal:

```bash
$ mkdir build
$ cd build && cmake .. 
$ make all
```

To cleanup the project, run the following command in the terminal:

```bash
$ make clean
```

Running the Tests
To run the tests, execute the following command in the terminal:

```bash
make test && ./runUnitTests
```

## Dependencies
This project depends on the Google Test framework for the unit tests. Make sure to install it before building the project.

## Contributing
Contributions are welcome. Please submit a pull request with your changes.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.
