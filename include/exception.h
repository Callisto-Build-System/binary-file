#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>

namespace binary_file {
    class BinaryFileException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };
}

#endif // EXCEPTION_H
