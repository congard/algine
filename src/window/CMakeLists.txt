set(ALGINE_WINDOW_HEADERS
        ${ALGINE_HEADERS}/window/input/KeyboardKey.h
        ${ALGINE_HEADERS}/window/input/MouseKey.h

        ${ALGINE_HEADERS}/window/unified/Event.h
        ${ALGINE_HEADERS}/window/unified/FocusInfo.h
        ${ALGINE_HEADERS}/window/unified/PointerInfo.h
        ${ALGINE_HEADERS}/window/unified/UnifiedEventHandler.h

        ${ALGINE_HEADERS}/window/Content.h
        ${ALGINE_HEADERS}/window/EventHandler.h
        ${ALGINE_HEADERS}/window/Window.h
)

set(ALGINE_WINDOW_SOURCES
        unified/Event.cpp
        unified/UnifiedEventHandler.cpp

        Content.cpp
        Window.cpp
)

target_sources(algine
        PUBLIC ${ALGINE_WINDOW_HEADERS}
        PRIVATE ${ALGINE_WINDOW_SOURCES}
)