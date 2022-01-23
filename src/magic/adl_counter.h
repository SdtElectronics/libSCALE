/**
    @file      constexpr_sequence.h
    @brief     Contains qx::constexpr_sequence class
    @author    Khrapov
    @date      25.08.2020
    @copyright © Nick Khrapov, 2021. All right reserved.
**/

#pragma once

#include <cstddef>

/**
    @class   constexpr_flag
    @brief   Constexpr flag class
    @details Returns "Start" at the beginning(test), "Start" with test_and_set and "End" after
    @tparam  Tag   - tag for unique instances 
    @tparam  T     - value type 
    @tparam  Start - start value 
    @tparam  End   - end value
    @author  Khrapov
    @date    14.09.2020
**/
template<typename Tag, typename T = bool, T Start = false, T End = true>
class constexpr_flag
{
private:
    struct Dummy
    {   
        friend constexpr void adl_flag(Dummy) noexcept;
    };

    template<T>
    struct Writer
    {
        friend constexpr void adl_flag(Dummy) noexcept
        {
        }
    };

    template<class Dummy, int = (adl_flag(Dummy {}), 0)>
    static constexpr T check(int) noexcept
    {
        return End;
    }

    template<class Dummy>
    static constexpr T check(short) noexcept
    {
        return Start;
    }

public:
    template<class Dummy = Dummy, T Value = check<Dummy>(0)>
    static constexpr T test_and_set(void) noexcept
    {
        Writer<Value && 0> tmp {};
        (void)tmp;
        return Value;
    }

    template<class Dummy = Dummy, T Value = check<Dummy>(0)>
    static constexpr T test(void) noexcept
    {
        return Value;
    }
};

template<class Tag, typename T, T Start, T Func(T)>
class constexpr_sequence
{
private:
    template<T nIndex>
    struct Element
    {
        static constexpr T value(void) noexcept
        {
            T _value = Start;

            for (std::size_t i = 0; i < nIndex; i++)
                _value = Func(_value);

            return _value;
        }
    };

    template<std::size_t nCurrent, bool bWasSet /* = false */>
    struct CheckerSetter
    {
        static constexpr std::size_t index(void) noexcept
        {
            return nCurrent;
        }
    };

    template<T nCurrent>
    struct CheckerWrapper
    {
        template<
            bool        bWasSet = constexpr_flag<Element<nCurrent>> {}.test(),
            std::size_t nNext   = CheckerSetter<nCurrent, bWasSet> {}.index()>
        static constexpr std::size_t index(void) noexcept
        {
            return nNext;
        }
    };

    template<std::size_t nCurrent>
    struct CheckerSetter<nCurrent, /* bool bWasSet = */ true>
    {
        template<std::size_t nNext = CheckerWrapper<nCurrent + 1> {}.index()>
        static constexpr std::size_t index(void) noexcept
        {
            return nNext;
        }
    };

public:
    /**
        @brief   Get current sequence value
        @details As function works through generating new template instance each time,
                 return value will be the same for each code line. That means in loops
                 you will get same value each time. Use macros like BOOST_PP_REPEAT
                 to simplify the code
        @tparam  nIndex - current sequence index
        @tparam  _value - current sequence value
        @retval         - current sequence value
    **/
    template<
        std::size_t nIndex = CheckerWrapper<0> {}.index(),
        T           _value = Element<nIndex> {}.value()>
    static constexpr T value(void) noexcept
    {
        return _value;
    }

    /**
        @brief   Change value by Func and return new value
        ¹details As function works through generating new template instance each time,
                 return value will be the same for one code line. That means in loops
                 you will get same value each time. Use macros like BOOST_PP_REPEAT
                 to simplify the code
        @tparam  nIndex - current sequence index
        @tparam  _value - current sequence value
        @tparam  bStub  - stub parameter
        @retval         - next sequence value
    **/
    template<
        std::size_t nIndex = CheckerWrapper<0> {}.index(),
        T           _value = Element<nIndex> {}.value(),
        bool        bStub  = constexpr_flag<Element<nIndex>> {}.test_and_set()>
    static constexpr T next(void) noexcept
    {
        return Func(_value);
    }
};

namespace detail
{

template<typename T, T INC>
constexpr T increase(T val)
{
    return val + INC;
}

template<typename T, T MULT>
constexpr T multiply(T val)
{
    return val * MULT;
}

} // namespace detail

template<
    typename Tag = struct CounterTag,
    typename T   = int,
    T Start      = 0,
    T Inc        = 1>
using constexpr_counter =
    constexpr_sequence<Tag, T, Start, detail::increase<T, Inc>>;

template<
    typename Tag = struct MultiplierTag,
    typename T   = int,
    T Start      = 1,
    T Mult       = 2>
using constexpr_multiplier =
    constexpr_sequence<Tag, T, Start, detail::multiply<T, Mult>>;
