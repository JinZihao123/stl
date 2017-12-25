//
// Created by jzh on 17-12-19.
//

#ifndef STL_STL_UNINITIALIZED_H
#define STL_STL_UNINITIALIZED_H


#include <cstring>
#include "stl_iterator.h"
#include "type_traits.h"
#include "stl_construct.h"
namespace mystl {
    template<class ForwardIterator, class Size, class T, class T1>
    ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T &x, T1 *);

    template<class ForwardIterator, class Size, class T>
    ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __false_type);

    template<class ForwardIterator, class Size, class T>
    ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __true_type);

    template<class ForwardIterator, class InputIterator, typename T>
    ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T *type);

    template<class ForwardIterator, class InputIterator>
    ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type);

    template<class ForwardIterator, class InputIterator>
    ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type);

    template<class ForwardIteraor, class T>
    ForwardIteraor __uninitialized_fill(ForwardIteraor first, ForwardIteraor last, const T &x, T *type);

    template<class ForwardIteraor, class T>
    ForwardIteraor __uninitialized_fill_aux(ForwardIteraor first, ForwardIteraor last, const T &x, __false_type);

    template<class ForwardIteraor, class T>
    ForwardIteraor __uninitialized_fill_aux(ForwardIteraor first, ForwardIteraor last, const T &x, __true_type);

/*------------------------------------uninitialized_fill_n-----------------------------*/

    template<class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __true_type) {
        return std::fill_n(first, n, x);//TODO 用的STL算法
    };

    template<class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __false_type) {
        ForwardIterator cur = first;
        for (; n > 0; --n, ++cur) {
            construct(&*cur, x);
        }
        return cur;
    };
    template<class ForwardIterator, class Size, class T, class T1>
    inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T &x, T1 *) {
        typedef typename __type_traits<T1>::is_POD_type is_POD;
        return __uninitialized_fill_n_aux(first, n, x, is_POD());
    }
    template<class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &x) {
        return __uninitialized_fill_n(first, n, x, value_type(first));
    };

/*------------------------------------uninitialized_fill_n-----------------------------*/
/*------------------------------------uninitialized_copy-----------------------------*/

    template<class ForwardIterator, class InputIterator>
    ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
        ForwardIterator cur = result;
        for (; first != last; ++cur, ++first) {
            construct(&*cur, *first);
        }
        return cur;
    }

    template<class ForwardIterator, class InputIterator>
    ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
        std::copy(first, last, result);//TODO 用了stl算法
    }

    template<class ForwardIterator, class InputIterator, typename T>
    ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T *) {
        return __uninitialized_copy_aux(first, last, result,typename __type_traits<T>::is_POD_type());
    }
    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {

        return __uninitialized_copy(first, last, result, value_type(result));
    }
//为char和wchar写一份特化版本，用memmove来复制
    inline char *uninitialized_copy(const char *first, const char *last, char *result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

/*------------------------------------uninitialized_copy-----------------------------*/
/*------------------------------------uninitialized_fill-----------------------------*/
    template<class ForwardIteraor, class T>
    inline void uninitialized_fill(ForwardIteraor first, ForwardIteraor last, const T &x) {
        __uninitialized_fill(first, last, x, value_type(x));
    }

    template<class ForwardIteraor, class T>
    inline void __uninitialized_fill(ForwardIteraor first, ForwardIteraor last, const T &x, T *) {
        __uninitialized_fill_aux(first, last, x, __type_traits<T>::is_POD_type());
    }

    template<class ForwardIteraor, class T>
    inline void __uninitialized_fill_aux(ForwardIteraor first, ForwardIteraor last, const T &x, __false_type) {
        ForwardIteraor cur = first;
        for (; cur != last; ++cur) {
            construct(&*cur, x);
        }
    };

    template<class ForwardIteraor, class T>
    inline void __uninitialized_fill_aux(ForwardIteraor first, ForwardIteraor last, const T &x, __true_type) {
        std::fill(first, last, x);//TODO 用了stl算法
    };

/*------------------------------------uninitialized_fill-----------------------------*/
}

#endif //STL_STL_UNINITIALIZED_H
