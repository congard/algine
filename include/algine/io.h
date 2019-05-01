#ifndef ALGINE_IO_H
#define ALGINE_IO_H

#include <iostream>

namespace algine {
namespace io {

char *read(const char *path);

void save(FILE *file, const std::string &data, const std::string &path);

std::string read(const std::string &path);

bool exists(const char *path);

bool isAbsolutePath(const std::string &path);

std::string getCurrentDir();

std::string getAbsolutePath(const std::string &path);

}
}

#endif