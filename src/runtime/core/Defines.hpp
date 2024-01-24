#pragma once

#include <type_traits>

#define MOVABLE_ONLY(T) \
        T(const T&) = delete; \
        T(T&&) = default; \
        T& operator=(const T&) = delete; \
        T& operator=(T&&) = default;

#define NON_COPYABLE_NOR_MOVABLE(T) \
        T(const T&) = delete; \
        T(T&&) = delete; \
        T& operator=(const T&) = delete; \
        T& operator=(T&&) = delete; 

#define ENUM_FLAG_OPERATOR(T, OPERATOR) \
        inline T operator OPERATOR(T lhs, T rhs) { \
        return (T) (static_cast<std::underlying_type_t<T>>(lhs) OPERATOR static_cast<std::underlying_type_t<T>>(rhs)); } \
        inline T& operator OPERATOR ## =(T& lhs, T rhs) { \
        lhs = (T)(static_cast<std::underlying_type_t<T>>(lhs) OPERATOR static_cast<std::underlying_type_t<T>>(rhs)); \
        return lhs; }

#define ENUM_FLAGS(T) \
        enum class T; \
        inline T operator~(T t) { return (T) (~static_cast<std::underlying_type_t<T>>(t)); } \
        ENUM_FLAG_OPERATOR(T, |) \
        ENUM_FLAG_OPERATOR(T, ^) \
        ENUM_FLAG_OPERATOR(T, &) \
        enum class T
