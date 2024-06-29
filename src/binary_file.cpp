#include "../include/binary_file.h"

namespace binary_file {
    _4bytes BinaryFile::join(const std::vector<byte>& bytes) {
        _4bytes joined{ 0 };

        size_t i{ 0 };
        for (const auto byte : bytes) {
            joined += byte << (i++ * 8);
        }

        return joined;
    }

    BinaryFile::BinaryFile(const fs::path& path) {
        if (!fs::exists(path)) {
            throw BinaryFileException(fmt::format(
                "Binary file {} does not exist",
                path.string()
            ));
        }

        if (!fs::is_regular_file(path)) {
            throw BinaryFileException(fmt::format(
                "{} is not a regular file",
                path.string()
            ));
        }

        std::ifstream file(path, std::ios::binary);

        if (!file) {
            throw BinaryFileException(fmt::format(
                "Failed to open binary file {} for reading",
                path.string()
            ));
        }

        bytes.reserve(fs::file_size(path));

        bytes.insert(bytes.begin(),
            std::istream_iterator<byte>(file),
            std::istream_iterator<byte>());
    }

    BinaryFile::BinaryFile(std::vector<byte>&& bytes) : bytes(std::move(bytes)) {}

    std::vector<byte> BinaryFile::read(size_t offset, size_t byte_count) const {
        const auto ending_byte_offset{ offset + byte_count };

        if (ending_byte_offset > bytes.size()) {
            throw BinaryFileException(fmt::format(
                "Attempt to read binary data from offset 0x{:X} "
                "to 0x{:X} inclusive, but final valid offset is 0x{:X}",
                offset, ending_byte_offset - 1, bytes.size() - 1
            ));
        }

        std::vector<byte> read_bytes{};
        while (offset != ending_byte_offset) {
            read_bytes.push_back(bytes.at(offset++));
        }

        return read_bytes;
    }

    byte BinaryFile::read1(size_t offset) const {
        return join(read(offset, 1));
    }
    
    _2bytes BinaryFile::read2(size_t offset) const {
        return join(read(offset, 2));
    }

    _4bytes BinaryFile::read3(size_t offset) const {
        return join(read(offset, 3));
    }

    _4bytes BinaryFile::read4(size_t offset) const {
        return join(read(offset, 4));
    }

    std::vector<byte> BinaryFile::split(_4bytes bytes, size_t byte_count) {
        std::vector<byte> split{};

        size_t i{ 0 };
        while (i != byte_count) {
            split.push_back((bytes >> (i++ * 8)) & 0xFF);
        }

        return split;
    }

    void BinaryFile::write(size_t offset, const std::vector<byte>& bytes_to_write) {
        const auto ending_byte_offset{ offset + bytes_to_write.size() };

        if (ending_byte_offset > bytes.size()) {
            throw BinaryFileException(fmt::format(
                "Attempt to write the {} byte(s) 0x{:X} at offset 0x{:X} "
                "to 0x{:X} inclusive, but final valid offset is 0x{:X}",
                bytes_to_write.size(), join(bytes_to_write), offset, ending_byte_offset - 1, bytes.size() - 1
            ));
        }

        size_t i{ 0 };
        while (offset != ending_byte_offset) {
            bytes[offset++] = bytes_to_write[i];
        }
    }

    void BinaryFile::write1(size_t offset, byte byte) {
        write(offset, { byte });
    }

    void BinaryFile::write2(size_t offset, _2bytes bytes_to_write) {
        write(offset, split(bytes_to_write, 2));
    }

    void BinaryFile::write3(size_t offset, _4bytes bytes_to_write) {
        write(offset, split(bytes_to_write, 3));
    }

    void BinaryFile::write4(size_t offset, _4bytes bytes_to_write) {
        write(offset, split(bytes_to_write, 4));
    }

    void BinaryFile::outputAt(const fs::path& path) const {
        std::ofstream file(path, std::ios::binary);

        if (!file) {
            throw BinaryFileException(fmt::format(
                "Failed to open binary file {} for writing",
                path.string()
            ));
        }

        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());

        if (!file) {
            throw BinaryFileException(fmt::format(
                "Failed to write data to binary file {}",
                path.string()
            ));
        }
    }

    void BinaryFile::output() const {
        if (!input_path.has_value()) {
            throw BinaryFileException(
                "Cannot output binary file without specifying an output path as it was not constructed"
                "from an input path"
            );
        }

        outputAt(input_path.value());
    }
    
    const std::optional<fs::path>& BinaryFile::getInputPath() const {
        return input_path;
    }
}
