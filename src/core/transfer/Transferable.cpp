#include <algine/core/transfer/Transferable.h>

#include <algine/core/JsonHelper.h>

#include <tulz/File.h>

using namespace std;
using namespace tulz;

namespace algine {
void Transferable::importFromFile(const string &path) {
    import(File(path, File::Mode::ReadText).readStr());
}
}
