//
// Created by jzh on 17-12-14.
// todo stream iterator

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
        typedef typename T::value_type value_type;
        typedef typename T::difference_type difference_type;
        typedef typename T::pointer pointer;
        typedef typename T::reference reference;
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
    Distance(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++n;
            ++first;
        }
        return n;
    }

    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    Distance(InputIterator first, InputIterator last, random_access_iterator_tag) {
        return last - first;
    }

    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
        //用category实例触发重载机制，在编译期间就根据迭代器类型确定合适的算法
        return Distance(first, last,typename iterator_traits<InputIterator>::iterator_category());
    }
/*------------------------distance-------------------------*/
/*------------------------advance-------------------------*/
    template<class InputIterator, class Distance>
    inline void advance(InputIterator &it, Distance n, input_iterator_tag) {
        while (n) {
            --n;
            ++it;
        }
    };

    template<class InputIterator, class Distance>
    inline void advance(InputIterator &it, Distance n, bidirectional_iterator_tag) {
        if (n >= 0) {
            while (n--)++it;
        } else {
            while (n++)--it;
        }
    };

    template<class InputIterator, class Distance>
    inline void advance(InputIterator &it, Distance n, random_access_iterator_tag) {
        it += n;
    };

    template<class InputIterator, class Distance>
    inline void advance(InputIterator &it, Distance n) {
        advance(it, n, iterator_category(it));
    };
