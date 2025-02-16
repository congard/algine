set(ALGINE_MATH_HEADERS
        ${ALGINE_HEADERS}/math/BezierCurve.h
        ${ALGINE_HEADERS}/math/CubicSpline.h
        ${ALGINE_HEADERS}/math/PCHIPInterpolator.h

        ${ALGINE_HEADERS}/math/Point.h
        ${ALGINE_HEADERS}/math/Rect.h
        ${ALGINE_HEADERS}/math/Size.h

        ${ALGINE_HEADERS}/math/Color.h
)

set(ALGINE_MATH_SOURCES
        CubicSpline.cpp
        PCHIPInterpolator.cpp

        Point.lua.cpp
        Rect.lua.cpp

        Color.cpp
        color/hsluv.c color/hsluv.h
        color/hsv.cpp color/hsv.h
)

target_sources(algine
        PUBLIC ${ALGINE_MATH_HEADERS}
        PRIVATE ${ALGINE_MATH_SOURCES}
)