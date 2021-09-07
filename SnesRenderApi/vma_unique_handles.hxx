#pragma once

#include <vk_mem_alloc.hpp>

namespace vma {
class NoDispatch;
}

namespace vk {
template <>
class UniqueHandleTraits<vma::Allocator, vma::NoDispatch> {
public:
    class deleter {
    protected:
        void destroy(vma::Allocator allocator) VULKAN_HPP_NOEXCEPT {
            allocator.destroy();
        };
    };
};

template <>
class UniqueHandleTraits<vma::Allocation, vma::NoDispatch> {
public:
    class deleter {
        deleter(vma::Allocator allocator) : allocator_(allocator) {}

    protected:
        void destroy(vma::Allocation allocation) VULKAN_HPP_NOEXCEPT {
            allocator_.freeMemory(allocation);
        };

    private:
        vma::Allocator allocator_;
    };
};

template <>
class UniqueHandleTraits<vk::Buffer, vma::NoDispatch> {
public:
    class deleter {
    public:
        deleter() = default;
        deleter(vma::Allocator allocator, vma::Allocation allocation)
                : allocator_(allocator), allocation_(allocation) {}

        vma::Allocator allocator() const { return allocator_; }
        vma::Allocation allocation() const { return allocation_; }

        void* mapMemory() const { return allocator_.mapMemory(allocation_); }
        void unmapMemory() const { return allocator_.unmapMemory(allocation_); }

    protected:
        void destroy(vk::Buffer buffer) VULKAN_HPP_NOEXCEPT {
            allocator_.destroyBuffer(buffer, allocation_);
        };

    private:
        vma::Allocator allocator_;
        vma::Allocation allocation_;
    };
};

template <>
class UniqueHandleTraits<vk::Image, vma::NoDispatch> {
public:
    class deleter {
        deleter(vma::Allocator allocator, vma::Allocation allocation)
                : allocator_(allocator), allocation_(allocation) {}

    protected:
        void destroy(vk::Image image) VULKAN_HPP_NOEXCEPT {
            allocator_.destroyImage(image, allocation_);
        };

    private:
        vma::Allocator allocator_;
        vma::Allocation allocation_;
    };
};

template <>
class UniqueHandleTraits<vma::Pool, vma::NoDispatch> {
public:
    class deleter {
        deleter(vma::Allocator allocator) : allocator_(allocator) {}

    protected:
        void destroy(vma::Pool pool) VULKAN_HPP_NOEXCEPT {
            allocator_.destroyPool(pool);
        };

    private:
        vma::Allocator allocator_;
    };
};
}  // namespace vk

namespace vma {
using UniqueAllocator = vk::UniqueHandle<Allocator, NoDispatch>;
using UniqueAllocation = vk::UniqueHandle<Allocation, NoDispatch>;
using UniqueBuffer = vk::UniqueHandle<vk::Buffer, NoDispatch>;
using UniqueImage = vk::UniqueHandle<vk::Image, NoDispatch>;
using UniquePool = vk::UniqueHandle<Pool, NoDispatch>;
}  // namespace vma