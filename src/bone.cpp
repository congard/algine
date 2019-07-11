#include <algine/bone.h>
#include <GL/glew.h>

namespace algine {
// struct BoneInfo
BoneInfo::BoneInfo(const uint &size) {
    ids = new GLuint[size];
    weights = new float[size];
    this->size = size;
    fill();
}

void BoneInfo::add(const uint &id, const float &weight) {
    for (size_t i = 0 ; i < size; i++) {
        if (weights[i] == 0.0) {
            ids[i] = id;
            weights[i] = weight;
            return;
        }
    }
}

// copy constructor
BoneInfo::BoneInfo(const BoneInfo &src) {
    ids = new GLuint[src.size];
    weights = new float[src.size];
    size = src.size;
	std::copy(src.ids, src.ids + src.size, ids);
    std::copy(src.weights, src.weights + src.size, weights);
}

// move constructor
BoneInfo::BoneInfo(BoneInfo &&src) {
    std::swap(ids, src.ids);
    std::swap(weights, src.weights);
    std::swap(size, src.size);
}

BoneInfo::~BoneInfo() {
    delete[] ids;
    delete[] weights;
}

void BoneInfo::fill() {
    for (uint i = 0; i < size; i++) {
        ids[i] = 0;
        weights[i] = 0;
    }
}

// struct Bone
Bone::Bone(const std::string &name, const glm::mat4 &offsetMatrix) {
    this->name = name;
    this->offsetMatrix = offsetMatrix;
}

} /* namespace algine */