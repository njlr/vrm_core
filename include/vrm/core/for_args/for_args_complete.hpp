// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <tuple>
#include <utility>

#include <vrm/core/config.hpp>
#include <vrm/core/assert.hpp>
#include <vrm/core/type_aliases/numerical.hpp>
#include <vrm/core/type_traits.hpp>
#include <vrm/core/static_if.hpp>
#include <vrm/core/args_utils.hpp>
#include <vrm/core/tuple_utils/apply.hpp>
#include <vrm/core/utility_macros/fwd.hpp>

// TODO: temp
#include <iostream>
#include <typeinfo>

// Implemented thanks to Daniel Frey:
// http://stackoverflow.com/a/29901074/598696

// TODO: cleanup with cppcon2015 implementation
// TODO: short circuiting with static_if
// TODO: return value
/*
struct for_args_continue{};
       struct for_args_break{};
       struct for_args_return{};*/

// TODO: to test, implement "std:cin choice switch" that returns choice int or
// executes choice function

// TODO: what is more general? static_for_ints? static_for_args?

// TODO: constexpr break_t break_v{};
// ...and others.

VRM_CORE_NAMESPACE
{
    template <typename T>
    struct result_wrapper
    {
        using type = T;
    };

    struct break_t
    {
    };

    struct skip_t_base
    {
    };

    template <sz_t TSkipCount>
    struct skip_t : skip_t_base, sz_t_<TSkipCount>
    {
    };

    using continue_t = skip_t<0>;

    // Aliases.
    template <typename T>
    using is_break = std::is_same<break_t, std::decay_t<T>>;

    template <typename T>
    using is_skip = std::is_base_of<skip_t_base, std::decay_t<T>>;

    template <typename T>
    struct wrap
    {
        using type = T;
    };

    namespace impl
    {
        // Wish we had `constexpr` lambdas.
        struct zero_returner
        {
            template <typename T>
            constexpr auto operator()(T)
            {
                return 0;
            }
        };

        struct skip_value_returner
        {
            template <typename T>
            constexpr auto operator()(T)
            {
                using inner_type = typename T::type;
                return inner_type{};
            }
        };
    }

    template <typename T>
    constexpr auto get_continue_count() noexcept
    {
        return static_if(is_skip<T>{})
            .then(impl::skip_value_returner{})
            .else_(impl::zero_returner{})(wrap<T>{});
    }


    template <sz_t TIteration, typename TLastReturn>
    struct static_for_args_data_type
    {
        static constexpr sz_t iteration{TIteration};
        using last_return_type = TLastReturn;
    };

    /*
        template <sz_t TIncrement, typename>
        struct data_type_inc_helper;

        template <sz_t TIncrement, sz_t TIteration, typename TLastReturn>
        struct data_type_inc_helper<
            static_for_args_data_type<TIteration, TLastReturn>>
        {
            using type =
                static_for_args_data_type<TIteration + TIncrement, TLastReturn>;
        };

        template <sz_t TIncrement, typename TDataType>
        using incremented_data_type =
            typename data_type_inc_helper<TIncrement, TDataType>::type;
    */

    namespace impl
    {
        template <sz_t TArity, typename TFunctionToCall>
        struct static_for_args_result : TFunctionToCall
        {
        private:
            VRM_CORE_ALWAYS_INLINE auto& as_f_to_call() noexcept
            {
                return static_cast<TFunctionToCall&>(*this);
            }

            template <sz_t TIteration, typename TLastResult>
            using data_type =
                static_for_args_data_type<TIteration, TLastResult>;

            template <typename TCurrDataType, typename TRet, sz_t TNumSkip>
            VRM_CORE_ALWAYS_INLINE constexpr auto get_next_data_type() noexcept
            {
                using return_type = data_type< // .
                    TCurrDataType::iteration,  // .
                    TRet                       // .
                    >;

                return return_type{};
            }

        public:
            template <typename TFFwd>
            VRM_CORE_ALWAYS_INLINE static_for_args_result(TFFwd&& f) noexcept
                : TFunctionToCall(FWD(f))
            {
            }


            // template <sz_t TNextIteration, sz_t TSkipCount, typename... Ts>
            template <typename TCurrRet, typename TCurrDataType, typename... Ts>
            VRM_CORE_ALWAYS_INLINE decltype(auto) continue_(
                TCurrDataType, Ts&&... xs)
            {
                // Executes the next iterator of the loop by applying all
                // arguments, starting from TArity, to the `impl_` function.

                constexpr sz_t num_skip(get_continue_count<TCurrRet>());

                auto next_data_type_val( // .
                    get_next_data_type<  // .
                        TCurrDataType,   // .
                        TCurrRet,        // .
                        num_skip         // .
                        >());

                using next_data = decltype(next_data_type_val);

                constexpr auto skips(num_skip + 1);
                constexpr auto real_next_iteration(
                    next_data::iteration + skips);
                constexpr auto slice_begin(TArity * skips);

                return apply(
                    [this](auto&&... vs)
                    {
                        return this->impl_(next_data{}, FWD(vs)...);
                    },
                    all_args_from<slice_begin>(FWD(xs)...));
            }

            template <typename TDataType, typename... Ts>
            VRM_CORE_ALWAYS_INLINE decltype(auto) call_with_arity(
                TDataType, Ts&&... xs)
            {
                // Calls the "stored" function with `TArity` args, instantiating
                // a `data_type` object depending on the current iteration.

                // TODO: better syntax?
                return apply(
                    [this](auto&&... ys)
                    {
                        return as_f_to_call()(TDataType{}, FWD(ys)...);
                    },
                    first_n_args<TArity>(FWD(xs)...));
            }

            template <typename TDataType, typename... Ts>
            VRM_CORE_ALWAYS_INLINE decltype(auto) impl_(TDataType, Ts&&... xs)
            {
                // Make sure that the count of loop arguments is divisible by
                // `TArity`.
                VRM_CORE_STATIC_ASSERT_NM(sizeof...(xs) % TArity == 0);

                // Get the return type of the current iteration.
                using curr_data_type = TDataType;
                using ret_t =
                    decltype(call_with_arity(curr_data_type{}, FWD(xs)...));

                // TODO:
                using next_data_type = data_type< // .
                    curr_data_type::iteration,    // .
                    ret_t                         // .
                    // typename curr_data_type::last_return_type // .
                    >;

                // Call the function, regardless of return type.

                // decltype(auto) call_result(
                call_with_arity(curr_data_type{}, FWD(xs)...);
                // );



                // TODO: refactor

                return static_if(is_break<ret_t>{})
                    .then([&](auto&&...)
                        {
                            return result_wrapper<ret_t>{};
                        })
                    .else_([&](auto&&... zs)
                        {
                            constexpr auto has_args(sizeof...(zs) > TArity);
                            return static_if(bool_v<has_args>)
                                .then([&](auto&&... ys)
                                    {
                                        // this->continue_<TIteration +
                                        // 1>(FWD(ys)...);
                                        return this->continue_<ret_t>(
                                            curr_data_type{}, FWD(ys)...);
                                    })
                                .else_([](auto&&...)
                                    {
                                        return result_wrapper<ret_t>{};
                                    })(FWD(zs)...);
                        })(FWD(xs)...);
            }

            template <typename... Ts>
            VRM_CORE_ALWAYS_INLINE decltype(auto) operator()(Ts&&... xs)
            // TODO: noexcept
            {
                return impl_(data_type<0, void>{}, FWD(xs)...);
            }
        };
    }

    template <sz_t TArity = 1, typename TF>
    VRM_CORE_ALWAYS_INLINE decltype(auto) static_for_args(TF && f)
    {
        return impl::static_for_args_result<TArity, std::decay_t<decltype(f)>>(
            FWD(f));
    }
}
VRM_CORE_NAMESPACE_END
