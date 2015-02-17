/* 
 * File:   range_wrapper.h
 * Author: bcumming
 *
 * Created on May 3, 2014, 5:14 PM
 */

#pragma once

#include <iostream>
#include <type_traits>

#include "definitions.h"
#include "util.h"
#include "ArrayView.h"

////////////////////////////////////////////////////////////////////////////////
namespace memory{

// forward declarations
template<typename T, typename Coord>
class Array;

namespace util {
    template <typename T, typename Coord>
    struct type_printer<Array<T,Coord>>{
        static std::string print() {
            std::stringstream str;
            str << "Array<" << type_printer<T>::print()
                << ", " << type_printer<Coord>::print() << ">";
            return str.str();
        }
    };

    template <typename T, typename Coord>
    struct pretty_printer<Array<T,Coord>>{
        static std::string print(const Array<T,Coord>& val) {
            std::stringstream str;
            str << type_printer<Array<T,Coord>>::print()
                << "(size="     << val.size()
                << ", pointer=" << val.data() << ")";
            return str.str();
        }
    };
}

// metafunctions for checking array types
template <typename T>
struct is_array_by_value : std::false_type {};

template <typename T, typename Coord>
struct is_array_by_value<Array<T, Coord> > : std::true_type {};

template <typename T>
struct is_array
    : std::conditional< is_array_by_value<T>::value || is_array_by_reference<T>::value,
                        std::true_type,
                        std::false_type>::type
{};

// array by value
// this wrapper owns the memory in the array
// and is responsible for allocating and freeing memory
template <typename T, typename Coord>
class Array
    : public ArrayView<T, Coord> {
public:
    using value_type = T;
    using base       = ArrayView<value_type, Coord>;
    using view_type  = ArrayView<value_type, Coord>;

    using coordinator_type = typename Coord::template rebind<value_type>;

    using size_type       = typename base::size_type;
    using difference_type = typename base::difference_type;

    using pointer       = value_type*;
    using const_pointer = const pointer;
    // TODO what about specialized references for things like GPU memory?
    using reference       = value_type&;
    using const_reference = value_type const&;

    // default constructor
    // we have to call constructor in ArrayView: pass base
    Array() : base(nullptr, 0) {}

    // constructor by size
    template < typename I,
               typename std::enable_if<std::is_integral<I>::value>::type* = nullptr>
    Array(I n)
        : base(coordinator_type().allocate(n))
    {
        #ifdef VERBOSE
        std::cerr << util::green("Array(integral_type) ")
                  << util::pretty_printer<Array>::print(*this) << std::endl;
        #endif
    }

    // construct as a copy of another range
    /*
    template <
        class RangeType,
        typename std::enable_if<is_array<RangeType>::value, void>::type* = nullptr
    >
    explicit Array(const RangeType& other)
        : base(coordinator_type().allocate(other.size()))
    {
        coordinator_.copy(static_cast<view_base const&>(other), *this);
    }
    */

    Array(const view_type& other)
        : base(coordinator_type().allocate(other.size()))
    {
        #ifdef VERBOSE
        std::cerr << util::green("Array(other&)") + " other = " << util::pretty_printer<view_type>::print(other) << std::endl;
        #endif
        coordinator_.copy(static_cast<base const&>(other), *this);
    }

    Array(const Array& other)
        : base(coordinator_type().allocate(other.size()))
    {
        #ifdef VERBOSE
        std::cerr << util::green("Array(other&)") + " other = " << util::pretty_printer<view_type>::print(other) << std::endl;
        #endif
        coordinator_.copy(static_cast<base const&>(other), *this);
    }

    Array& operator = (const Array& other) {
        #ifdef VERBOSE
        std::cerr << util::green("Array operator=(other&)") + " other = " << util::pretty_printer<view_type>::print(other) << std::endl;
        #endif
        coordinator_.free(*this);
        auto ptr = coordinator_type().allocate(other.size());
        base::reset(ptr.data(), other.size());
        coordinator_.copy(static_cast<base const&>(other), *this);
        return *this;
    }

    Array(Array&& other) {
        #ifdef VERBOSE
        std::cerr << util::green("Array(other&&)") + " other = " << util::pretty_printer<view_type>::print(other) << std::endl;
        #endif
        base::swap(other);
    }

    Array& operator = (Array&& other) {
        #ifdef VERBOSE
        std::cerr << util::green("Array operator=(other&&)") + " other = " << util::pretty_printer<view_type>::print(other) << std::endl;
        #endif
        base::swap(other);
        return *this;
    }

    // have to free the memory in a "by value" range
    ~Array() {
        #ifdef VERBOSE
        //std::cerr << util::yellow("~Array() ") << util::pretty_printer<Array>::print(*this) << std::endl;
        std::cerr << util::yellow("~Array() ") << "size " << size()*sizeof(value_type) << " bytes @ " << base::data() << std::endl;
        #endif
        coordinator_.free(*this);
    }

    // use the accessors provided by ArrayView
    // this enforces the requirement that accessing all of or a sub-array of an
    // Array should return a view, not a new array.
    using base::operator();

    const coordinator_type& coordinator() const {
        return coordinator_;
    }

    using base::size;

private:
    coordinator_type coordinator_;
};

} // namespace memory
////////////////////////////////////////////////////////////////////////////////

