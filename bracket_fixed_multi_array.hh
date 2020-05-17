#pragma once
#include <cstddef>
#include <utility>
#include <tuple>
#include <array>
#include <initializer_list>
#include <limits>
template<typename T, size_t Size, size_t ... Sizes>
class sub_array;
namespace turtle {
    template<typename T, size_t Size, size_t ... Sizes>
    #if __cplusplus > 201709L
    requires (Size != 0) && ((Sizes != 0) && ... && true) //prevent any size parameters from being zero
    #endif
    class array {
    protected:
        template<typename ... Args>
        static constexpr auto multply(const Args& ... args) {return (args * ...);}
        constexpr static size_t size_ =  multply(Size, Sizes...);
        constexpr static std::tuple<size_t,decltype(Sizes)...> sizes = std::make_tuple(Size,Sizes...);
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

        template<typename U, size_t Size_, size_t ... Sizes_>
        class sub_array : public array<T,Size_, Sizes_...> {
        public:
           T* data__;
           sub_array(T* data):data__(data){}
           //Element access
           constexpr reference at(size_type pos) { return data__[pos]; }
           constexpr const_reference at(size_type pos) const { return data__[pos]; }
           constexpr sub_array<T, Sizes_...>  operator[](size_type pos) {
           sub_array<T, Sizes_...> temp (data__+pos*multply(Sizes_...));
           return  temp;
           }
           constexpr const sub_array<T,Sizes_...>& operator[](size_type pos) const {return (*this)[pos];}
           template<typename Index, typename ... Indices>
           constexpr size_type index(const Index& index, const Indices& ... indices) {
               static_assert (sizeof...(Indices) >= sizeof...(Sizes_),"not enough parameters");
               static_assert (sizeof...(Indices) <= sizeof...(Sizes_),"too many parameters");
               //x + (y*i) + (z*i*j) + (w*i*j*k) ...
               return get_index(std::pair<size_type, size_type>(Size_, index), std::pair<size_type, size_type>(Sizes_, indices)...);
           }
           constexpr reference front() { return data__[0]; }
           constexpr const_reference front() const { return this->front(); }
           constexpr reference back() { return data__[this->size_ - 1]; }
           constexpr const_reference back() const { return this->back(); }
           constexpr T* data() noexcept { return data__; }
           constexpr const T* data() const noexcept { return data__; }
           //Iterators
           constexpr iterator begin() noexcept { return data__; }
           constexpr iterator end() noexcept { return data__ + this->size_; }
           constexpr const_iterator begin() const noexcept { return data__; }
           constexpr const_iterator end() const noexcept { return data__ + this->size_; }
           constexpr iterator cbegin() const noexcept { return data__; }
           constexpr iterator cend() const noexcept { return data__ + this->size_; }
           //Capacity
           constexpr size_type size() const noexcept { return  this->size_; }
           constexpr bool empty() const noexcept { return this->begin() == this->begin(); }
           constexpr size_type max_size() const noexcept { return std::numeric_limits<size_type>::max() / sizeof(T); }
           //Operations
           constexpr void fill(const T& value) {
               std::fill(begin(),end(), value);
           }
           constexpr void swap(sub_array other) const noexcept(noexcept(std::swap(std::declval<value_type&>(), std::declval<value_type&>()))) {
               /*from swap_ranges*/
               iterator first1 = data__;
               iterator first2 = other.data__;
               while (first1 != end()) {
                   std::swap(*first1, *first2);
                   ++first1; ++first2;
               }
           }
        };
        template<typename U, size_t Size_>
        class sub_array<U,Size_> : public sub_array<T,Size_, 1> {
        public:
           //Element access
           constexpr reference operator[](size_type pos) {return  this->data__[pos];}
        };
        ~array() = default;
        //Element access
        constexpr reference at(size_type pos) { return data_[pos]; }
        constexpr const_reference at(size_type pos) const { return data_[pos]; }
        constexpr sub_array<T,Sizes...> operator[](size_type pos) {
            auto temp = sub_array<T,Sizes...>(&data_[pos*multply(Sizes...)]);
            return temp;
        }
        constexpr const sub_array<T,Sizes...>& operator[](size_type pos) const {return (*this)[pos];}
        template<typename Index, typename ... Indices>
        constexpr size_type index(const Index& index, const Indices& ... indices) {
            static_assert (sizeof...(Indices) >= sizeof...(Sizes),"not enough parameters");
            static_assert (sizeof...(Indices) <= sizeof...(Sizes),"too many parameters");
            //x + (y*i) + (z*i*j) + (w*i*j*k) ...
            return get_index(std::pair<size_type, size_type>(Size, index), std::pair<size_type, size_type>(Sizes, indices)...);
        }
        constexpr reference front() { return data_[0]; }
        constexpr const_reference front() const { return front(); }
        constexpr reference back() { return data_[size_ - 1]; }
        constexpr const_reference back() const { return back(); }
        constexpr T* data() noexcept { return data_; }
        constexpr const T* data() const noexcept { return data_; }
        //Iterators
        constexpr iterator begin() noexcept { return data_; }
        constexpr iterator end() noexcept { return data_ + size_; }
        constexpr const_iterator begin() const noexcept { return data_; }
        constexpr const_iterator end() const noexcept { return data_ + size_; }
        constexpr iterator cbegin() const noexcept { return data_; }
        constexpr iterator cend() const noexcept { return data_ + size_; }
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
            iterator first1 = data_;
            iterator first2 = other.data_;
            while (first1 != end()) {
                std::swap(*first1, *first2);
                ++first1; ++first2;
            }
        }
        T data_[size_];
        private:
        //Helper functions
        template<typename Arg, typename ... Args>
        static constexpr size_type multply_pair(const Arg& arg, const Args& ... args) {
            if constexpr (sizeof...(args) > 0) {
                size_type remainder = multply_pair(args...);
                return arg.first * remainder;
            }
            return arg.first;
        }
        template<typename Index, typename ... Indices>
        constexpr size_type get_index(const Index& index, const Indices& ... indices) const {
            size_type new_index = index.second;
            size_type final_index = 0;
            if constexpr (sizeof...(Indices)) {
                new_index *= multply_pair(indices...);
                return get_index(indices...) + new_index;
            }
            return final_index + new_index;
        }

    };
    //Comparisons
    template<typename T, size_t Size, size_t ... Sizes>
    constexpr inline bool operator==(const array<T,Size,Sizes...>& one, const array<T,Size,Sizes...>& two){
       auto first1 = one.begin(), first2 = two.begin();
       for(;first1!=one.end();first1++,first2++){
           if(*first1!=*first2) return false;
       }
       return true;
    }
    template<typename T, size_t Size>
    #if __cplusplus > 201709L
    requires (Size != 0) //prevent any size parameters from being zero
    #endif
    class array<T,Size> : public array<T,Size,1> {
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

        constexpr reference operator[](size_type pos) {return this->data_[pos];}
        constexpr const_reference operator[](size_type pos) const {return (*this)[pos];}
    };
} //namespace turtle
template<typename T, size_t Size, size_t ... Sizes>
using fixed_multi_array = turtle::array<T,Size,Sizes...>;


