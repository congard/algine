#include <algine/core/io/StandardIOStream.h>

#include <stdexcept>

using namespace tulz;

namespace algine {
void StandardIOStream::open(const std::string &path, IOStream::Mode mode) {
    m_file.open(Path(path), [&]() {
        switch (mode) {
            case Mode::None: return File::Mode::None;
            case Mode::ReadText: return File::Mode::ReadText;
            case Mode::Read: return File::Mode::Read;
            case Mode::WriteText: return File::Mode::WriteText;
            case Mode::Write: return File::Mode::Write;
            case Mode::AppendText: return File::Mode::AppendText;
            case Mode::Append: return File::Mode::Append;
        }

        throw std::invalid_argument("StandardIOStream: unsupported open mode");
    }());
}

void StandardIOStream::close() {
    m_file.close();
}

int StandardIOStream::flush() {
    return m_file.flush();
}

size_t StandardIOStream::size() const {
    return m_file.size();
}

size_t StandardIOStream::read(void *buffer, size_t size, size_t count) {
    return m_file.read(buffer, size, count);
}

size_t StandardIOStream::seek(size_t offset, int whence) {
    return m_file.seek((long) offset, static_cast<File::Origin>(whence));
}

size_t StandardIOStream::tell() const {
    return m_file.tell();
}

size_t StandardIOStream::write(const void *buffer, size_t size, size_t count) {
    return m_file.write(buffer, size, count);
}

void StandardIOStream::writeStr(const std::string &str) {
    m_file.write(str);
}

std::string StandardIOStream::readStr() {
    return m_file.readStr();
}
}
