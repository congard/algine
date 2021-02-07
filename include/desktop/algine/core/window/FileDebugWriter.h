#ifndef ALGINE_FILEDEBUGWRITER_H
#define ALGINE_FILEDEBUGWRITER_H

#include <algine/core/window/DebugWriter.h>

#include <fstream>

namespace algine {
class FileDebugWriter: public DebugWriter {
public:
    explicit FileDebugWriter(const std::string &file = "debug.txt");
    ~FileDebugWriter() override;

    std::ostream& stream() override;

private:
    std::ofstream m_stream;
};
}

#endif //ALGINE_FILEDEBUGWRITER_H
