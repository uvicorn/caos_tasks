#include "engine.hpp"

#include <array>
#include <bit>
#include <cstring>
#include <limits>
#include <type_traits>
#include <utility>

namespace {

template <class... T> struct t0 {};
template <unsigned N, class... T> struct t1 {};
template <class T> struct t2 { using type = T; };

template <class A, class B> struct t3;
template <class... A, class... B>
struct t3<t0<A...>, t0<B...>> : t2<t0<A..., B...>> {};

template <class T> struct t4;
template <> struct t4<t0<>> : t2<t0<>> {};
template <class H, class... T>
struct t4<t0<H, T...>>
    : t3<typename t4<t0<T...>>::type, t0<H>> {};

template <std::size_t N, class T> struct t5;
template <class H, class... T>
struct t5<0, t0<H, T...>> : t2<H> {};
template <std::size_t N, class H, class... T>
struct t5<N, t0<H, T...>> : t5<N - 1, t0<T...>> {};

template <class T> struct t6;
template <class... T>
struct t6<t0<T...>> : std::integral_constant<std::size_t, sizeof...(T)> {};

template <std::size_t N, class T>
using t7 = typename t5<N, T>::type;

using t8 = typename t4<t0<std::int64_t, std::uint32_t, std::int32_t,
                           std::uint16_t, std::int16_t, std::uint8_t>>::type;

consteval std::uint32_t f0(std::size_t n) {
    auto x = 0x9e3779b9u ^ ((static_cast<std::uint32_t>(n) + 1u) * 0x85ebca6bu);
    for (std::uint32_t j = 0; j != 3; ++j) {
        x ^= x >> 16;
        x *= 0x7feb352du;
        x ^= x >> 15;
        x *= 0x846ca68bu;
        x ^= x >> 16;
        x += 0x9e3779b9u + j;
    }
    return x;
}

inline constexpr std::array<std::uint32_t, 5> v0{
    0x69358835u, 0xc8ebcf58u, 0x14670f59u, 0x300d38e0u, 0xe48cbed8u,
};

template <std::size_t... I>
consteval auto f1(std::index_sequence<I...>) {
    return std::array{
        (std::rotr(v0[(I * 3 + 1) % v0.size()],
                   static_cast<int>((I * 7 + 3) % 31 + 1)) ^ f0(I))...,
    };
}

inline constexpr auto v1 = f1(std::make_index_sequence<v0.size()>{});

template <std::size_t I>
using t9 = std::integral_constant<unsigned, (v1[I] >> 24)>;
template <std::size_t I>
using ta = std::integral_constant<std::uint32_t, v1[I] & 0x00ffffffu>;

consteval bool f2() {
    std::size_t n = 0;
    for (auto x : v1) {
        const auto a = x >> 24;
        const auto b = x & 0x00ffffffu;
        if (a < 2) {
            if (a == 0 && b >= t6<t8>::value) {
                return false;
            }
            ++n;
        } else {
            if (a > 3 || n < 2) {
                return false;
            }
            --n;
        }
    }
    return n == 1;
}

static_assert(f2());

template <unsigned A, std::uint32_t B, class S> struct tb;

template <std::uint32_t B, class... S>
struct tb<0, B, t0<S...>> : t2<t0<t1<0, t7<B, t8>>, S...>> {};

template <std::uint32_t B, class... S>
struct tb<1, B, t0<S...>>
    : t2<t0<t1<1, std::integral_constant<std::int32_t,
                                        static_cast<std::int32_t>(B)>>, S...>> {};

template <std::uint32_t B, class R, class L, class... S>
struct tb<2, B, t0<R, L, S...>> : t2<t0<t1<2, L, R>, S...>> {};

template <std::uint32_t B, class R, class L, class... S>
struct tb<3, B, t0<R, L, S...>> : t2<t0<t1<3, L, R>, S...>> {};

template <std::size_t I, class S, bool = (I == v1.size())> struct tc;

template <std::size_t I, class S>
struct tc<I, S, false>
    : tc<I + 1, typename tb<t9<I>::value, ta<I>::value, S>::type> {};

template <std::size_t I, class S>
struct tc<I, S, true> : t5<0, S> {
    static_assert(t6<S>::value == 1);
};

template <class T> struct td;

template <class T> struct td<t1<0, T>> {
    static_assert(sizeof(T) == sizeof(std::int16_t));
    [[gnu::always_inline]] static inline std::int32_t f(const void *p) noexcept {
        T v;
        std::memcpy(&v, p, sizeof(v));
        return static_cast<std::int32_t>(v);
    }
};

template <class T> struct td<t1<1, T>> {
    [[gnu::always_inline]] static inline std::int32_t f(const void *) noexcept {
        return T::value;
    }
};

template <class L, class R> struct td<t1<2, L, R>> {
    [[gnu::always_inline]] static inline std::int32_t f(const void *p) noexcept {
        return td<L>::f(p) * td<R>::f(p);
    }
};

template <class L, class R> struct td<t1<3, L, R>> {
    [[gnu::always_inline]] static inline std::int32_t f(const void *p) noexcept {
        return td<L>::f(p) - td<R>::f(p);
    }
};

using te = td<typename tc<0, t0<>>::type>;

static_assert((3ll * 65535 + 7) * max_samples <
              std::numeric_limits<std::int32_t>::max());

} // namespace

extern "C" [[gnu::noinline]]
std::int32_t telemetry_fold(const std::int16_t *samples, std::size_t count) noexcept {
    std::int32_t result = 0;
    for (std::size_t i = 0; i != count; ++i) {
        result += te::f(samples + i);
    }
    return result;
}
