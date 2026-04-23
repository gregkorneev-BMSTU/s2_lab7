#pragma once

#include <cstddef>
#include <iterator>
#include <new>
#include <type_traits>
#include <utility>

template<typename T>
class MyContainer {
private:
    static constexpr std::size_t kBlockSize = 16;
    using Storage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

    template<typename ValueType>
    class BasicIterator {
        template<typename>
        friend class BasicIterator;
        friend class MyContainer;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename std::remove_const<ValueType>::type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() : container_(nullptr), index_(0) {}

        template<typename OtherValueType,
                 typename = typename std::enable_if<
                     std::is_same<OtherValueType, value_type>::value &&
                     std::is_const<ValueType>::value>::type>
        BasicIterator(const BasicIterator<OtherValueType>& other)
            : container_(other.container_), index_(other.index_) {}

        reference operator*() const {
            return (*container_)[index_];
        }

        pointer operator->() const {
            return &(*container_)[index_];
        }

        BasicIterator& operator++() {
            ++index_;
            return *this;
        }

        BasicIterator operator++(int) {
            BasicIterator temp(*this);
            ++(*this);
            return temp;
        }

        BasicIterator& operator--() {
            --index_;
            return *this;
        }

        BasicIterator operator--(int) {
            BasicIterator temp(*this);
            --(*this);
            return temp;
        }

        BasicIterator& operator+=(difference_type offset) {
            index_ = static_cast<std::size_t>(
                static_cast<difference_type>(index_) + offset);
            return *this;
        }

        BasicIterator& operator-=(difference_type offset) {
            index_ = static_cast<std::size_t>(
                static_cast<difference_type>(index_) - offset);
            return *this;
        }

        BasicIterator operator+(difference_type offset) const {
            BasicIterator temp(*this);
            temp += offset;
            return temp;
        }

        BasicIterator operator-(difference_type offset) const {
            BasicIterator temp(*this);
            temp -= offset;
            return temp;
        }

        difference_type operator-(const BasicIterator& other) const {
            return static_cast<difference_type>(index_) -
                   static_cast<difference_type>(other.index_);
        }

        reference operator[](difference_type offset) const {
            return *(*this + offset);
        }

        bool operator==(const BasicIterator& other) const {
            return container_ == other.container_ && index_ == other.index_;
        }

        bool operator!=(const BasicIterator& other) const {
            return !(*this == other);
        }

        bool operator<(const BasicIterator& other) const {
            return index_ < other.index_;
        }

        bool operator>(const BasicIterator& other) const {
            return other < *this;
        }

        bool operator<=(const BasicIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const BasicIterator& other) const {
            return !(*this < other);
        }

        friend BasicIterator operator+(difference_type offset, const BasicIterator& it) {
            return it + offset;
        }

        std::size_t index() const {
            return index_;
        }

    private:
        using ContainerType = typename std::conditional<
            std::is_const<ValueType>::value,
            const MyContainer,
            MyContainer>::type;

        BasicIterator(ContainerType* container, std::size_t index)
            : container_(container), index_(index) {}

        ContainerType* container_;
        std::size_t index_;
    };

public:
    using Iterator = BasicIterator<T>;
    using ConstIterator = BasicIterator<const T>;

    MyContainer()
        : blocks_(nullptr),
          size_(0),
          capacity_(0),
          blockCount_(0),
          blockCapacity_(0) {}

    MyContainer(const MyContainer& other) : MyContainer() {
        reserve(other.size_);
        for (std::size_t i = 0; i < other.size_; ++i) {
            push_back(other[i]);
        }
    }

    MyContainer(MyContainer&& other) noexcept
        : blocks_(other.blocks_),
          size_(other.size_),
          capacity_(other.capacity_),
          blockCount_(other.blockCount_),
          blockCapacity_(other.blockCapacity_) {
        other.blocks_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        other.blockCount_ = 0;
        other.blockCapacity_ = 0;
    }

    MyContainer& operator=(const MyContainer& other) {
        if (this == &other) {
            return *this;
        }

        clear();
        reserve(other.size_);
        for (std::size_t i = 0; i < other.size_; ++i) {
            push_back(other[i]);
        }
        return *this;
    }

    MyContainer& operator=(MyContainer&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        releaseStorage();

        blocks_ = other.blocks_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        blockCount_ = other.blockCount_;
        blockCapacity_ = other.blockCapacity_;

        other.blocks_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        other.blockCount_ = 0;
        other.blockCapacity_ = 0;
        return *this;
    }

    ~MyContainer() {
        releaseStorage();
    }

    void push_back(const T& value) {
        emplace_back(value);
    }

    void push_back(T&& value) {
        emplace_back(std::move(value));
    }

