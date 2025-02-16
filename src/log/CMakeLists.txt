set(ALGINE_LOG_HEADERS
        ${ALGINE_HEADERS}/log/DefaultLogWriter.h
        ${ALGINE_HEADERS}/log/Log.h
        ${ALGINE_HEADERS}/log/Logger.h
        ${ALGINE_HEADERS}/log/LogWriter.h
)

set(ALGINE_LOG_SOURCES
        DefaultLogWriter.cpp
        Log.cpp
        Logger.cpp
)

target_sources(algine
        PUBLIC ${ALGINE_LOG_HEADERS}
        PRIVATE ${ALGINE_LOG_SOURCES}
)