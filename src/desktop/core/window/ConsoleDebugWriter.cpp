#include <algine/core/window/ConsoleDebugWriter.h>

#include <iostream>

namespace algine {
std::ostream& ConsoleDebugWriter::stream() {
    return std::cout;
}
}
