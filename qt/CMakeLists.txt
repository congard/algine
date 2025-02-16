set(ALGINE_QT_HEADERS
        include/algine/log/QtLogWriter.h
)

set(ALGINE_QT_SOURCES
        src/log/QtLogWriter.cpp
)

target_sources(algine
        PUBLIC ${ALGINE_QT_HEADERS}
        PRIVATE ${ALGINE_QT_SOURCES}
)