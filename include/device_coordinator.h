#pragma once

namespace memory {

    template <typename T, class Allocator_=cuda_allocator<T> >
    class device_coordinator {
    public:
        typedef T value_type;
        typedef typename Allocator_::template rebind<value_type>::other Allocator;

        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;

        typedef range<value_type> range_type;

        typedef typename types::size_type size_type;
        typedef typename types::difference_type difference_type;

        // metafunction for rebinding host_coordinator with another type
        template <typename Tother>
        struct rebind {
            typedef device_coordinator<Tother, Allocator> other;
        };

        range_type allocate(size_type n) {
            Allocator allocator;

            // only allocate memory if nonzero memory allocation has been requested
            pointer ptr = n>0 ? allocator.allocate(n) : 0;

            return range_type(ptr, n);
        }

        void free(range_type& rng) {
            Allocator allocator;

            if(rng.data())
                allocator.deallocate(rng.data(), rng.size());

            rng.reset();
        }

        // copy memory from one range into another
        void copy(const range_type &from, range_type &to) {
            // free memory associated with R2
            assert(from.size()==to.size());
            assert(!from.overlaps(to));

            std::copy(from.begin(), from.end(), to.begin());
        }
    };

} // namespace memory
