//
// Created by jzh on 18-1-2.
// todo：
// 捕获异常

#ifndef STL_PRIORITY_QUEUE_H
#define STL_PRIORITY_QUEUE_H

#include "heap.h"
#include "vector.h"
#include "functional.h"
namespace mystl {

    template<class T, class Sequence = vector<T> ,class Compare = less<typename  Sequence::value_type> >
    class priority_queue {
    public:
        typedef typename Sequence::value_type      value_type;
        typedef typename Sequence::size_type       size_type;
        typedef          Sequence                  container_type;

        typedef typename Sequence::reference       reference;
        typedef typename Sequence::const_reference const_reference;
    protected:
        Sequence c;
        Compare comp;
    public:
        priority_queue():c(){}

        explicit priority_queue(const Compare& cmp):c(),comp(cmp){};

        template <class InputIterator>
        priority_queue(InputIterator first,InputIterator last):
                c(first,last){make_heap(c.begin(),c.end());}

        priority_queue(const value_type* first, const value_type* last,
                       const Compare& x)
                : c(first, last), comp(x)
        { make_heap(c.begin(), c.end(), comp); }

        bool empty(){ return  c.empty();}

        size_type size(){ return c.size();}

        const_reference top(){ return c.front();}

        void push(const value_type & x){
            c.push_back(x);
            mystl::push_heap(c.begin(),c.end(),comp);
        }

        void pop(){
            mystl::pop_heap(c.begin(),c.end(),comp);
            c.pop_back() ;
        }

        void swap(priority_queue<T,Sequence> &x){
            c.swap(x.c);
        }
    };
}
#endif //STL_PRIORITY_QUEUE_H
