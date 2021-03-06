// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <type_traits>
#include <utility>
#include <vrm/core/config/names.hpp>
#include <vrm/core/type_aliases.hpp>

VRM_CORE_NAMESPACE
{
    template <typename TOut, typename TIn>
    using same_signedness =
        bool_<std::is_signed<TOut>{} == std::is_signed<TIn>{}>;

    template <typename TEnum, typename TOut>
    using underlying_convertible_to =
        bool_<std::is_convertible<std::underlying_type_t<TEnum>, TOut>{}>;

    template <typename TOut, typename TIn>
    using number_convertible_to_enum =
        bool_<std::is_enum<TOut>() && !std::is_enum<TIn>() &&
              underlying_convertible_to<TOut, TIn>{}>;

    template <typename TOut, typename TIn>
    using are_both_enums = bool_<std::is_enum<TOut>() && std::is_enum<TIn>()>;

    template <typename TOut, typename TIn>
    using are_underlying_types_convertible =
        bool_<std::is_convertible<std::underlying_type_t<TOut>,
            std::underlying_type_t<TIn>>{}>;

    template <typename T, typename TStorage>
    using valid_storage = bool_<sizeof(typename TStorage::type) >= sizeof(T) &&
                                alignof(typename TStorage::type) >= alignof(T)>;
}
VRM_CORE_NAMESPACE_END
