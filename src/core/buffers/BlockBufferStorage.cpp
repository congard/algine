#include <algine/core/buffers/BlockBufferStorage.h>

#include <tulz/macros.h>

using namespace std;
using namespace tulz;

#define allocated_block true
#define free_block false
#define is_block_allocated(index) m_bufferMap[index]
#define is_block_free(index) !m_bufferMap[index]

namespace algine {
BlockBufferStorage::~BlockBufferStorage() {
    deletePtr(m_buffer)
}

void BlockBufferStorage::bind() const {
    m_buffer->bind();
}

void BlockBufferStorage::unbind() const {
    m_buffer->unbind();
}

#define storageSize m_blockSize * m_blocksCount

void BlockBufferStorage::allocateStorage() {
    m_buffer = new Buffer();
    m_buffer->m_target = m_type; // NOTE: set the type directly
    m_buffer->bind(); // do not do unbind() after reallocateStorage()
    reallocateStorage();
}

template<typename T1, typename T2> inline T1 min(T1 t1, T2 t2) {
    return t1 > t2 ? t2 : t1;
}

void BlockBufferStorage::reallocateStorage() {
    if (m_blocksCount != m_bufferMap.size()) {
        auto newBufferMap = Array<bool>(m_blocksCount);
        uint oldBlocksCount = min(m_blocksCount, m_bufferMap.size());

        for (uint i = 0; i < oldBlocksCount; i++)
            newBufferMap[i] = m_bufferMap[i];

        for (uint i = oldBlocksCount; i < m_blocksCount; i++)
            newBufferMap[i] = free_block;

        m_bufferMap = newBufferMap;
    }

    // TODO: add copying of old buffer data
    m_buffer->setData(storageSize, nullptr, m_usage);
}

void BlockBufferStorage::allocateBlock(uint blockIndex) {
    m_bufferMap[blockIndex] = allocated_block;
}

void BlockBufferStorage::freeBlock(uint blockIndex) {
    m_bufferMap[blockIndex] = free_block;
}

uint BlockBufferStorage::allocateBlock() {
    for (uint i = 0; i < m_bufferMap.size(); i++) {
        if (is_block_free(i)) {
            m_bufferMap[i] = allocated_block;
            return i;
        }
    }

    return StorageFull;
}

void BlockBufferStorage::write(uint offset, uint size, const void *data) {
    m_buffer->updateData(offset, size, data);
}

void BlockBufferStorage::write(uint blockIndex, uint offset, uint size, const void *data) {
    m_buffer->updateData(blockIndex * m_blockSize + offset, size, data);
}

inline int getFirstBlockType(Array<bool> &map, int lastBlock, bool type) {
    for (uint i = lastBlock + 1; i < map.size(); i++)
        if (map[i] == type)
            return i;

    return -1;
}

vector<BlockBufferStorage::DefragmentationInfo> BlockBufferStorage::defragmentMap() {
    vector<DefragmentationInfo> infos;

    int freeBlock = -1;
    int allocatedBlock = 0;

    while (true) {
        freeBlock = getFirstBlockType(m_bufferMap, freeBlock, free_block);
        allocatedBlock = getFirstBlockType(m_bufferMap,
                freeBlock > allocatedBlock ? freeBlock : allocatedBlock, allocated_block);

        if (freeBlock != -1 && allocatedBlock != -1) {
            DefragmentationInfo info{};
            info.oldIndex = allocatedBlock;
            info.newIndex = freeBlock;
            infos.emplace_back(info);

            swap(m_bufferMap[freeBlock], m_bufferMap[allocatedBlock]);
        } else {
            return infos;
        }
    }
}

void BlockBufferStorage::setBlockSize(uint size, uint alignment) {
    uint remainder = size % alignment;
    uint integer = size / alignment;
    m_blockSize = alignment * (integer + (remainder == 0 ? 0 : 1));
}

void BlockBufferStorage::setBlocksCount(uint count) {
    m_blocksCount = count;
}

void BlockBufferStorage::setBufferType(uint type) {
    m_type = type;
}

void BlockBufferStorage::setBufferUsage(uint usage) {
    m_usage = usage;
}

uint BlockBufferStorage::capacity() const {
    return storageSize;
}

uint BlockBufferStorage::getBlockSize() const {
    return m_blockSize;
}

uint BlockBufferStorage::getBlocksCount() const {
    return m_blocksCount;
}

uint BlockBufferStorage::getBufferType() const {
    return m_buffer->getType();
}

uint BlockBufferStorage::getBufferUsage() const {
    return m_usage;
}

uint BlockBufferStorage::getBlockOffset(uint index) const {
    return m_blockSize * index;
}

Buffer* BlockBufferStorage::getBuffer() const {
    return m_buffer;
}

vector<uint> BlockBufferStorage::getFreeBlocks() const {
    vector<uint> freeBlocks;

    for (uint i = 0; i < m_bufferMap.size(); i++)
        if (is_block_free(i))
            freeBlocks.emplace_back(i);

    return freeBlocks;
}

vector<uint> BlockBufferStorage::getAllocatedBlocks() const {
    vector<uint> allocatedBlocks;

    for (uint i = 0; i < m_bufferMap.size(); i++)
        if (is_block_allocated(i))
            allocatedBlocks.emplace_back(i);

    return allocatedBlocks;
}

bool BlockBufferStorage::isBlockFree(uint index) const {
    return is_block_free(index);
}

const Array<bool>& BlockBufferStorage::getBufferMap() const {
    return m_bufferMap;
}
}
