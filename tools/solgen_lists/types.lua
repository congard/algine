--- The list of algine types that support the generation of Lua bindings
--- Note: this list may be incomplete

local types = {
    -- core
    ["Object"] = {
        file = "include/algine/core/Object.h"
    },
    ["Scriptable"] = {
        file = "include/algine/lua/Scriptable.h"
    },
    ["Font"] = {
        file = "include/common/algine/core/font/Font.h"
    },
    ["FontLibrary"] = {
        file = "include/common/algine/core/font/FontLibrary.h"
    },
    ["FontMetrics"] = {
        file = "include/common/algine/core/font/FontMetrics.h"
    },
    ["FontRenderer"] = {
        file = "include/common/algine/core/font/FontRenderer.h"
    },
    ["IOProvider"] = {
        file = "include/algine/io/IOProvider.h"
    },
    ["Log"] = {
        file = "include/algine/log/Log.h"
    },
    ["Shader"] = {
        file = "include/algine/gpu/shader/Shader.h"
    },
    ["ShaderBuilder"] = {
        file = "include/algine/gpu/shader/ShaderBuilder.h"
    },
    ["ShaderDefinitionGenerator"] = {
        file = "include/algine/gpu/shader/ShaderDefinitionGenerator.h"
    },
    ["ShaderProgram"] = {
        file = "include/algine/gpu/shader/ShaderProgram.h"
    },
    ["ShaderProgramBuilder"] = {
        file = "include/algine/gpu/shader/ShaderProgramBuilder.h"
    },
    ["Color"] = {
        file = "include/common/algine/core/Color.h"
    },
    ["ImageBuilder"] = {
        file = "include/algine/gpu/ImageBuilder.h"
    },
    ["Texture"] = {
        file = "include/algine/gpu/texture/Texture.h"
    },
    ["Texture2D"] = {
        file = "include/algine/gpu/texture/Texture2D.h"
    },
    ["Texture2DBuilder"] = {
        file = "include/algine/gpu/texture/Texture2DBuilder.h"
    },
    ["TextureCreateInfo"] = {
        file = "include/algine/gpu/texture/TextureCreateInfo.h"
    },
    ["TextureBuilder"] = {
        file = "include/algine/gpu/texture/TextureBuilder.h"
    },
    ["TextureCube"] = {
        file = "include/algine/gpu/texture/TextureCube.h"
    },
    ["TextureCubeBuilder"] = {
        file = "include/algine/gpu/texture/TextureCubeBuilder.h"
    },
    ["TextureFileInfo"] = {
        file = "include/algine/gpu/texture/TextureFileInfo.h"
    },
    ["Framebuffer"] = {
        file = "include/algine/gpu/Framebuffer.h"
    },
    ["FramebufferBuilder"] = {
        file = "include/algine/gpu/FramebufferBuilder.h"
    },
    ["Renderbuffer"] = {
        file = "include/algine/gpu/Renderbuffer.h"
    },
    ["RenderbufferBuilder"] = {
        file = "include/algine/gpu/RenderbufferBuilder.h"
    },
    ["Builder"] = {
        file = "include/algine/core/Builder.h"
    },
    ["DataType"] = {
        file = "include/common/algine/core/DataType.h"
    },
    ["OutputList"] = {
        file = "include/common/algine/core/OutputList.h"
    },
    ["Paint"] = {
        file = "include/common/algine/core/painter/Paint.h"
    },
    ["Painter"] = {
        file = "include/common/algine/core/painter/Painter.h"
    },
    ["RoundRect"] = {
        file = "include/common/algine/core/painter/RoundRect.h"
    },
    ["Widget"] = {
        file = "include/algine/widgets/Widget.h"
    },
    ["Dimen"] = {
        file = "include/algine/widgets/Dimen.h"
    },
    ["Unit"] = {
        file = "include/algine/widgets/Unit.h"
    },
    ["RectI"] = {
        file = "include/common/algine/core/math/Rect.h"
    },
    ["RectF"] = {
        file = "include/common/algine/core/math/Rect.h"
    },
    ["PointI"] = {
        file = "include/common/algine/core/math/Point.h"
    },
    ["PointF"] = {
        file = "include/common/algine/core/math/Point.h"
    },
    ["Scene"] = {
        file = "include/common/algine/core/scene/Scene.h"
    },
    ["GlobalScene"] = {
        file = "include/common/algine/core/scene/GlobalScene.h"
    },

    -- std
    ["ShapeBuilder"] = {
        file = "include/common/algine/std/model/ShapeBuilder.h"
    },
    ["Shape"] = {
        file = "include/common/algine/std/model/Shape.h"
    },
    ["ModelBuilder"] = {
        file = "include/common/algine/std/model/ModelBuilder.h"
    },
    ["Model"] = {
        file = "include/common/algine/std/model/Model.h"
    },
    ["InputLayoutShapeLocations"] = {
        file = "include/common/algine/std/model/InputLayoutShapeLocations.h"
    },
    ["Rotator"] = {
        file = "include/common/algine/std/rotator/Rotator.h"
    },
    ["EulerRotator"] = {
        file = "include/common/algine/std/rotator/EulerRotator.h"
    },
    ["FreeRotator"] = {
        file = "include/common/algine/std/rotator/FreeRotator.h"
    },
    ["Rotatable"] = {
        file = "include/common/algine/std/Rotatable.h"
    },
    ["Scalable"] = {
        file = "include/common/algine/std/Scalable.h"
    },
    ["Translatable"] = {
        file = "include/common/algine/std/Translatable.h"
    },
    ["AMTLManager"] = {
        file = "include/common/algine/std/AMTLManager.h"
    },
    ["AMTLMaterialManager"] = {
        file = "include/common/algine/std/AMTLMaterialManager.h"
    }
}

-- List of minimum required types
-- These types are mostly needed by classes
-- such as `core/lua/Lua` and `std/AMTLManager`
local minimumRequiredTypes = {
    "IOProvider",
    "Object",
    "Texture",
    "Texture2D",
    "TextureBuilder",
    "Texture2DBuilder",
    "ImageBuilder",
    "Builder",
    "Scriptable"
}

return {
    types = types,
    minimumRequiredTypes = minimumRequiredTypes
}