// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <array>
#include <vrm/core/config.hpp>
#include <vrm/core/assert.hpp>
#include <vrm/core/type_aliases.hpp>
#include <vrm/core/casts.hpp>
#include "./shared.hpp"

VRM_CORE_NAMESPACE
{
    namespace impl
    {
        namespace sparse_set_storage
        {
            template <         // .
                typename T,    // .
                sz_t TCapacity // .
                >
            class fixed_array
            {
                friend class sparse_set_storage::utils;

            public:
                using value_type = T;
                using sparse_type = T*;

            private:
                std::array<value_type, TCapacity> _dense;
                std::array<sparse_type, TCapacity> _sparse;
                sparse_type _end;

                auto& dense() noexcept
                {
                    return _dense;
                }

                const auto& dense() const noexcept
                {
                    return _dense;
                }

                auto& sparse() noexcept
                {
                    return _sparse;
                }

                const auto& sparse() const noexcept
                {
                    return _sparse;
                }

                auto last_element_ptr() const noexcept
                {
                    return _end - 1;
                }

                void grow_if_required(T) noexcept
                {
                    // Intentionally empty.
                }

                void set_last_element(T x) noexcept
                {
                    *_end = x;
                    _sparse[x] = _end;
                    ++_end;
                }

                bool is_null(sparse_type x) noexcept
                {
                    return x == nullptr;
                }

                void nullify(T x) noexcept
                {
                    sparse()[x] = nullptr;
                }

                void decrement_size() noexcept
                {
                    --_end;
                }

                auto& value_from_sparse(sparse_type x) noexcept
                {
                    return *x;
                }

            public:
                fixed_array() noexcept
                {
                    for(auto& p : _sparse)
                    {
                        p = nullptr;
                    }

                    _end = _dense.data();
                }

                fixed_array(const fixed_array&) = delete;
                fixed_array& operator=(const fixed_array&) = delete;

                fixed_array(fixed_array&&) = delete;
                fixed_array& operator=(fixed_array&&) = delete;

                void reserve(sz_t) noexcept
                {
                    // Intentionally empty.
                }

                void clear() noexcept
                {
                    for_each([this](auto x)
                        {
                            _sparse[x] = nullptr;
                        });

                    _end = _dense.data();
                }

                bool has(T x) const noexcept
                {
                    VRM_CORE_ASSERT_OP(x, <, TCapacity);
                    return _sparse[x] != nullptr;
                }

                auto capacity() const noexcept
                {
                    return TCapacity;
                }

                bool add(T x) noexcept
                {
                    VRM_CORE_ASSERT_OP(x, <, TCapacity);
                    return utils{}.add_impl(*this, x);
                }

                bool erase(T x) noexcept
                {
                    VRM_CORE_ASSERT_OP(x, <, TCapacity);
                    return utils{}.erase_impl(*this, x);
                }

                void unchecked_add(T x) noexcept
                {
                    VRM_CORE_ASSERT_OP(x, <, TCapacity);
                    return utils{}.unchecked_add(*this, x);
                }

                void unchecked_erase(T x) noexcept
                {
                    VRM_CORE_ASSERT_OP(x, <, TCapacity);
                    return utils{}.unchecked_erase(*this, x);
                }


                bool empty() const noexcept
                {
                    return _end == _dense.data();
                }

                void pop_back() noexcept
                {
                    utils{}.pop_back_impl(*this);
                }

                auto back() const noexcept
                {
                    VRM_CORE_ASSERT_OP(size(), >, 0);

                    VRM_CORE_ASSERT(has(*(last_element_ptr())));
                    return *(last_element_ptr());
                }

                template <typename TF>
                void for_each(TF&& f) const noexcept
                {
                    VRM_CORE_ASSERT_OP(size(), <=, TCapacity);

                    for(auto p(_dense.data()); p != _end; ++p)
                    {
                        VRM_CORE_ASSERT(has(*p));
                        f(*p);
                    }
                }

                auto size() const noexcept
                {
                    return to_sz_t(end() - begin());
                }

                auto begin() noexcept
                {
                    return _dense.data();
                }

                auto begin() const noexcept
                {
                    return _dense.data();
                }

                auto end() noexcept
                {
                    return _end;
                }

                auto end() const noexcept
                {
                    return static_cast<const T*>(_end);
                }

                auto at(sz_t i) const noexcept
                {
                    VRM_CORE_ASSERT_OP(i, <, size());
                    return dense()[i];
                }

                void swap(fixed_array& rhs) noexcept
                {
                    using std::swap;
                    swap(_dense, rhs._dense);
                    swap(_sparse, rhs._sparse);
                    swap(_end, rhs._end);
                }
            };
        }
    }
}
VRM_CORE_NAMESPACE_END
