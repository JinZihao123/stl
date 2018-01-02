//
// Created by jzh on 18-1-2.
// 未实现的功能：
// 自定义比较函数
// 捕获异常

#ifndef STL_PRIORITY_QUEUE_H
#define STL_PRIORITY_QUEUE_H

#include "heap.h"
#include "vector.h"
namespace mystl {

    template<class T, class Sequence = vector<T> >
    class priority_queue {
    public:
        typedef typename Sequence::value_type      value_type;
        typedef typename Sequence::size_type       size_type;
        typedef          Sequence                  container_type;

        typedef typename Sequence::reference       reference;
        typedef typename Sequence::const_reference const_reference;
    protected:
        Sequence c;
    public:
        priority_queue():c(){}

        template <class InputIterator>
        priority_queue(InputIterator first,InputIterator last):
                c(first,last){make_heap(c.begin(),c.end());}

        bool empty(){ return  c.empty();}

        size_type size(){ return c.size();}

        const_reference top(){ return c.front();}

        void push(const value_type & x){
            c.push_back(x);
            mystl::push_heap(c.begin(),c.end());
        }

        void pop(){
            pop_heap(c.begin(),c.end());
            c.pop_back() ;
        }

        void swap(priority_queue<T,Sequence> &x){
            c.swap(x.c);
        }
    };
}
#endif //STL_PRIORITY_QUEUE_H
