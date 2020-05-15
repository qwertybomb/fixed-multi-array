#pragma once
#include <utility>
#include <initializer_list>
template<typename T,size_t Size,size_t ... Sizes>
class fixed_multi_array {
    template<typename ... Args>
    static constexpr auto multply(const Args& ... args) {
        return (args * ...);
    }
    template<typename Arg,typename ... Args>
    static constexpr size_t multply_pair(const Arg& arg, const Args& ... args) {
        if constexpr (sizeof...(args) > 0) {
            size_t remainder = multply_pair(args...);
            return arg.first * remainder;
        }
        return arg.first;
    }
    const size_t size_ = multply(Size,Sizes...);
    T data_[multply(Size,Sizes...)];
    template<typename Index, typename ... Indexs>
    void get_index(size_t& final_index, const Index& index, const Indexs& ... indexs) {
        const size_t& dim_index = sizeof...(Indexs);
        size_t new_index = index.second;       
        if constexpr (sizeof...(Indexs)) {
            new_index *= multply_pair(indexs...);
            get_index(final_index, indexs...);
        }
        final_index += new_index;
    }
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    using iterator = T*;
    using const_iterator = const T*;
    fixed_multi_array() = default;
    fixed_multi_array(const std::initializer_list<T>& list) { std::uninitialized_copy(list.begin(), list.end(), data_); }
    ~fixed_multi_array() = default;
    //Element access
    constexpr reference at(size_type pos) { return data_[pos];}
    constexpr const_reference at(size_type pos) const {return data_[pos];}
    constexpr reference operator[](size_type pos) {return data_[pos];}
    constexpr const_reference operator[](size_type pos) const {return data_[pos];}
    template<typename Index, typename ... Indexs>
    T& operator ()(const Index& index, const Indexs& ... indexs) {
        size_t final_index = 0;
        get_index(final_index, std::pair<size_type, size_type>(Size,index), std::pair<size_type, size_type>(Sizes,indexs)...);
        return data_[final_index];
    }
    constexpr reference front() {return data_[0];}
    constexpr const_reference front() const { return data[0]; }
    constexpr reference back() { return data_[size_-1]; }
    constexpr const_reference back() const { return data[size_-1]; }
    constexpr T* data() noexcept { return data_; }
    constexpr const T* data() const noexcept { return data_; }
    //Iterators
    constexpr iterator begin() noexcept { return data_; }
    constexpr iterator end() noexcept { return data_+size_; }
    constexpr const_iterator begin() const noexcept { return data_; }
    constexpr const_iterator end() const noexcept { return data_ + size_; }
    constexpr iterator cbegin() const noexcept { return data_; }
    constexpr iterator cend() const noexcept { return data_ + size_; }
    //Capacity
    constexpr size_type size() const noexcept {return size_;}
    constexpr bool empty() const noexcept {return begin() == end();}
    constexpr size_type max_size() const noexcept { return std::numeric_limits<size_type>::max() / sizeof(T); }
    //Operations
    constexpr void fill(const T& value) {
        std::fill(data_, data_ + size_, value);
    }
    constexpr void swap(fixed_multi_array& other)  noexcept(noexcept(std::swap(std::declval<value_type&>(), std::declval<value_type&>()))) {
        /*from swap_ranges*/
        iterator first1 = data_;
        iterator first2 = other.data_;
        while (first1 != end()) {
            std::swap(*first1, *first2);
            ++first1; ++first2;
        }
    }
};