    template<typename... Args>
    T& emplace_back(Args&&... args) {
        ensureCapacity();
        new (elementPtr(size_)) T(std::forward<Args>(args)...);
        ++size_;
        return (*this)[size_ - 1];
    }

    void pop_back() {
        if (size_ == 0) {
            return;
        }

        elementPtr(size_ - 1)->~T();
        --size_;
    }

    bool removeAt(std::size_t index) {
        if (index >= size_) {
            return false;
        }

        for (std::size_t i = index; i + 1 < size_; ++i) {
            (*this)[i] = std::move((*this)[i + 1]);
        }

        pop_back();
        return true;
    }

    Iterator erase(Iterator position) {
        if (position.container_ != this || position.index() >= size_) {
            return end();
        }

        std::size_t index = position.index();
        removeAt(index);
        return index < size_ ? Iterator(this, index) : end();
    }

    void clear() {
        while (size_ > 0) {
            pop_back();
        }
    }

    void reserve(std::size_t newCapacity) {
        if (newCapacity <= capacity_) {
            return;
        }

        std::size_t requiredBlocks = blocksForCapacity(newCapacity);
        ensureBlockTableCapacity(requiredBlocks);

        while (blockCount_ < requiredBlocks) {
            blocks_[blockCount_] = allocateBlock();
            ++blockCount_;
        }

        capacity_ = blockCount_ * kBlockSize;
    }

    std::size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    T& operator[](std::size_t index) {
        return *elementPtr(index);
    }

    const T& operator[](std::size_t index) const {
        return *elementPtr(index);
    }

    Iterator find(const T& value) {
        for (Iterator it = begin(); it != end(); ++it) {
            if (*it == value) {
                return it;
            }
        }
        return end();
    }

    ConstIterator find(const T& value) const {
        for (ConstIterator it = begin(); it != end(); ++it) {
            if (*it == value) {
                return it;
            }
        }
        return end();
    }

    Iterator begin() {
        return Iterator(this, 0);
    }

    Iterator end() {
        return Iterator(this, size_);
    }

    ConstIterator begin() const {
        return ConstIterator(this, 0);
    }

    ConstIterator end() const {
        return ConstIterator(this, size_);
    }

    ConstIterator cbegin() const {
        return begin();
    }

    ConstIterator cend() const {
        return end();
    }

    void swap(MyContainer& other) noexcept {
        std::swap(blocks_, other.blocks_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(blockCount_, other.blockCount_);
        std::swap(blockCapacity_, other.blockCapacity_);
    }

private:
    static std::size_t blocksForCapacity(std::size_t capacity) {
        return (capacity + kBlockSize - 1) / kBlockSize;
    }

    static Storage* allocateBlock() {
        return static_cast<Storage*>(::operator new(sizeof(Storage) * kBlockSize));
    }

    void ensureCapacity() {
        if (size_ < capacity_) {
            return;
        }

        std::size_t newCapacity = capacity_ == 0 ? kBlockSize : capacity_ * 2;
        reserve(newCapacity);
    }

    void ensureBlockTableCapacity(std::size_t requiredBlocks) {
        if (requiredBlocks <= blockCapacity_) {
            return;
        }

        std::size_t newBlockCapacity = blockCapacity_ == 0 ? 4 : blockCapacity_;
        while (newBlockCapacity < requiredBlocks) {
            newBlockCapacity *= 2;
        }

        Storage** newBlocks = new Storage*[newBlockCapacity];
        for (std::size_t i = 0; i < newBlockCapacity; ++i) {
            newBlocks[i] = nullptr;
        }

        for (std::size_t i = 0; i < blockCount_; ++i) {
            newBlocks[i] = blocks_[i];
        }

        delete[] blocks_;
        blocks_ = newBlocks;
        blockCapacity_ = newBlockCapacity;
    }

    T* elementPtr(std::size_t index) {
        return const_cast<T*>(static_cast<const MyContainer*>(this)->elementPtr(index));
    }

    const T* elementPtr(std::size_t index) const {
        std::size_t blockIndex = index / kBlockSize;
        std::size_t offset = index % kBlockSize;
        return reinterpret_cast<const T*>(blocks_[blockIndex] + offset);
    }

    void releaseStorage() noexcept {
        clear();

        for (std::size_t i = 0; i < blockCount_; ++i) {
            ::operator delete(blocks_[i]);
        }

        delete[] blocks_;
        blocks_ = nullptr;
        capacity_ = 0;
        blockCount_ = 0;
        blockCapacity_ = 0;
    }

    Storage** blocks_;
    std::size_t size_;
    std::size_t capacity_;
    std::size_t blockCount_;
    std::size_t blockCapacity_;
};
