#include <algine/core/Serializable.h>

#include <algine/core/JsonHelper.h>

#include <tulz/File.h>

using namespace std;
using namespace tulz;

namespace algine {
void Serializable::deserializeFromFile(const string &path) {
    deserialize(File(path, File::ReadText).readStr());
}
}
