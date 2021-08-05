#include <algorithm>
#include <cstddef>
#include <memory>
#include <new>
#include <utility>

template <typename T>
class RawMemory
{
public:
    RawMemory() = default;
    RawMemory(const RawMemory&) = delete;

    RawMemory(size_t n)
    {
        memory_ = allocate(n);
        capacity_ = n;
    }

    RawMemory(RawMemory&& other) noexcept
    {
        swap(other);
    }

    void swap(RawMemory& other) noexcept
    {
        std::swap(memory_, other.memory_);
        std::swap(capacity_, other.capacity_);
    }

    RawMemory& operator=(const RawMemory&) = delete;
    RawMemory& operator=(RawMemory&& other) noexcept
    {
        swap(other);
        return *this;
    }

    const T& operator[](size_t i) const
    {
        return memory_[i];
    }
    T& operator[](size_t i)
    {
        return memory_[i];
    }

    const T* begin() const noexcept
    {
        return memory_;
    }
    T* begin() noexcept
    {
        return memory_;
    }
    size_t capacity() const noexcept
    {
        return capacity_;
    }

    ~RawMemory()
    {
        deallocate(memory_);
    }

private:
    static T* allocate(size_t n)
    {
        return static_cast<T*>(operator new(n * sizeof(T)));
    }
    static void deallocate(T* buf)
    {
        operator delete(buf);
    }

private:
    T* memory_ = nullptr;
    size_t capacity_ = 0;
};

template <typename T>
class Vector
{
public:
    Vector() = default;

    Vector(size_t n)
        : data_(n)
    {
        std::uninitialized_value_construct_n(data_.begin(), n);
        size_ = n;
    }

    Vector(const Vector& other)
        : data_(other.size_)
    {
        std::uninitialized_copy_n(other.data_.begin(), other.size_, data_.begin());
        size_ = other.size_;
    }

    Vector(Vector&& other) noexcept
    {
        swap(other);
    }

    Vector& operator=(const Vector& other)
    {
        if (other.size_ > data_.capacity())
        {
            Vector tmp(other);
            swap(tmp);
        }
        else
        {
            for (size_t i = 0; i < size_ && i < other.size_; ++i)
            {
                data_[i] = other.data_[i];
            }
            if (size_ < other.size_)
            {
                std::uninitialized_copy_n(
                    other.data_.begin() + size_, other.size_ - size_, data_.begin() + size_);
            }
            else if (size_ > other.size_)
            {
                std::destroy_n(data_.begin() + other.size_, size_ - other.size_);
            }
            size_ = other.size_;
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept
    {
        swap(other);
        return *this;
    }

    void swap(Vector& other) noexcept
    {
        data_.swap(other.data_);
        std::swap(size_, other.size_);
    }

    void reserve(size_t n)
    {
        if (n <= data_.capacity())
        {
            return;
        }

        RawMemory<T> newData(n);
        std::uninitialized_move_n(data_.begin(), size_, newData.begin());
        std::destroy_n(data_.begin(), size_);
        data_ = std::move(newData);
    }

    void resize(size_t n)
    {
        reserve(n);
        if (size_ < n)
        {
            std::uninitialized_value_construct_n(data_.begin() + size_, n - size_);
        }
        else if (size_ > n)
        {
            std::destroy_n(data_.begin() + n, size_ - n);
        }
        size_ = n;
    }

    void pushBack(const T& element)
    {
        if (size_ == data_.capacity())
        {
            reserve(size_ == 0 ? 1 : size_ * 2);
        }
        new (data_.begin() + size_) T(element);
        ++size_;
    }

    void pushBack(T&& element)
    {
        if (size_ == data_.capacity())
        {
            reserve(size_ == 0 ? 1 : size_ * 2);
        }
        new (data_.begin() + size_) T(std::move(element));
        ++size_;
    }

    template <typename... Args>
    T& emplaceBack(Args&&... args)
    {
        if (size_ == data_.capacity())
        {
            reserve(size_ == 0 ? 1 : size_ * 2);
        }
        auto element = new (data_.begin() + size_) T(std::forward<Args>(args)...);
        ++size_;
        return *element;
    }

    void popBack()
    {
        std::destroy_at(data_.begin() + size_ - 1);
        --size_;
    }

    using iterator = T*;
    using const_iterator = const T*;

    [[nodiscard]] iterator begin() noexcept
    {
        return data_.begin();
    }
    [[nodiscard]] iterator end() noexcept
    {
        return data_.begin() + size_;
    }
    [[nodiscard]] const_iterator begin() const noexcept
    {
        return data_.begin();
    }
    [[nodiscard]] const_iterator end() const noexcept
    {
        return data_.begin() + size_;
    }
    [[nodiscard]] const_iterator cbegin() const noexcept
    {
        return data_.begin();
    }
    [[nodiscard]] const_iterator cend() const noexcept
    {
        return data_.begin() + size_;
    }

    iterator insert(const_iterator pos, const T& element)
    {
        size_t i = pos - begin();
        pushBack(element);
        std::rotate(data_.begin() + i, data_.begin() + size_ - 1, data_.begin() + size_);
        return begin() + i;
    }
    iterator insert(const_iterator pos, T&& element)
    {
        size_t i = static_cast<size_t>(pos - begin());
        pushBack(std::move(element));
        std::rotate(data_.begin() + i, data_.begin() + size_ - 1, data_.begin() + size_);
        return begin() + i;
    }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        size_t i = static_cast<size_t>(pos - cbegin());
        emplaceBack(std::forward<Args>(args)...);
        std::rotate(data_.begin() + i, data_.begin() + size_ - 1, data_.begin() + size_);
        return begin() + i;
    }

    iterator erase(const_iterator pos)
    {
        size_t i = static_cast<size_t>(pos - cbegin());
        if (i + 1 < size_)
        {
            std::move(data_.begin() + i + 1, data_.begin() + size_, data_.begin() + i);
        }
        popBack();
        return begin() + i;
    }

    [[nodiscard]] size_t size() const noexcept
    {
        return size_;
    }
    [[nodiscard]] size_t capacity() const noexcept
    {
        return data_.capacity();
    }

    [[nodiscard]] const T& operator[](size_t i) const
    {
        return data_[i];
    }
    [[nodiscard]] T& operator[](size_t i)
    {
        return data_[i];
    }

    ~Vector()
    {
        std::destroy_n(data_.begin(), size_);
    }

private:
    RawMemory<T> data_;
    size_t size_ = 0;
};