/*------------------------advance-------------------------*/


    /*---------------------adaptor-------------------*/
    template <class Container>
    class back_insert_iterator {
    protected:
        Container* container;
    public:
        typedef Container          container_type;
        typedef output_iterator_tag iterator_category;
        typedef void                value_type;
        typedef void                difference_type;
        typedef void                pointer;
        typedef void                reference;

        explicit back_insert_iterator(Container& x) : container(&x) {}
        back_insert_iterator<Container>&
        operator=(const typename Container::value_type& value) {
            container->push_back(value);//把对迭代器的赋值操作转换成对容器的push_back操作
            return *this;
        }
        back_insert_iterator<Container>& operator*() { return *this; }
        back_insert_iterator<Container>& operator++() { return *this; }
        back_insert_iterator<Container>& operator++(int) { return *this; }
    };
    template <class Container>
    class front_insert_iterator {
    protected:
        Container* container;
    public:
        typedef Container          container_type;
        typedef output_iterator_tag iterator_category;
        typedef void                value_type;
        typedef void                difference_type;
        typedef void                pointer;
        typedef void                reference;

        explicit front_insert_iterator(Container& x) : container(&x) {}
        front_insert_iterator<Container>&
        operator=(const typename Container::value_type& value) {
            container->push_front(value);//把对迭代器的赋值操作转换成对容器的push_front操作
            return *this;
        }
        front_insert_iterator<Container>& operator*() { return *this; }
        front_insert_iterator<Container>& operator++() { return *this; }
        front_insert_iterator<Container>& operator++(int) { return *this; }
    };
    template <class Container>
    class insert_iterator {
    protected:
        Container* container;
        typename Container::iterator iter;
    public:
        typedef Container          container_type;
        typedef output_iterator_tag iterator_category;
        typedef void                value_type;
        typedef void                difference_type;
        typedef void                pointer;
        typedef void                reference;

        insert_iterator(Container& x, typename Container::iterator i)
                : container(&x), iter(i) {}
        insert_iterator<Container>&
        operator=(const typename Container::value_type& value) {
            iter = container->insert(iter, value);//在前面插入，并且后移iter，可以反复操作
            ++iter;
            return *this;
        }
        insert_iterator<Container>& operator*() { return *this; }
        insert_iterator<Container>& operator++() { return *this; }
        insert_iterator<Container>& operator++(int) { return *this; }
    };




    //反向迭代器,位置是原来的迭代器-1,
    //并把递增操作变递减，递减变递增
    template <class BidirectionalIterator, class T, class Reference = T&,
            class Distance = ptrdiff_t>
    class reverse_bidirectional_iterator {
        typedef reverse_bidirectional_iterator<BidirectionalIterator, T,
                Reference, Distance>  Self;
    protected:
        BidirectionalIterator current;
    public:
        typedef bidirectional_iterator_tag iterator_category;
        typedef T                        value_type;
        typedef Distance                  difference_type;
        typedef T*                       pointer;
        typedef Reference                 reference;

        reverse_bidirectional_iterator() {}
        explicit reverse_bidirectional_iterator(BidirectionalIterator x)
                : current(x) {}
        BidirectionalIterator base() const { return current; }
        Reference operator*() const {
            BidirectionalIterator tmp = current;
            return *--tmp;//解引用的操作返回的是原来位置-1！
        }
        pointer operator->() const { return &(operator*()); }
        Self& operator++() {
            --current;
            return *this;
        }
        Self operator++(int) {
            Self tmp = *this;
            --current;
            return tmp;
        }
        Self& operator--() {
            ++current;
            return *this;
        }
        Self operator--(int) {
            Self tmp = *this;
            ++current;
            return tmp;
        }
    };
    template <class _BiIter, class T, class Ref, class Distance>
    inline bool operator==(
            const reverse_bidirectional_iterator<_BiIter, T, Ref, Distance>& x,
            const reverse_bidirectional_iterator<_BiIter, T, Ref, Distance>& y)
    {
        return x.base() == y.base();
    }

    //随机迭代器的反向迭代器
    template <class RandomAccessIterator, class T, class Reference = T&,
            class Distance = ptrdiff_t>
    class reverse_iterator_base {
        typedef reverse_iterator_base<RandomAccessIterator, T, Reference, Distance>
                Self;
    protected:
        RandomAccessIterator current;
    public:
        typedef random_access_iterator_tag iterator_category;
        typedef T                        value_type;
        typedef Distance                  difference_type;
        typedef T*                       pointer;
        typedef Reference                 reference;

        reverse_iterator_base() {}
        explicit reverse_iterator_base(RandomAccessIterator x) : current(x) {}
        RandomAccessIterator base() const { return current; }
        Reference operator*() const { return *(current - 1); }
        pointer operator->() const { return &(operator*()); }
        Self& operator++() {
            --current;
            return *this;
        }
        Self operator++(int) {
            Self tmp = *this;
            --current;
            return tmp;
        }
        Self& operator--() {
            ++current;
            return *this;
        }
        Self operator--(int) {
            Self tmp = *this;
            ++current;
            return tmp;
        }
        Self operator+(Distance n) const {
            return Self(current - n);
        }
        Self& operator+=(Distance n) {
            current -= n;
            return *this;
        }
        Self operator-(Distance n) const {
            return Self(current + n);
        }
        Self& operator-=(Distance n) {
            current += n;
            return *this;
        }
        Reference operator[](Distance n) const { return *(*this + n); }
    };
    template <class RandomAccessIterator, class T,
            class Reference, class Distance>
    inline bool
    operator==(const reverse_iterator_base<RandomAccessIterator, T,
            Reference, Distance>& x,
               const reverse_iterator_base<RandomAccessIterator, T,
                       Reference, Distance>& y)
    {
        return x.base() == y.base();
    }

    template <class RandomAccessIterator, class T,
            class Reference, class Distance>
    inline bool
    operator!=(const reverse_iterator_base<RandomAccessIterator, T,
            Reference, Distance>& x,
               const reverse_iterator_base<RandomAccessIterator, T,
                       Reference, Distance>& y)
    {
        return !(x == y);
    }
    template <class RandomAccessIterator, class T,
            class Reference, class Distance>
    inline bool
    operator<(const reverse_iterator_base<RandomAccessIterator, T,
            Reference, Distance>& x,
              const reverse_iterator_base<RandomAccessIterator, T,
                      Reference, Distance>& y)
    {
        return y.base() < x.base();
    }
    template <class RandomAccessIterator, class T,
            class Reference, class Distance>
    inline Distance
    operator-(const reverse_iterator_base<RandomAccessIterator, T,
            Reference, Distance>& x,
              const reverse_iterator_base<RandomAccessIterator, T,
                      Reference, Distance>& y)
    {
        return y.base() - x.base();
    }

    template <class RandAccIter, class T, class Ref, class Dist>
    inline reverse_iterator_base<RandAccIter, T, Ref, Dist>
    operator+(Dist n,
              const reverse_iterator_base<RandAccIter, T, Ref, Dist>& x)
    {
        return reverse_iterator_base<RandAccIter, T, Ref, Dist>(x.base() - n);
    }
}


#endif //STL_STL_ITERATOR_H
