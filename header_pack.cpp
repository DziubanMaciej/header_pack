#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

void printHelp() {
    const char *helpMessage = R"DELIMETER(
Usage: header_pack INPUT_FILE OUTPUT_FILE [options...]

Options:
    -t - text mode. Produces const char* containing text from the input file.
    -b - binary mode. Produces const unsigned char[] containing bytes from the input file.
    -n - name. Selects name of the variable defined in the output file. Optional.
    -c - hex values per line. Only applicable to binary mode. Must be a positive number. Optional.
)DELIMETER";
    printf("%s\n", helpMessage);
}

[[noreturn]] void printHelpAndExit() {
    printHelp();
    exit(1);
}

[[noreturn]] void printErrorAndExit(bool showHelp, const char *format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    printf("ERROR: %s\n", buffer);

    if (showHelp) {
        printHelp();
    }

    exit(1);
}

struct Options {
    enum class Mode {
        Unknown,
        Text,
        Binary
    };
    Mode mode = Mode::Unknown;
    const char *variableName = "var";
    unsigned int hexValuesPerLine = 16;
};

std::filesystem::path parseFilePath(int &argIndex, int argc, const char **argv) {
    if (argIndex >= argc) {
        printHelpAndExit();
    }
    return argv[argIndex++];
}

Options parseOptions(int &argIndex, int argc, const char **argv) {
    Options options = {};
    for (; argIndex < argc; argIndex++) {
        const std::string option = argv[argIndex];
        if (option == "-t") {
            options.mode = Options::Mode::Text;
        } else if (option == "-b") {
            options.mode = Options::Mode::Binary;
        } else if (option == "-n") {
            if (argIndex + 1 == argc) {
                printErrorAndExit(false, "specify variable name after %s", option.c_str());
            }
            options.variableName = argv[++argIndex];
        } else if (option == "-c") {
            if (argIndex + 1 == argc) {
                printErrorAndExit(false, "specify number of hex values per line after %s", option.c_str());
            }
            options.hexValuesPerLine = std::atoi(argv[++argIndex]);
        } else {
            printErrorAndExit(true, "unrecognized option: %s", option.c_str());
        }
    }

    if (options.mode == Options::Mode::Unknown) {
        printErrorAndExit(true, "specify operation mode");
    }
    if (strlen(options.variableName) == 0) {
        printErrorAndExit(true, "variable name is empty");
    }
    if (options.mode == Options::Mode::Binary && options.hexValuesPerLine == 0) {
        printErrorAndExit(true, "hex values per line is 0");
    }

    return options;
}

int main(const int argc, const char **argv) {
    int argIndex = 1;
    const std::filesystem::path inputFile = parseFilePath(argIndex, argc, argv);
    const std::filesystem::path outputFile = parseFilePath(argIndex, argc, argv);
    const Options options = parseOptions(argIndex, argc, argv);

    // Open input file
    int inputOpenMode = std::ios::in;
    if (options.mode == Options::Mode::Binary) {
        inputOpenMode |= std::ios::binary;
    }
    std::ifstream inputStream(inputFile, inputOpenMode);
    if (!inputStream) {
        printErrorAndExit(false, "Could not open input file %s", inputFile.c_str());
    }

    // Open output file
    std::ofstream outputStream(outputFile, std::ios::out);
    if (!outputStream) {
        printErrorAndExit(false , "Could not open output file %s", outputFile.c_str());
    }

    // Execute text mode
    if (options.mode == Options::Mode::Text) {
        outputStream << "#pragma once\n\n";
        outputStream << "const char *" << options.variableName << " = R\"DELIMETER(";

        constexpr size_t chunkSize = 4096;
        char chunk[chunkSize];
        while (inputStream) {
            inputStream.read(chunk, chunkSize);
            if (inputStream.good()) {
                outputStream.write(chunk, chunkSize);
            } else if (inputStream.eof()) {
                outputStream.write(chunk, inputStream.gcount());
            } else {
                printErrorAndExit(false, "Failed reading input file %s", inputFile);
            }
        }

        outputStream << ")DELIMETER\";\n";
        outputStream.close();
        exit(0);
    }

    // Execute binary mode
    if (options.mode == Options::Mode::Binary) {
        outputStream << "#pragma once\n\n";
        outputStream << "const char " << options.variableName << "[] = {\n";

        constexpr size_t chunkSize = 4096;
        char chunk[chunkSize];
        size_t bytesInCurrentLine = 0;
        while (inputStream) {
            inputStream.read(chunk, chunkSize);
            size_t bytesToWrite = 0;
            if (inputStream.good()) {
                bytesToWrite = chunkSize;
            } else if (inputStream.eof()) {
                bytesToWrite = inputStream.gcount();
            } else {
                printErrorAndExit(false, "Failed reading input file %s", inputFile);
            }

            for (size_t byteIndex = 0; byteIndex < bytesToWrite; byteIndex++) {
                const bool startLine = bytesInCurrentLine == 0;
                if (startLine) {
                    outputStream << "    ";
                }

                outputStream
                    << "0x"
                    << std::setfill('0')
                    << std::setw(2)
                    << std::hex
                    << static_cast<unsigned int>(chunk[byteIndex])
                    << ',';

                const bool breakLine = bytesInCurrentLine == options.hexValuesPerLine - 1 || byteIndex == bytesToWrite - 1;
                if (breakLine) {
                    outputStream << "\n";
                    bytesInCurrentLine = 0;
                } else {
                    outputStream << ' ';
                    bytesInCurrentLine++;
                }
            }
        }

        outputStream << "};\n";
        outputStream.close();
        exit(0);
    }

    printErrorAndExit(true, "Unsupported mode");
}
