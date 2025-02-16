set(ALGINE_GPU_HEADERS
        ${ALGINE_HEADERS}/gpu/buffers/ArrayBuffer.h
        ${ALGINE_HEADERS}/gpu/buffers/BlockBufferStorage.h
        ${ALGINE_HEADERS}/gpu/buffers/Buffer.h
        ${ALGINE_HEADERS}/gpu/buffers/BufferReader.h
        ${ALGINE_HEADERS}/gpu/buffers/BufferWriter.h
        ${ALGINE_HEADERS}/gpu/buffers/IndexBuffer.h
        ${ALGINE_HEADERS}/gpu/buffers/UniformBuffer.h

        ${ALGINE_HEADERS}/gpu/context/Context.h
        ${ALGINE_HEADERS}/gpu/context/ContextConfig.h
        ${ALGINE_HEADERS}/gpu/context/ContextObject.h
        ${ALGINE_HEADERS}/gpu/context/ContextPool.h

        ${ALGINE_HEADERS}/gpu/shader/Shader.h
        ${ALGINE_HEADERS}/gpu/shader/ShaderBuilder.h
        ${ALGINE_HEADERS}/gpu/shader/ShaderDefinitionGenerator.h
        ${ALGINE_HEADERS}/gpu/shader/ShaderProgram.h
        ${ALGINE_HEADERS}/gpu/shader/ShaderProgramBuilder.h
        ${ALGINE_HEADERS}/gpu/shader/ShadersInfo.h
        ${ALGINE_HEADERS}/gpu/shader/ShaderTools.h
        ${ALGINE_HEADERS}/gpu/shader/BaseUniformBlock.h
        ${ALGINE_HEADERS}/gpu/shader/UniformBlock.h

        ${ALGINE_HEADERS}/gpu/texture/PixelData.h
        ${ALGINE_HEADERS}/gpu/texture/Texture.h
        ${ALGINE_HEADERS}/gpu/texture/Texture2D.h
        ${ALGINE_HEADERS}/gpu/texture/Texture2DBuilder.h
        ${ALGINE_HEADERS}/gpu/texture/TextureBuilder.h
        ${ALGINE_HEADERS}/gpu/texture/TextureCreateInfo.h
        ${ALGINE_HEADERS}/gpu/texture/TextureCube.h
        ${ALGINE_HEADERS}/gpu/texture/TextureCubeBuilder.h
        ${ALGINE_HEADERS}/gpu/texture/TextureFileInfo.h
        ${ALGINE_HEADERS}/gpu/texture/TextureTools.h

        ${ALGINE_HEADERS}/gpu/DataType.h
        ${ALGINE_HEADERS}/gpu/Framebuffer.h
        ${ALGINE_HEADERS}/gpu/FramebufferBuilder.h
        ${ALGINE_HEADERS}/gpu/gl.h
        ${ALGINE_HEADERS}/gpu/ImageBuilder.h
        ${ALGINE_HEADERS}/gpu/InputAttributeDescription.h
        ${ALGINE_HEADERS}/gpu/InputLayout.h
        ${ALGINE_HEADERS}/gpu/OutputList.h
        ${ALGINE_HEADERS}/gpu/Renderbuffer.h
        ${ALGINE_HEADERS}/gpu/RenderbufferBuilder.h
)

set(ALGINE_GPU_SOURCES
        buffers/ArrayBuffer.cpp
        buffers/BlockBufferStorage.cpp
        buffers/Buffer.cpp
        buffers/BufferReader.cpp
        buffers/BufferWriter.cpp
        buffers/IndexBuffer.cpp
        buffers/UniformBuffer.cpp

        context/Context.cpp
        context/ContextCommon.h
        context/ContextPool.cpp

        shader/Shader.cpp
        shader/ShaderBuilder.cpp
        shader/ShaderDefinitionGenerator.cpp
        shader/ShaderDefinitionGenerator.lua.cpp
        shader/ShaderProgram.cpp
        shader/ShaderProgramBuilder.cpp
        shader/ShaderTools.cpp
        shader/BaseUniformBlock.cpp
        shader/UniformBlock.cpp

        texture/Texture.cpp
        texture/Texture2D.cpp
        texture/Texture2DBuilder.cpp
        texture/TextureBuilder.cpp
        texture/TextureCube.cpp
        texture/TextureCubeBuilder.cpp
        texture/TextureTools.cpp

        Framebuffer.cpp
        FramebufferBuilder.cpp
        FramebufferBuilder.lua.cpp
        ImageBuilder.cpp
        InputAttributeDescription.cpp
        InputLayout.cpp
        IOStreamUtils.h
        OutputList.cpp
        Renderbuffer.cpp
        RenderbufferBuilder.cpp
)

target_sources(algine
        PUBLIC ${ALGINE_GPU_HEADERS}
        PRIVATE ${ALGINE_GPU_SOURCES}
)