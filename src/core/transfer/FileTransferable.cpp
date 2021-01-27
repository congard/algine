#include <algine/core/transfer/FileTransferable.h>

#include <algine/core/JsonHelper.h>

#include <tulz/File.h>

using namespace std;
using namespace tulz;

namespace algine {
void FileTransferable::setWorkingDirectory(const string &path) {
    m_workingDirectory = path;
}

const string& FileTransferable::getWorkingDirectory() const {
    return m_workingDirectory;
}

void FileTransferable::importFromFile(const string &path) {
    string m_confPath;

    if (!m_workingDirectory.empty()) {
        m_confPath = Path::join(m_workingDirectory, path);
        m_workingDirectory = Path(m_confPath).getParentDirectory().toString();
    } else {
        m_confPath = path;
        m_workingDirectory = Path(path).getParentDirectory().toString();
    }

    import(File(m_confPath, File::Mode::ReadText).readStr());
}

void FileTransferable::dumpToFile(const string &path) {
    File(path, File::Mode::WriteText).write(dump().toString());
}
}
