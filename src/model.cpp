/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 */

#ifndef ALGINE_MODEL_CPP
#define ALGINE_MODEL_CPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#include "lib/libcmf/arraysmaker.cpp"
#include "texture.cpp"

namespace algine {
#define XYZ 3
#define XY 2
#define TRIANGLE 3

class ModelBuffers {
    public:
        GLuint buffers[5];
        PolygonArrayHelper polygons;

        void init(CMF2Arrays &arrays) {
            glGenBuffers(5, buffers);
            glBindBuffer(GL_ARRAY_BUFFER, buffers[0]); // vertices
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arrays.outVertices.size, arrays.outVertices.array, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, buffers[1]); // normals
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arrays.outNormals.size, arrays.outNormals.array, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, buffers[2]); // tex coords
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arrays.outTexCoords.size, arrays.outTexCoords.array, GL_STATIC_DRAW);

            polygons.array = new Polygon[arrays.polygons.size];
            polygons.size = arrays.polygons.size;
            for (size_t i = 0; i < arrays.polygons.size; i++) polygons.array[i] = arrays.polygons.array[i];

            /* --- making tangent and bitangent buffers --- */
            float tmp;
            float *tangents = new float[arrays.outVertices.size], *bitangents = new float[arrays.outVertices.size];
            glm::vec3 tmpv;
            for (size_t i = 0; i < arrays.outVertices.size / XYZ; i += TRIANGLE) {
                /* --- calculating edge and UV vectors --- */

                glm::vec3 edge1 = 
                    + glm::vec3(arrays.outVertices.array[i * XYZ + 3], arrays.outVertices.array[i * XYZ + 4], arrays.outVertices.array[i * XYZ + 5])
                    - glm::vec3(arrays.outVertices.array[i * XYZ + 0], arrays.outVertices.array[i * XYZ + 1], arrays.outVertices.array[i * XYZ + 2]);
                glm::vec3 edge2 = 
                    + glm::vec3(arrays.outVertices.array[i * XYZ + 6], arrays.outVertices.array[i * XYZ + 7], arrays.outVertices.array[i * XYZ + 8])
                    - glm::vec3(arrays.outVertices.array[i * XYZ + 0], arrays.outVertices.array[i * XYZ + 1], arrays.outVertices.array[i * XYZ + 2]);
                glm::vec2 deltaUV1 = 
                    + glm::vec2(arrays.outTexCoords.array[i * XY + 2], arrays.outTexCoords.array[i * XY + 3])
                    - glm::vec2(arrays.outTexCoords.array[i * XY + 0], arrays.outTexCoords.array[i * XY + 1]);
                glm::vec2 deltaUV2 = 
                    + glm::vec2(arrays.outTexCoords.array[i * XY + 4], arrays.outTexCoords.array[i * XY + 5])
                    - glm::vec2(arrays.outTexCoords.array[i * XY + 0], arrays.outTexCoords.array[i * XY + 1]);
                
                /*
                 * calculating tangents and bitangents for triangle vertices
                 * one triangle - 3 tangents and bitangents
                 */

                tmp = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                // tangents
                tmpv = glm::vec3(
                    tmp * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                    tmp * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                    tmp * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
                );
                glm::normalize(tmpv);
                for (size_t j = 0; j < TRIANGLE; j++) {
                    tangents[i * XYZ + 0 + j * XYZ] = tmpv.x;
                    tangents[i * XYZ + 1 + j * XYZ] = tmpv.y;
                    tangents[i * XYZ + 2 + j * XYZ] = tmpv.z;
                }

                // bitangents
                tmpv = glm::vec3(
                    tmp * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
                    tmp * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
                    tmp * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
                );
                glm::normalize(tmpv);
                for (size_t j = 0; j < TRIANGLE; j++) {
                    bitangents[i * XYZ + 0 + j * XYZ] = tmpv.x;
                    bitangents[i * XYZ + 1 + j * XYZ] = tmpv.y;
                    bitangents[i * XYZ + 2 + j * XYZ] = tmpv.z;
                }
            }

            glBindBuffer(GL_ARRAY_BUFFER, buffers[3]); // tangents
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arrays.outVertices.size, tangents, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, buffers[4]); // bitangents
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arrays.outVertices.size, bitangents, GL_STATIC_DRAW);

            delete[] tangents;
            delete[] bitangents;
        }

        void recycle() {
            glDeleteBuffers(5, buffers);
            delete[] polygons.array;
            #ifdef ALGINE_LOGGING
            std::cout << "ModelBuffers::recycle()\n";
            #endif
        }
};

class ModelMapping {
    public:
        GLuint textures[6];

        void init(const char *atex, const char *dtex, const char *stex, const char *ntex, const char *rstex, const char *jtex) {
            textures[0] = atex != nullptr ? loadTexture(atex) : 0;
            textures[1] = dtex != nullptr ? loadTexture(dtex) : 0;
            textures[2] = stex != nullptr ? loadTexture(stex) : 0;
            textures[3] = ntex != nullptr ? loadTexture(ntex) : 0;
            textures[4] = rstex != nullptr ? loadTexture(rstex) : 0;
            textures[5] = jtex != nullptr ? loadTexture(jtex) : 0;
        }

        void recycle() {
            glDeleteTextures(6, textures);
        }
};

class Model {
    public:
        ModelBuffers *buffers;
        ModelMapping *mapping;
        glm::vec3 origin, angles;
        
        /* --- constructors, operators, destructor --- */

        Model(const Model &src) {
            buffers = src.buffers;
            mapping = src.mapping;
            angles = src.angles;
            origin = src.origin;
            #ifdef ALGINE_LOGGING
            std::cout << "Model copy constructor\n";
            #endif
        }

        Model(Model &&src) {
            src.swap(*this);
            #ifdef ALGINE_LOGGING
            std::cout << "Model move constructor\n";
            #endif
        }

        Model& operator = (const Model &rhs) {
            if (&rhs != this) Model(rhs).swap(*this);
            #ifdef ALGINE_LOGGING
            std::cout << "Model copy operator =\n";
            #endif
            return *this;
        }

        Model& operator = (Model &&rhs) {
            rhs.swap(*this);
            #ifdef ALGINE_LOGGING
            std::cout << "Model move operator =\n";
            #endif
            return *this;
        }

        Model(ModelBuffers *buffers, ModelMapping *mapping) {
            this->buffers = buffers;
            this->mapping = mapping;
        }

        Model() { /* for arrays constructor */ }

        ~Model() {
            #ifdef ALGINE_LOGGING
            std::cout << "~Model()\n";
            #endif
        }

        /* --- --- */

        void swap(Model &other) {
            std::swap(buffers, other.buffers);
            std::swap(mapping, other.mapping);
            std::swap(origin, other.origin);
            std::swap(angles, other.angles);
        }

        inline GLuint getVerticesBuffer() {
            return buffers->buffers[0];
        }

        inline GLuint getNormalsBuffer() {
            return buffers->buffers[1];
        }

        inline GLuint getTexCoordsBuffer() {
            return buffers->buffers[2];
        }

        inline GLuint getTangentsBuffer() {
            return buffers->buffers[3];
        }

        inline GLuint getBitangentsBuffer() {
            return buffers->buffers[4];
        }

        static void inverse(FloatArrayHelper &fah) {
            for (size_t i = 0; i < fah.size; i++) fah.array[i] *= -1;
        }
};

#undef XYZ
#undef XY
#undef TRIANGLE

} // namespace algine

#endif