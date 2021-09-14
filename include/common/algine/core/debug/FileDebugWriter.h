#ifndef ALGINE_FILEDEBUGWRITER_H
#define ALGINE_FILEDEBUGWRITER_H

#include <algine/core/debug/DebugWriter.h>

#include <fstream>
#include <mutex>

namespace algine {
class FileDebugWriter: public DebugWriter {
public:
    explicit FileDebugWriter(const std::string &file = "debug.txt");
    ~FileDebugWriter() override;

    Logger logger() override;

private:
    void onInputEnd(Logger &logger) override;

private:
    std::ofstream m_stream;
    static std::mutex m_mutex;
};
}

#endif //ALGINE_FILEDEBUGWRITER_H
