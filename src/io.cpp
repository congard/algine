#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef __linux__
#include <unistd.h>
#define _getCurrentDir getcwd
#elif __MINGW32__
#include <direct.h>
#define _getCurrentDir _getcwd
#endif

#include <algine/io.h>

namespace algine {
namespace io {
char *read(const char *path) {
    FILE *file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *result = new char[size + 1];
    fseek(file, 0, SEEK_SET);
    fread(result, 1, size, file);
    fclose(file);
    result[size] = '\0';
    return result;
}

void save(FILE *file, const std::string &data, const std::string &path) {
    fwrite(
        data.c_str(),
        1, data.size(),
        file = fopen(path.c_str(), "wb")
    );

    fclose(file);
}

std::string read(const std::string &path) {
    char *data = read(path.c_str());
    std::string data_str = data;
    delete[] data;
    return data_str;
}

bool exists(const char *path) {
    FILE *file = fopen(path, "r");
    if (file) fclose(file);
    return file;
}

bool isAbsolutePath(const std::string &path) {
    #ifdef __linux__
    return path.find('/') == 0;
    #elif __MINGW32__
    return path.find(':') == 1 && (path.find('/') == 2 || path.find('\\') == 2);
    #endif
}

std::string getCurrentDir() {
    char buff[FILENAME_MAX];
    _getCurrentDir(buff, FILENAME_MAX);
    return std::string(buff);
}

std::string getAbsolutePath(const std::string &path) {
    return isAbsolutePath(path) ? path : merge(getCurrentDir(), path);
}

#if defined(__linux__) || defined(__MINGW32__)
#define _fileSeparator '/'
#else
#error "Only linux and mingw supported!"
#endif

std::string merge(const std::string &path1, const std::string &path2) {
    std::string first = path1;
    std::string second = path2;

    if (first.back() != _fileSeparator)
        first += _fileSeparator;

    if (second.front() == _fileSeparator)
        second.erase(0, 1);

    return first + second;
}

std::string upDir(const std::string &dir) {
    if (dir.find_last_of(_fileSeparator) == std::string::npos)
        return dir;

    return std::string(dir).erase(dir.find_last_of(_fileSeparator), dir.size());
}

}
}