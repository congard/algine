#ifndef ALGINE_BLOCKBUFFERSTORAGE_H
#define ALGINE_BLOCKBUFFERSTORAGE_H

#include <algine/core/buffers/Buffer.h>
#include <algine/types.h>

#include <vector>
#include <tulz/Array.h>

namespace algine {
class BlockBufferStorage {
public:
    constexpr static uint StorageFull = -1;

public:
    struct DefragmentationInfo {
        uint oldIndex;
        uint newIndex;
    };

public:
    ~BlockBufferStorage();

    void bind() const;
    void unbind() const;

    void allocateStorage(Object *bufferParent);
    void reallocateStorage();
    void allocateBlock(uint blockIndex);
    void freeBlock(uint blockIndex);
    uint allocateBlock();

    void write(uint offset, uint size, const void *data);
    void write(uint blockIndex, uint offset, uint size, const void *data);
    std::vector<DefragmentationInfo> defragmentMap();

    void setBlockSize(uint size, uint alignment = 1);
    void setBlocksCount(uint count);
    void setBufferType(uint type);
    void setBufferUsage(uint usage);

    uint capacity() const;
    uint getBlockSize() const;
    uint getBlocksCount() const;
    uint getBufferType() const;
    uint getBufferUsage() const;
    uint getBlockOffset(uint index) const;
    Buffer* getBuffer() const;
    std::vector<uint> getFreeBlocks() const;
    std::vector<uint> getAllocatedBlocks() const;
    bool isBlockFree(uint index) const;
    const tulz::Array<bool>& getBufferMap() const;

private:
    tulz::Array<bool> m_bufferMap;
    Buffer *m_buffer = nullptr;
    uint m_blockSize = 0, m_blocksCount = 0;
    uint m_type = 0, m_usage = 0;
};
}

#endif //ALGINE_BLOCKBUFFERSTORAGE_H
