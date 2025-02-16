set(ALGINE_IO_HEADERS
        ${ALGINE_HEADERS}/io/IOProvider.h
        ${ALGINE_HEADERS}/io/IOStream.h
        ${ALGINE_HEADERS}/io/IOSystem.h
        ${ALGINE_HEADERS}/io/IOSystemPtr.h
        ${ALGINE_HEADERS}/io/StandardIOStream.h
        ${ALGINE_HEADERS}/io/StandardIOSystem.h
)

set(ALGINE_IO_SOURCES
        IOProvider.cpp
        IOStream.cpp
        IOSystem.cpp
        StandardIOStream.cpp
        StandardIOSystem.cpp
)

target_sources(algine
        PUBLIC ${ALGINE_IO_HEADERS}
        PRIVATE ${ALGINE_IO_SOURCES}
)