#pragma once

// External APIs
#include <vulkan/vulkan.hpp>

// Standard library
#include <utility>
#include <vector>

namespace VULKAN_HPP_NAMESPACE {
template <typename Type, typename Dispatch>
class UniqueHandles : public vk::UniqueHandleTraits<Type, Dispatch>::deleter {
private:
    using Deleter = typename vk::UniqueHandleTraits<Type, Dispatch>::deleter;

public:
    using element_type = Type;
    using iterator = typename std::vector<Type>::iterator;
    using const_iterator = typename std::vector<Type>::const_iterator;

    UniqueHandles(size_t size = 0) : Deleter(), handles_(size) {}

    UniqueHandles(const UniqueHandles&) = delete;

    UniqueHandles(UniqueHandles&& rhs) VULKAN_HPP_NOEXCEPT
            : Deleter(std::move(static_cast<Deleter&>(rhs))),
              handles_(std::move(rhs.handles_)) {}

    ~UniqueHandles() { clear(); }

    UniqueHandles& operator=(const UniqueHandles&) = delete;

    UniqueHandles& operator=(UniqueHandles&& rhs) VULKAN_HPP_NOEXCEPT {
        static_cast<Deleter&>(*this) = std::move(static_cast<Deleter&>(rhs));
        handles_ = std::move(rhs.handles_);
    }

    void swap(UniqueHandles& rhs) VULKAN_HPP_NOEXCEPT {
        std::swap(static_cast<Deleter&>(*this), static_cast<Deleter&>(rhs));
        std::swap(handles_, rhs.handles_);
    }

    const Type& front() const { return handles_.front(); }
    const Type& back() const { return handles_.back(); }
    const Type& operator[](size_t index) const { return handles_[index]; }

    const Type* data() const { return handles_.data(); }

    bool empty() const { return handles_.empty(); }
    size_t size() const { return handles_.size(); }

    const_iterator begin() const { return handles_.begin(); }
    const_iterator cbegin() const { return handles_.cbegin(); }

    const_iterator rbegin() const { return handles_.rbegin(); }
    const_iterator crbegin() const { return handles_.crbegin(); }

    const_iterator end() const { return handles_.end(); }
    const_iterator cend() const { return handles_.cend(); }

    const_iterator rend() const { return handles_.rend(); }
    const_iterator crend() const { return handles_.crend(); }

    void clear() {
        for (Type& handle : handles_) {
            if (handle) {
                this->destroy(handle);
            }
        }
        handles_.clear();
    }

    void push_back(const Type& value) { handles_.push_back(value); }
    void push_back(Type&& value) { handles_.push_back(std::exchange(value, 0)); }

    void pop_back() {
        Type& handle = handles_.back();
        if (handle) {
            this->destroy(handle);
        }

        handles_.pop_back();
    }

private:
    std::vector<Type> handles_;
};

using UniqueFences = UniqueHandles<Fence, DispatchLoaderStatic>;
}  // namespace VULKAN_HPP_NAMESPACE