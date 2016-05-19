// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <vrm/core/config/names.hpp>

VRM_CORE_NAMESPACE
{
    namespace detection
    {
        enum class operating_system
        {
            os_linux,
            os_windows,
            os_mac,
            os_android,
            os_freebsd,
            os_unknown
        };

        constexpr auto current_operating_system() noexcept;

        constexpr auto on_unix() noexcept;
        constexpr auto on_windows() noexcept;
        constexpr auto on_mac() noexcept;
    }
}
VRM_CORE_NAMESPACE_END
