//
// Created by jzh on 18-1-4.
//

#ifndef STL_FUNCTIONAL_H
#define STL_FUNCTIONAL_H
namespace mystl {

//所有一元仿函数的基类
    template<class Arg, class Result>
    struct unary_function {
        typedef Arg argument_type;
        typedef Result result_type;
    };
//所有二元仿函数的基类
    template<class Arg1, class Arg2, class Result>
    struct binary_function {
        typedef Arg1 firstArgument_type;
        typedef Arg2 secondArgument_type;
        typedef Result result_type;
    };

    template<class _Tp>
    struct plus : public binary_function<_Tp, _Tp, _Tp> {
        _Tp operator()(const _Tp &__x, const _Tp &__y) const { return __x + __y; }
    };

    template<class _Tp>
    struct minus : public binary_function<_Tp, _Tp, _Tp> {
        _Tp operator()(const _Tp &__x, const _Tp &__y) const { return __x - __y; }
    };

    template<class _Tp>
    struct multiplies : public binary_function<_Tp, _Tp, _Tp> {
        _Tp operator()(const _Tp &__x, const _Tp &__y) const { return __x * __y; }
    };

    template<class _Tp>
    struct divides : public binary_function<_Tp, _Tp, _Tp> {
        _Tp operator()(const _Tp &__x, const _Tp &__y) const { return __x / __y; }
    };

    template<class T>
    struct modulus : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const { return x % y; }
    };

    template<class T>
    struct negate : public unary_function<T, T> {
        T operator()(const T &x) const { return -x; }
    };

    template<class T>
    struct equal_to : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x == y; }
    };

    template<class T>
    struct not_equal_to : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x != y; }
    };

    template<class T>
    struct greater : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x > y; }
    };

    template<class T>
    struct less : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x < y; }
    };

    template<class T>
    struct greater_equal : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x >= y; }
    };

    template<class T>
    struct less_equal : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x <= y; }
    };

    template<class T>
    struct logical_and : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x && y; }
    };

    template<class T>
    struct logical_or : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x || y; }
    };

    template<class T>
    struct logical_not : public unary_function<T, bool> {
        bool operator()(const T &x) const { return !x; }
    };



}

#endif //STL_FUNCTIONAL_H
