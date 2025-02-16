set(ALGINE_CORE_HEADERS
        ${ALGINE_HEADERS}/core/assert_cast.h
        ${ALGINE_HEADERS}/core/Builder.h
        ${ALGINE_HEADERS}/core/Engine.h
        ${ALGINE_HEADERS}/core/MutableValue.h
        ${ALGINE_HEADERS}/core/Object.h
        ${ALGINE_HEADERS}/core/platform.h
        ${ALGINE_HEADERS}/core/Ptr.h
        ${ALGINE_HEADERS}/core/PtrMaker.h
        ${ALGINE_HEADERS}/core/PtrView.h
        ${ALGINE_HEADERS}/core/Resources.h
        ${ALGINE_HEADERS}/core/TypeRegistry.h
        ${ALGINE_HEADERS}/core/types.h
)

set(ALGINE_CORE_SOURCES
        Builder.cpp
        Engine.cpp
        Object.cpp
        Resources.cpp
        TypeRegistry.cpp
)

target_sources(algine
        PUBLIC ${ALGINE_CORE_HEADERS}
        PRIVATE ${ALGINE_CORE_SOURCES}
)