#ifndef BINARY_FILE_H
#define BINARY_FILE_H

#include <filesystem>
#include <utility>
#include <optional>
#include <vector>
#include <fstream>

#include "fmt/format.h"

namespace fs = std::filesystem;

namespace binary_file {
    using byte = uint8_t;
    using _2bytes = uint16_t;
    using _4bytes = uint32_t;

    class BinaryFileException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class BinaryFile {
    protected:
        std::vector<byte> bytes;
        const std::optional<fs::path> input_path;

        static _4bytes join(const std::vector<byte>& bytes);
        static std::vector<byte> split(_4bytes bytes, size_t byte_count);

        std::vector<byte> read(size_t offset, size_t byte_count) const;
        void write(size_t offset, const std::vector<byte>& bytes_to_write);

    public:
        BinaryFile(const fs::path& path);
        BinaryFile(std::vector<byte>&& bytes);

        byte read1(size_t offset) const;
        _2bytes read2(size_t offset) const;
        _4bytes read3(size_t offset) const;
        _4bytes read4(size_t offset) const;

        void write1(size_t offset, byte byte_to_write);
        void write2(size_t offset, _2bytes bytes_to_write);
        void write3(size_t offset, _4bytes bytes_to_write);
        void write4(size_t offset, _4bytes bytes_to_write);

        void outputAt(const fs::path& path) const;
        void output() const;

        const std::optional<fs::path>& getInputPath() const;
    };
}

#endif //BINARY_FILE_H
