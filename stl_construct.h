//
// Created by jzh on 17-12-15.
//

#ifndef STL_STL_CONSTRUCT_H
#define STL_STL_CONSTRUCT_H

#include <new>
#include "type_traits.h"
#include "stl_iterator.h"


template <class T1,class T2>
inline void construct(T1* p ,const T2& t){
    new(p) T1(t);
};
/*--------------------------------destroy--------------------------------*/
template <class T>
inline void destroy(T*p){
    p->~T();
}
//下面是接受一对迭代器的批量destroy，用到迭代器相关类型的traits方法推断值的类型
//根据__type_traits<T>::has_trivial_destructor的结果，触发重载函数来提高__true_type的析构效率
template <class ForwardIteror>
inline void destroy(ForwardIteror first,ForwardIteror last){
    __destroy(first,last,value_type(first)/*迭代器相关类型*/);
}
template <class ForwardIteror,class T>
inline void __destroy(ForwardIteror first,ForwardIteror last,T* ){
    __destroy_aux(first,last,__type_traits<T>::has_trivial_destructor());
}
template <class ForwardIteror>
inline void __destroy_aux(ForwardIteror first,ForwardIteror last,__true_type ){
    //什么都不做
}
template <class ForwardIteror>
inline void __destroy_aux(ForwardIteror first,ForwardIteror last,__false_type ){
    while(first!=last){
        destroy(&*first);
        ++first;
    }
}
inline void destroy(char*,char*){};
inline void destroy(wchar_t*,wchar_t*){};
/*--------------------------------destroy--------------------------------*/

#endif //STL_STL_CONSTRUCT_H
