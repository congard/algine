#ifndef ALGINE_BONE_CPP
#define ALGINE_BONE_CPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

namespace algine {
struct BoneInfo {
    GLuint *ids = nullptr, size;
    float *weights = nullptr;

    BoneInfo(GLuint size) {
        ids = new GLuint[size];
        weights = new float[size];
        this->size = size;
        fill();
    }

    void add(GLuint id, float weight) {
        for (size_t i = 0 ; i < size; i++) {
            if (weights[i] == 0.0) {
                ids[i] = id;
                weights[i] = weight;
                return;
            }
        }
    }

    // copy constructor
    BoneInfo(const BoneInfo &src) {
        ids = new GLuint[src.size];
        weights = new float[src.size];
        size = src.size;
		std::copy(src.ids, src.ids + src.size, ids);
        std::copy(src.weights, src.weights + src.size, weights);
	}

    // move constructor
    BoneInfo(BoneInfo &&src) {
        std::swap(ids, src.ids);
        std::swap(weights, src.weights);
        std::swap(size, src.size);
    }

    ~BoneInfo() {
        delete[] ids;
        delete[] weights;
    }

    private:
    void fill() {
        for (GLuint i = 0; i < size; i++) {
            ids[i] = 0;
            weights[i] = 0;
        }
    }
};

struct Bone {
    std::string name;
    glm::mat4 offsetMatrix, finalTransformation;

    Bone(std::string name, glm::mat4 offsetMatrix) {
        this->name = name;
        this->offsetMatrix = offsetMatrix;
    }
};

} /* namespace algine */

#endif /* ALGINE_BONE_CPP */