//
// Created by jzh on 18-1-2.
//
// 内部类heap

#ifndef STL_HEAP_H
#define STL_HEAP_H

#include "iterator.h"

namespace mystl {

    //向topIndex为根的堆插入value，holeIndex是堆最后一个空位的index，first只是起定位作用
    template <class RandomAccessIterator, class Distance, class T>
    void
    __push_heap(RandomAccessIterator first,
                Distance holeIndex, Distance topIndex, T value)
    {
        Distance parent = (holeIndex - 1) / 2;
        while (holeIndex > topIndex && *(first + parent) < value) {
            *(first + holeIndex) = *(first + parent);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = value;
    }

    template <class RandomAccessIterator, class Distance, class T>
    inline void
    push_heap_aux(RandomAccessIterator first,
                    RandomAccessIterator last, Distance*, T*)
    {
        //最后一个位置当做空洞，因为这里放置的是新进来还没调整的元素
        __push_heap(first, Distance((last - first) - 1), Distance(0),
                    T(*(last - 1)));
    }

    //前提是first和last之间已经包含了待插入的元素，last-1就是那个待插入的元素
    template<class RandomAccessIterator>
    inline void
    push_heap(RandomAccessIterator first, RandomAccessIterator last) {
        push_heap_aux(first, last,
                      difference_type (first), value_type(first));
    }
    
    //接受比较函数的版本
    template <class RandomAccessIterator, class Distance, class T,
            class Compare>
    void
    __push_heap(RandomAccessIterator first, Distance holeIndex,
                Distance topIndex, T value, Compare comp)
    {
        Distance parent = (holeIndex - 1) / 2;
        while (holeIndex > topIndex && comp(*(first + parent), value)) {
            *(first + holeIndex) = *(first + parent);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = value;
    }

    template <class RandomAccessIterator, class Compare,
            class Distance, class T>
    inline void
    push_heap_aux(RandomAccessIterator first,
                  RandomAccessIterator last, Compare comp,
                  Distance *, T *)
    {
        __push_heap(first, Distance((last - first) - 1), Distance(0),
                    T(*(last - 1)), comp);
    }

    template <class RandomAccessIterator, class Compare>
    inline void
    push_heap(RandomAccessIterator first, RandomAccessIterator last,
              Compare comp)
    {
        push_heap_aux(first, last, comp,
                      difference_type(first), value_type(first));
    }
/*----------------------------------------pop-------------------------------------*/
    //调整holdeIndex为根的树为堆，value为顶部的待调整元素的值，len为待调整的树的元素个数
    //前提是左子树和右子树都为堆
    template <class RandomAccessIterator, class Distance, class T>
    void
    adjust_heap(RandomAccessIterator first, Distance holeIndex,
                  Distance len, T value)
    {
        //stl源码剖析里给出的实现不太好，应该只需要下滤一次就可以了，重新实现一下 fixme
        Distance child,cur;
        for( cur = holeIndex;cur*2 +1<len;cur = child){
            child = cur *2+1;
            if(child!=len-1 && *(first + child) < *(first + child +1)){
                ++child;
            }
            if(value < *(first + child)){
                *(first + cur) = *(first + child);
            }else{
                break;
            }
        }
        *(first + cur) = value;
    }

    template <class RandomAccessIterator, class T, class Distance>
    inline void
    __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
               RandomAccessIterator result, T value, Distance*)
    {
        *result = *first;
        adjust_heap(first, Distance(0), Distance(last - first), value);
    }

    template <class RandomAccessIterator, class T>
    inline void
    pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
                   T*)
    {
        __pop_heap(first, last - 1, last - 1,
                   T(*(last - 1)), difference_type(first));
    }
    //仅仅是把first和last之间的数据做调整没有真正移除元素，first的元素移到了last-1的位置，剩余元素还是堆
    template <class RandomAccessIterator>
    inline void pop_heap(RandomAccessIterator first,
                         RandomAccessIterator last)
    {
        pop_heap_aux(first, last, value_type(first));
    }
    //接受比较器的版本
    template <class RandomAccessIterator, class Distance,
            class T, class Compare>
    void
    adjust_heap(RandomAccessIterator first, Distance holeIndex,
                  Distance len, T value, Compare comp)
    {        Distance child,cur;
        for( cur = holeIndex;cur*2 +1<len;cur = child){
            child = cur *2+1;
            if(child!=len-1 && comp(*(first + child),*(first + child +1))){
                ++child;
            }
            if(comp( value , *(first + child))){
                *(first + cur) = *(first + child);
            }else{
                break;
            }
        }
        *(first + cur) = value;
    }

    template <class RandomAccessIterator, class T, class Compare,
            class Distance>
    inline void
    __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
               RandomAccessIterator result, T value, Compare comp,
               Distance*)
    {
        *result = *first;
        adjust_heap(first, Distance(0), Distance(last - first),
                      value, comp);
    }

    template <class RandomAccessIterator, class T, class Compare>
    inline void
    pop_heap_aux(RandomAccessIterator first,
                   RandomAccessIterator last, T*, Compare comp)
    {
        __pop_heap(first, last - 1, last - 1, T(*(last - 1)), comp,
                   difference_type(first));
    }

    template <class RandomAccessIterator, class Compare>
    inline void
    pop_heap(RandomAccessIterator first,
             RandomAccessIterator last, Compare comp)
    {
        pop_heap_aux(first, last, value_type(first), comp);
    }


    template <class RandomAccessIterator, class T, class Distance>
    void
    __make_heap(RandomAccessIterator first,
                RandomAccessIterator last, T*, Distance*)
    {
        if (last - first < 2) return;
        Distance len = last - first;
        Distance parent = (len - 2)/2;

        while (true) {
            adjust_heap(first, parent, len, T(*(first + parent)));
            if (parent == 0) return;
            parent--;
        }
    }
    //将first和last调整成堆
    template <class RandomAccessIterator>
    inline void
    make_heap(RandomAccessIterator first, RandomAccessIterator last)
    {
        __make_heap(first, last,
                    value_type(first), value_type(first));
    }
    template <class RandomAccessIterator, class Compare,
            class T, class Distance>
    void
    __make_heap(RandomAccessIterator first, RandomAccessIterator last,
                Compare comp, T*, Distance*)
    {
        if (last - first < 2) return;
        Distance len = last - first;
        Distance parent = (len - 2)/2;

        while (true) {
            adjust_heap(first, parent, len, T(*(first + parent)),
                          comp);
            if (parent == 0) return;
            parent--;
        }
    }

    template <class RandomAccessIterator, class Compare>
    inline void
    make_heap(RandomAccessIterator first,
              RandomAccessIterator last, Compare comp)
    {
        __make_heap(first, last, comp,
                    value_type(first), difference_type(first));
    }
    //堆排序，因为pop_heap每次把最大元素放到容器尾端，但是不删除它，所以反复调用pop_heap可以排序
    template <class RandomAccessIterator>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
    {
        while (last - first > 1)
            pop_heap(first, last--);
    }

    template <class RandomAccessIterator, class Compare>
    void
    sort_heap(RandomAccessIterator first,
              RandomAccessIterator last, Compare comp)
    {
        while (last - first > 1)
            pop_heap(first, last--, comp);
    }
}
#endif //STL_HEAP_H
