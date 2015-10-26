// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <memory>
#include <tuple>
#include <vrm/core/config.hpp>
#include <vrm/core/assert.hpp>
#include <vrm/core/resizable_buffer.hpp>
#include <vrm/core/tuple_utils.hpp>

VRM_CORE_NAMESPACE
{
    namespace impl
    {
        template <typename TMultiBuffer>
        class VRM_CORE_CLASS_API multi_vector
        {
        public:
            using size_type = std::size_t;
            using multi_buffer_type = TMultiBuffer;

            static constexpr size_type initial_capacity{0};

            using value_reference_tuple =
                typename multi_buffer_type::value_reference_tuple;

            using const_value_reference_tuple =
                typename multi_buffer_type::const_value_reference_tuple;

        private:
            multi_buffer_type _multi_buffer;
            size_type _capacity{0};
            size_type _size{0};

            void grow_if_necessary(size_type desired_size)
            {
                if(desired_size > _capacity)
                {
                    reserve((_capacity + 10) * 2);
                }
            }

        public:
            multi_vector() = default;

            ~multi_vector()
            {
                _multi_buffer.destroy(0, _size);
                _multi_buffer.deallocate(_capacity);
            }

            // TODO: implement
            multi_vector(const multi_vector&) = default;
            multi_vector& operator=(const multi_vector&) = default;

            multi_vector(multi_vector&&) = default;
            multi_vector& operator=(multi_vector&&) = default;

            const auto& size() const noexcept { return _size; }
            const auto& capacity() const noexcept { return _capacity; }
            auto empty() const noexcept { return _size == 0; }

            void reserve(size_type new_capacity)
            {
                VRM_CORE_ASSERT_OP(new_capacity, >, _capacity);

                _multi_buffer.grow(_capacity, new_capacity);
                _capacity = new_capacity;
            }

            void resize(size_type new_size)
            {
                VRM_CORE_ASSERT_OP(new_size, >, _size);
                grow_if_necessary(new_size);

                for(size_type i(_size); i < new_size; ++i)
                    _multi_buffer.construct_at(i);

                _size = new_size;
            }

            void clear()
            {
                _multi_buffer.destroy(0, _size);
                _size = 0;
            }

            // TODO:
            void insert() {}
            void emplace() {}
            void erase() {}

            // TODO:
            void push_back() {}
            void emplace_back() {}
            void pop_back() {}

            auto operator[](size_type pos) noexcept
            {
                return _multi_buffer[pos];
            }

            auto operator[](size_type pos) const noexcept
            {
                return _multi_buffer[pos];
            }
        };
    }
}
VRM_CORE_NAMESPACE_END
