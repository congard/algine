#ifndef ALGINE_IO_CPP
#define ALGINE_IO_CPP

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef __linux__
#include <unistd.h>
#define _getCurrentDir getcwd
#elif __MINGW32__
#include <direct.h>
#define _getCurrentDir _getcwd
#endif

#include <iostream>

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
    return isAbsolutePath(path) ? path : getCurrentDir() + "/" + path;
}

}
}

#endif /* ALGINE_IO_CPP */