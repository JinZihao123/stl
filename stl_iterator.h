//
// Created by jzh on 17-12-14.
//

#ifndef STL_STL_ITERATOR_H
#define STL_STL_ITERATOR_H

#include <cstddef>  //ptrdiff_t
namespace mystl {

//五种迭代器类型
    struct input_iterator_tag {
    };
    struct output_iterator_tag {
    };
    struct forward_iterator_tag : public input_iterator_tag {
    };
    struct bidirectional_iterator_tag : public forward_iterator_tag {
    };
    struct random_access_iterator_tag : public bidirectional_iterator_tag {
    };

/*------------------------iterator-------------------------*/

//作为所有Iterator的基类，没有任何成员，只是申明了一些类型
//这样所有的迭代器都存有一份自己的相关类型的定义
//需要用到迭代器相关类型的地方就可以直接取用，唯一的问题是：
//原生指针也当迭代器用，但是他没法在内部定义相关类型
//解决方法就是iterator_traits,偏特化出针对原生指针的版本
    template<class Category, class T, class Distance = ptrdiff_t, class Pointer =T *, class Reference =T &>
    struct iterator {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Distance difference_type;
        typedef Pointer pointer;
        typedef Reference reference;

    };
/*------------------------iterator-------------------------*/


/*------------------------iterator_traits-------------------------*/
//迭代器使用的普通版本
    template<class T>
    struct iterator_traits {
        typedef typename T::iterator_category iterator_category;
        typedef typename T::T value_type;
        typedef typename T::Distance difference_type;
        typedef typename T::Pointer pointer;
        typedef typename T::Reference reference;
    };
//针对原生指针的两个偏特化版本
    template<class T>
    struct iterator_traits<T *> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T *pointer;
        typedef T &reference;
    };
    template<class T>
    struct iterator_traits<const T *> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T *pointer;
        typedef T &reference;
    };
/*------------------------iterator_traits-------------------------*/

//获取迭代器的category的一个实例,注意是实例不是指针，因为category就是用来当做参数触发重载的
    template<class Iterator>
    typename iterator_traits<Iterator>::iterator_category
    inline iterator_category(const Iterator &) {
        return typename iterator_traits<Iterator>::iterator_category();
    }

//获取迭代器的相关类型的指针，这里是指针不是实例了，因为用途不是仅仅是为了触发重载机制了
    template<class Iterator>
    inline typename iterator_traits<Iterator>::difference_type *
    difference_type(const Iterator &) {
        return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
    }

    template<class Iterator>
    inline typename iterator_traits<Iterator>::value_type *
    value_type(const Iterator &) {
        return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
    }


/*------------------------distance-------------------------*/
    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++n;
            ++first;
        }
        return n;
    }

    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, random_access_iterator_tag) {
        return last - first;
    }

    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
        //用category实例触发重载机制，在编译期间就根据迭代器类型确定合适的算法
        return __distance(first, last,typename iterator_traits<InputIterator>::iterator_category());
    }
/*------------------------distance-------------------------*/
/*------------------------advance-------------------------*/
    template<class InputIterator, class Distance>
    inline void __advance(InputIterator &it, Distance n, input_iterator_tag) {
        while (n) {
            --n;
            ++it;
        }
    };

    template<class InputIterator, class Distance>
    inline void __advance(InputIterator &it, Distance n, bidirectional_iterator_tag) {
        if (n >= 0) {
            while (n--)++it;
        } else {
            while (n++)--it;
        }
    };

    template<class InputIterator, class Distance>
    inline void __advance(InputIterator &it, Distance n, random_access_iterator_tag) {
        it += n;
    };

    template<class InputIterator, class Distance>
    inline void advance(InputIterator &it, Distance n) {
        __advance(it, n, iterator_category(it));
    };
/*------------------------advance-------------------------*/

}


#endif //STL_STL_ITERATOR_H
