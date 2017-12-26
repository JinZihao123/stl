//
// Created by jzh on 17-12-26.
//

#ifndef STL_STL_ALGOBASE_H
#define STL_STL_ALGOBASE_H

#include <cstring>
#include "stl_iterator.h"
#include "type_traits.h"
namespace mystl {
/*-----------------------------------copy-----------------------------------*/

    template<class RandomAccessIterator, class OutputIterator, class Distance>
    inline OutputIterator
    __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance *) {
        for (Distance n = last - first; n > 0; --n, ++result, ++first) {
            *result = *first;
        }
        return result;
    }

    template<class T>
    inline T *copy_t(const T *first, const T *last, T *result, __false_type) {
        return __copy_d(first, last, result, (ptrdiff_t *) 0);
    }

    template<class T>
    inline T *copy_t(const T *first,const T *last, T *result, __true_type) {
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }


    template<class RandomAccessIterator, class OutputIterator>
    inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result,
                                       mystl::random_access_iterator_tag) {
        typedef typename mystl::iterator_traits<RandomAccessIterator>::difference_type diff_type;
        return __copy_d(first, last, result, (diff_type *) 0);
    }

    template<class OutputIterator, class InputIterator>
    inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result,
                                 mystl::input_iterator_tag) {
        for (; first != last; ++result, ++first) {
            *result = *first;
        }
        return result;
    }

//__copy_dispatch在源码中是一个重载了（）的类，而不是函数
    //因为函数不能偏特化，类才可以
    template<class InputIterator, class OutputIterator>
    struct __copy_dispatch{
        OutputIterator operator()
                (InputIterator first, InputIterator last, OutputIterator result) {
            return __copy(first, last, result, mystl::iterator_category(first));
        };
    };


    template<class T>
    struct __copy_dispatch<const T *, T *>{
        T *operator()
                (const T *first, const T *last, T *result) {
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;
            return copy_t(first, last, result, t());
        }

    };

    template<class T>
    struct __copy_dispatch<T *, T *>{
        T *operator()(T *first, T *last, T *result) {
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;
            return copy_t(first, last, result, t());
        }
    };


    template<class InputIterator, class OutputIterator>
    inline InputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        return __copy_dispatch<InputIterator,  OutputIterator>()(first, last, result);
    };
    inline char* copy(const char* first,const char * last,char *result){
        memmove(result,first,(last-first));
        return result +(last - first);
    }
    inline wchar_t* copy(const wchar_t * first,const wchar_t * last,wchar_t *result){
        memmove(result,first,(last-first));
        return result +(last - first);
    }


/*-----------------------------------copy-----------------------------------*/
/*-----------------------------------copy_backward-----------------------------------*/


    template<class RandomAccessIterator,class BidirectionalIter,class Distance>
    inline BidirectionalIter copy_backward_d
            (RandomAccessIterator first, RandomAccessIterator last, BidirectionalIter result, Distance *) {
        for(Distance n = last - first; n>0;--n){
            *--result = *--last;
        }
        return  result;
    }
    template<class T>
    inline T *copy_backward_t(const T *first, const T *last, T *result, __true_type) {
        const ptrdiff_t  Num = last - first;
        memmove(result - Num, first, sizeof(T) * Num);
        return result - Num;
    }
    template<class T>
    inline T *copy_backward_t(const T *first, const T *last, T *result, __false_type) {
        return copy_backward_d(first,last,result,(ptrdiff_t*)0);
    }
    template<class RandomAccessIterator, class BidirectionalIter>
    inline BidirectionalIter __copy_backward(RandomAccessIterator first, RandomAccessIterator last, BidirectionalIter result,
                                       mystl::random_access_iterator_tag) {
        typedef typename mystl::iterator_traits<RandomAccessIterator>::difference_type diff_type;
        return copy_backward_d(first, last, result, (diff_type *) 0);
    }

    template<class BidirectionalIter1, class BidirectionalIter2>
    inline BidirectionalIter2 __copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result,
                                 mystl::bidirectional_iterator_tag) {
        while(first!=last){
            *--result = *--last;
        }
        return result;
    }
    template<class BidirectionalIter1, class BidirectionalIter2>
    struct __copy_backward_dispatch{
        BidirectionalIter2 operator()
                (BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
            return __copy_backward(first, last, result, mystl::iterator_category(first));
        }
    };

    template<class T>
    struct __copy_backward_dispatch<const T *, T *>{
        T *operator()(const T *first, const T *last, T *result) {
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;
            return copy_backward_t(first, last, result, t());
        }
    };
    template<class T>
    struct __copy_backward_dispatch<T *, T *>{
        T *operator()(T *first, T *last, T *result) {
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;
            return copy_backward_t(first, last, result, t());
        }
    };

    template<class BidirectionalIter1, class BidirectionalIter2>
    inline BidirectionalIter2 copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
        return __copy_backward_dispatch<BidirectionalIter1,BidirectionalIter2>()(first, last, result);
    };
    inline char * copy_backward(const char *first,const char *last,char*result){
        auto num = last - first;
        memmove(result - num,first,num);
        return result -num;
    }
    inline wchar_t * copy_backward(const wchar_t *first,const wchar_t *last,wchar_t*result){
        auto num = last - first;
        memmove(result - num,first,num);
        return result -num;
    }
/*-----------------------------------copy_backward-----------------------------------*/
/*-----------------------------------equal-----------------------------------*/

    template <class InputIterator1,class InputIterator2>
    inline bool equal(InputIterator1 first1,InputIterator1 last1,InputIterator2 first2){
        for(;first1!=last1;++first1,++first2){
            if(*first1!=*first2){
                return false;
            }
        }
        return true;
    };
    template <class InputIterator1,class InputIterator2,class BinaryPredicate>
    inline bool equal(InputIterator1 first1,InputIterator1 last1,InputIterator2 first2,BinaryPredicate pre ){
        for(;first1!=last1;++first1,++first2){
            if(!pre(*first1,*first2)){
                return false;
            }
        }
        return true;
    };
/*-----------------------------------equal-----------------------------------*/
/*-----------------------------------fill/fill_n-----------------------------------*/
    template <class ForwardIterator,class T>
    inline void fill(ForwardIterator first,ForwardIterator last,const T& value){
        for(;first!=last;++first){
            *first = value;
        }
    };
    template <class ForwardIterator ,class Size,class T>
    inline ForwardIterator fill_n(ForwardIterator first,Size n,const T& value){
        for(;n>0;--n,++first){
            *first = value;
        }
        return  first;
    };

/*-----------------------------------fill/fill_n-----------------------------------*/
    template <class Iterator1,class Iterator2>
    inline void iter_swap(Iterator1 it1,Iterator2 it2){
        typename iterator_traits<Iterator1>::value_type tmp = *it1;
        *it1 = *it2;
        *it2 = tmp;
    };template <class T>
    inline const T& max(const T&a,const T& b){
        return  a<b? b:a;
    };
    template <class T,class Compare>
    inline const T& max(const T&a,const T& b,Compare cmp){
        return  cmp(a,b)? b:a;
    };
    template <class T>
    inline const T& min(const T&a,const T& b){
        return  b<a? b:a;
    };
    template <class T,class Compare>
    inline const T& min(const T&a,const T& b,Compare cmp){
        return  cmp(b,a)? b:a;
    };

    template <class T>
    inline void swap(T&a,T&b){
        T tmp =a;
        a=b;
        b=tmp;
    }




}


#endif //STL_STL_ALGOBASE_H
