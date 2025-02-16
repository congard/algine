set(ALGINE_WIDGETS_HEADERS
        ${ALGINE_HEADERS}/widgets/animation/Animation.h
        ${ALGINE_HEADERS}/widgets/animation/ColorAnimation.h
        ${ALGINE_HEADERS}/widgets/animation/CubicSplineInterpolator.h
        ${ALGINE_HEADERS}/widgets/animation/Interpolator.h
        ${ALGINE_HEADERS}/widgets/animation/LinearInterpolator.h
        ${ALGINE_HEADERS}/widgets/animation/OpacityAnimation.h

        ${ALGINE_HEADERS}/widgets/Alignment.h
        ${ALGINE_HEADERS}/widgets/Container.h
        ${ALGINE_HEADERS}/widgets/Dimen.h
        ${ALGINE_HEADERS}/widgets/ImageWidget.h
        ${ALGINE_HEADERS}/widgets/Label.h
        ${ALGINE_HEADERS}/widgets/Layer.h
        ${ALGINE_HEADERS}/widgets/Layout.h
        ${ALGINE_HEADERS}/widgets/LinearLayout.h
        ${ALGINE_HEADERS}/widgets/RelativeLayout.h
        ${ALGINE_HEADERS}/widgets/Scene.h
        ${ALGINE_HEADERS}/widgets/SizePolicy.h
        ${ALGINE_HEADERS}/widgets/Unit.h
        ${ALGINE_HEADERS}/widgets/Widget.h
        ${ALGINE_HEADERS}/widgets/WidgetDisplayOptions.h
)

set(ALGINE_WIDGETS_SOURCES
        animation/Animation.cpp
        animation/ColorAnimation.cpp
        animation/CubicSplineInterpolator.cpp
        animation/LinearInterpolator.cpp
        animation/OpacityAnimation.cpp

        Alignment.cpp
        Dimen.cpp
        LayoutProperties.h
        TexturePathLoader.h

        Layer.cpp
        Scene.cpp
        Container.cpp
        Layout.cpp
        LinearLayout.cpp
        RelativeLayout.cpp

        Widget.cpp
        ImageWidget.cpp
        Label.cpp
)

target_sources(algine
        PUBLIC ${ALGINE_WIDGETS_HEADERS}
        PRIVATE ${ALGINE_WIDGETS_SOURCES}
)