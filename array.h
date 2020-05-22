#pragma once
#include <cstddef>
#include <utility>
#include <initializer_list>
#include <limits>
#include <cstring>
#include <iostream>
namespace turtle {
    template<typename T, size_t Size, size_t ... Sizes>
#if __cplusplus > 201709L
    requires (Size != 0) && ((Sizes != 0) && ... && true) //prevent any size parameters from being zero
#endif
        class array {
        protected:
            template<typename ... Args>
            static constexpr auto multply(const Args& ... args) { return (args * ...); }
            constexpr static size_t size_ = multply(Size, Sizes...);
            //friend class turtle::array<T,Size>;
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
            template<typename... Args>
            array(const Args&... args) {
                array_copy(sizeof...(args)-1,args...);
            }
            array() = default;
            ~array() = default;
            //Element access
            constexpr reference at(size_type pos) { return data_[pos]; }
            constexpr const_reference at(size_type pos) const { return data_[pos]; }
            constexpr decltype(auto) operator[](size_type pos) {
                if constexpr (sizeof...(Sizes)) {
                    auto temp = array<T, Sizes...>();
                    temp.data_ = data_ + multply(pos, Sizes...);
                    return temp;
                }
                else {
                    return data_at(pos);
                }
            }
            constexpr const auto& operator[](size_type pos) const { return (*this)[pos]; }
            template<typename Index, typename ... Indices>
            constexpr reference operator ()(const Index& index, const Indices& ... indices) {
                return data_[this->index(index, indices...)];
            }
            template<typename Index, typename ... Indices>
            constexpr const_reference operator()(const Index& index, const Indices& ... indices) const {
                return (*this)(index,indices...);
            }
            template<typename Index, typename ... Indices>
            constexpr size_type index(const Index& index, const Indices& ... indices) {
                static_assert (sizeof...(Indices) >= sizeof...(Sizes), "not enough parameters");
                static_assert (sizeof...(Indices) <= sizeof...(Sizes), "too many parameters");
                //x + (y*i) + (z*i*j) + (w*i*j*k) ...
                return get_index(std::pair<size_type, size_type>(Size, index), std::pair<size_type, size_type>(Sizes, indices)...);
            }
            constexpr reference front() { return data_[0]; }
            constexpr const_reference front() const { return front(); }
            constexpr reference back() { return data_[size_ - 1]; }
            constexpr const_reference back() const { return back(); }
            constexpr T* data() noexcept { return data_; }
            constexpr const T* data() const noexcept { return data(); }
            //Iterators
            constexpr iterator begin() noexcept { return data_; }
            constexpr iterator end() noexcept { return begin() + size_; }
            constexpr const_iterator begin() const noexcept { return data_; }
            constexpr const_iterator end() const noexcept { return begin() + size_; }
            constexpr iterator cbegin() const noexcept { return begin(); }
            constexpr iterator cend() const noexcept { return begin() + size_; }
            //Capacity
            constexpr size_type size() const noexcept { return size_; }
            constexpr bool empty() const noexcept { return begin() == end(); }
            constexpr size_type max_size() const noexcept { return std::numeric_limits<size_type>::max() / sizeof(T); }
            //Operations
            constexpr void fill(const T& value) {
                std::fill(begin(), end(), value);
            }
            constexpr void swap(array other)  noexcept(noexcept(std::swap(std::declval<value_type&>(), std::declval<value_type&>()))) {
                /*from swap_ranges*/
                iterator first1 = data();
                iterator first2 = other.data();
                while (first1 != end()) {
                    std::swap(*first1, *first2);
                    ++first1; ++first2;
                }
            }
            T data_array[size_];
            T* data_ = &data_array[0];
        private:
            //Helper functions
            template<typename Index, typename ... Indices>
            constexpr size_type get_index(const Index& index, const Indices& ... indices) const {
                size_type new_index = index.second;
                size_type final_index = 0;
                if constexpr (sizeof...(Indices)) {
                    new_index *= multply(std::get<0>(indices)...);
                    return get_index(indices...) + new_index;
                }
                return final_index + new_index;
            }
            template<typename Arg,typename... Args>
            constexpr void array_copy(const size_type& size,const Arg& arg, const Args&... args) {
                static_assert(sizeof...(args) < size_, "Too many parameters");
                data_[size-sizeof...(args)] = arg;
                if constexpr (sizeof...(args) > 0) {
                    array_copy(size,args...);
                }
            }
            constexpr reference data_at(const size_type& pos) { return data_[pos]; }
    };

    //Comparisons
    template<typename T, size_t Size, size_t ... Sizes>
    constexpr inline bool operator==(const array<T, Size, Sizes...>& one, const array<T, Size, Sizes...>& two) {
        auto first1 = one.begin(), first2 = two.begin();
        for (; first1 != one.end(); first1++, first2++) {
            if (*first1 != *first2) return false;
        }
        return true;
    }
} //namespace turtle
template<typename T, size_t Size, size_t ... Sizes>
using fixed_multi_array = turtle::array<T, Size, Sizes...>;