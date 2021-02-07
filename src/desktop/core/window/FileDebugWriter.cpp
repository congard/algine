#include <algine/core/window/FileDebugWriter.h>

namespace algine {
FileDebugWriter::FileDebugWriter(const std::string &file) {
    m_stream.open(file, std::ios::app);
}

FileDebugWriter::~FileDebugWriter() {
    m_stream.close();
}

std::ostream& FileDebugWriter::stream() {
    return m_stream;
}
}
