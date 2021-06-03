#ifndef ALGINE_FILETRANSFERABLE_H
#define ALGINE_FILETRANSFERABLE_H

#include <algine/core/transfer/Transferable.h>
#include <algine/core/io/IOWrapper.h>

#include <string>

namespace algine {
class FileTransferable: public Transferable, public IOWrapper {
public:
    void setWorkingDirectory(const std::string &path);
    const std::string& getWorkingDirectory() const;

    virtual void importFromFile(const std::string &path);
    virtual void dumpToFile(const std::string &path);

protected:
    std::string m_workingDirectory;
};
}

#endif //ALGINE_FILETRANSFERABLE_H
