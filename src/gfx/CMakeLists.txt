set(ALGINE_GFX_HEADERS
        ${ALGINE_HEADERS}/gfx/font/Font.h
        ${ALGINE_HEADERS}/gfx/font/FontLibrary.h
        ${ALGINE_HEADERS}/gfx/font/FontMetrics.h
        ${ALGINE_HEADERS}/gfx/font/FontRenderer.h

        ${ALGINE_HEADERS}/gfx/painter/Paint.h
        ${ALGINE_HEADERS}/gfx/painter/Painter.h
        ${ALGINE_HEADERS}/gfx/painter/RoundRect.h
)

set(ALGINE_GFX_SOURCES
        font/Font.cpp
        font/FontLibrary.cpp
        font/FontMetrics.cpp
        font/FontRenderer.cpp

        painter/Paint.cpp
        painter/Painter.cpp
)

target_sources(algine
        PUBLIC ${ALGINE_GFX_HEADERS}
        PRIVATE ${ALGINE_GFX_SOURCES}
)