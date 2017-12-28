//
// Created by jzh on 17-12-21.
//

#ifndef STL_STL_VECTOR_H
#define STL_STL_VECTOR_H

#include "iterator.h"
#include "alloc.h"
#include "construct.h"
#include "uninitialized.h"
#include "algobase.h"
namespace mystl {
    template<class T, class Alloc = alloc>
    class vector {
    public:
        typedef T                   value_type;
        typedef value_type *        pointer;
        typedef const value_type *  const_pointer;
        typedef value_type *        iterator;                 // vecotr的迭代器是原生指针
        typedef const value_type *  const_iterator;
        typedef value_type &        reference;
        typedef const value_type &  const_reference;
        typedef size_t              size_type;
        typedef ptrdiff_t           difference_type;
    protected:
        typedef simple_alloc<T, Alloc> data_allocator;
        iterator start;               // 内存空间起始点
        iterator finish;              // 当前使用的内存空间结束点
        iterator end_of_storage;      // 实际分配内存空间的结束点
        void insert_aux(iterator position, const T &x);

        void deallocate();

        template<class ForwardIterator>
        iterator allocate_and_copy(size_type n, ForwardIterator first, ForwardIterator last);

        iterator allocate_and_copy(size_type n, const_iterator it1, const_iterator it2);

        iterator allocate_and_fill(size_type n, const T &x);
/*------------------------------------initialize------------------------------------*/
//fill_initialize
        //分配并初始化内存
        void fill_initialize(size_type n, const T &value);

//range_initialize
        template <class _Integral>
        void range_initialize_aux(_Integral first,_Integral last,__true_type);
        template <class _Integral>
        void range_initialize_aux(_Integral first,_Integral last,__false_type);
        template<class InputIterator>
        void range_initialize(InputIterator first, InputIterator last, input_iterator_tag);

        template<class ForwardIterator>
        void range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag);
/*------------------------------------initialize------------------------------------*/
/*-----------------------------------range_insert------------------------------------*/
// 在指定位置插入指定区间的对象
        template <class _Integral>
        void range_insert_aux(iterator pos,_Integral n,_Integral value,__true_type);
        template <class Iterator>
        void range_insert_aux(iterator pos,Iterator first,Iterator last,__false_type);
        //input_iterator_tag类型迭代器的版本，一个一个调用insert插入
        //复杂度是插入的数目n，容器原有的数量size，O(n*size),极慢
        template<class InputIterator>
        void range_insert(iterator pos, InputIterator first, InputIterator last, input_iterator_tag);

        template<class ForwardIterator>
        void range_insert(iterator position, ForwardIterator first, ForwardIterator last, forward_iterator_tag);
/*-----------------------------------range_insert------------------------------------*/



    public:
        iterator begin() { return start; }

        const_iterator cbegin() const { return start; }

        iterator end() { return finish; }

        const_iterator cend() const { return finish; }

        size_type size() const { return size_type(finish - start); }

        size_type capacity() const { return size_type(end_of_storage - start); }

        bool empty() const { return start == finish; }

        reference operator[](size_type n) { return *(start + n); };

        pointer data();

/*------------------------------------initialize------------------------------------*/

        vector() : start(0), finish(0), end_of_storage(0) {}

        vector(size_type n, const T &value) { fill_initialize(n, value); }

        explicit vector(size_type n) { fill_initialize(n, T()); }

        vector(const vector<T, Alloc> &x);

        vector(const_iterator first, const_iterator last);

        template <class InputIterator>
        vector(InputIterator first, InputIterator last);

        ~vector() {
            // 析构对象
            destroy(start, finish);
            // 释放内存
            deallocate();
        }
/*------------------------------------initialize------------------------------------*/
        //重新设置容量
        void reserve(size_type n);

        reference front() { return *start; }

        const_reference front() const { return *start; }

        reference back() { return *(finish - 1); }

        const_reference back() const { return *(finish - 1); }

        void push_back(const T &x);

        void pop_back();//对空的vec调用pop_back是未定义的行为，非常危险
        //交换两个vector, 实际上是交换内部的状态指针
        void swap(vector<T, Alloc> &x);

        //insert函数是重点
        iterator insert(iterator position, const T &x);

        template<class InputIterator>
        void insert(iterator position, InputIterator first, InputIterator last);

        void insert(iterator position, size_type n, const T &x);

        iterator erase(iterator first, iterator last);

        iterator erase(iterator position);

        void clear();

    };


    template<class T, class Alloc>
    void vector<T, Alloc>::insert_aux(iterator position, const T &x) {
        if (finish != end_of_storage) {       // 还有剩余内存
            construct(finish, *(finish - 1));
            ++finish;
            T x_copy = x;
            mystl::copy_backward(position, finish - 2, finish - 1);
            *position = x_copy;
        } else {        // 内存不足, 需要重新分配
            // 本实作中是按原内存2倍进行重新分配
            const size_type old_size = size();
            const size_type len = old_size != 0 ? 2 * old_size : 1;
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            // 将内存重新配置
            new_finish = mystl::uninitialized_copy(start, position, new_start);
            construct(new_finish, x);
            ++new_finish;
            new_finish = mystl::uninitialized_copy(position, finish, new_finish);
            // 析构原容器中的对象
            destroy(begin(), end());
            // 释放原容器分配的内存
            deallocate();
            // 调整内存指针状态
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::deallocate() {
        if (start) {
            //vector分配和释放内存全部通过data_allocator进行
            data_allocator::deallocate(start, end_of_storage - start);
        }
    }

    template<class T, class Alloc>
    template<class ForwardIterator>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::allocate_and_copy
            (vector::size_type n, ForwardIterator first, ForwardIterator last) {
        iterator result = data_allocator::allocate(n);
        mystl::uninitialized_copy(first, last, result);
        return result;
    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator
     vector<T, Alloc>::allocate_and_copy
            (vector::size_type n, vector::const_iterator it1, vector::const_iterator it2) {
        iterator result = data_allocator::allocate(n);
        uninitialized_copy(it1, it2, result);
        return result;
    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::allocate_and_fill
            (vector::size_type n, const T &x) {
        iterator result = data_allocator::allocate(n);
        mystl::uninitialized_fill_n(result, n, x);
        return result;
    }
/*------------------------------------initialize------------------------------------*/
//fill_initialize
//分配并初始化内存
    template<class T, class Alloc>
    inline void vector<T, Alloc>::fill_initialize(vector::size_type n, const T &value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

//range_initialize

    template<class T, class Alloc>
    template <class _Integer>
    void vector<T, Alloc>::range_initialize_aux(_Integer n, _Integer value, __true_type) {
        //构造函数的两个参数是int类型，说明不是要迭代器初始化，而是n个value初始化
        start = allocate_and_fill( n,value);
        finish = start + n;
        end_of_storage = finish;
    }
    template<class T, class Alloc>
    template <class _InputIerator>
    void vector<T, Alloc>::range_initialize_aux(_InputIerator _first, _InputIerator _last, __false_type) {
        //这里是构造函数的两个参数确定是迭代器了！
        //分开处理是iterator是input类型和不是input类型的情况
        range_initialize(_first,_last,iterator_category(_first));
    }


    template<class T, class Alloc>
    template<class InputIterator>
    inline void vector<T, Alloc>::range_initialize(InputIterator first, InputIterator last, input_iterator_tag) {
        for (; first != last; ++first)
            //之所以要一个一个push back是因为input interator求distance（first，last）需要一个一个递增
            //而input iterator只能读一次，也就是求完距离就再也读不到要读的内容了，所以只能直接pushback，读一次就构造好
            //这么做可能会多allocate几次，也不能精确控制容量了，但是没有办法，只能这样了
            push_back(*first);
    }

    template<class T, class Alloc>
    template<class ForwardIterator>
    inline void vector<T, Alloc>::range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
        //不是input iteraotr就可以用distance求first和last的距离了
        size_type n = static_cast<size_type> (mystl::distance(first, last));
        start = allocate_and_copy(n, first, last);
        finish = start + n;
        end_of_storage = finish;
    }

/*------------------------------------initialize------------------------------------*/


/*-----------------------------------range_insert------------------------------------*/
    template<class T, class Alloc>
    template<class _Integral>
    inline void vector<T, Alloc>::range_insert_aux
            (vector<T, Alloc>::iterator pos, _Integral n, _Integral value, __true_type) {
        //__true_type对应参数是非迭代器，而是n个value的情况
        insert(pos, static_cast<vector<T, Alloc>::size_type>(n), static_cast<T>(value));
    }
    template<class T, class Alloc>
    template<class Iterator>
    void vector<T, Alloc>::range_insert_aux(vector::iterator pos, Iterator first, Iterator last, __false_type) {
        //__false_type对应参数是迭代器的情况
        range_insert(pos,first,last,iterator_category(first));
    }

    // 在指定位置插入指定区间的对象
//input_iterator_tag类型迭代器的版本，一个一个调用insert插入
//复杂度是插入的数目n，容器原有的数量size，O(n*size),极慢
    template<class T, class Alloc>
    template<class InputIterator>
    inline void vector<T, Alloc>::range_insert
            (vector::iterator pos, InputIterator first, InputIterator last, input_iterator_tag) {
        for (; first != last; ++first) {
            pos = insert(pos, *first);
            ++pos;
        }
    }

    template<class T, class Alloc>
    template<class ForwardIterator>
    inline void vector<T, Alloc>::range_insert(vector::iterator position, ForwardIterator first, ForwardIterator last,
                                               forward_iterator_tag) {
        if (first != last) {
            size_type n = static_cast<size_type >(distance(first, last));
            if (size_type(end_of_storage - finish) >= n) {
                //备用空间足够的情况
                const size_type elems_after = finish - position;
                iterator old_finish = finish;
                if (elems_after > n) {
                    //关键的操作，在为初始化的内存上必须调用uninitialized_copy创建对象
                    //因为非POD类型的copy construction和assignment = 创建对象的动作很可能是不一样的
                    //copy调用的是assignment = ，
                    // 而uninitialized_copy会根据是否是POD来决定是逐个调用copy construction还是直接copy
                    //例如string类的assignment = ，会析构原来的char*，再构造新的
                    //这种类就必须用copy construction构造，如果直接调用copy就会析构未初始化的内存！
                    uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    mystl::copy_backward(position, old_finish - n, old_finish);
                    mystl::copy(first, last, position);
                } else {
                    ForwardIterator mid = first;
                    advance(mid, elems_after);
                    uninitialized_copy(mid, last, finish);
                    finish += n - elems_after;
                    uninitialized_copy(position, old_finish, finish);
                    finish += elems_after;
                    mystl::copy(first, mid, position);
                }
            } else {
                const size_type old_size = size();
                const size_type len = old_size + mystl::max(old_size, n);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                new_finish = uninitialized_copy(start, position, new_start);
                new_finish = uninitialized_copy(first, last, new_finish);
                new_finish = uninitialized_copy(position, finish, new_finish);
                destroy(start, finish);
                deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
        }
    }

/*-----------------------------------range_insert------------------------------------*/

    template<class T, class Alloc>
    inline vector<T, Alloc>::vector(const vector<T, Alloc> &x) {
        start = allocate_and_copy(x.cend() - x.cbegin(), x.cbegin(), x.cend());
        finish = start + (x.cend() - x.cbegin());
        end_of_storage = finish;
    }

    template<class T, class Alloc>
    inline vector<T, Alloc>::vector(vector<T, Alloc>::const_iterator first, vector<T, Alloc>::const_iterator last) {
        size_type n = static_cast<size_type>(distance(first, last));
        start = allocate_and_copy(n, first, last);
        finish = start + n;
        end_of_storage = finish;
    }


    template<class T, class Alloc>
    template <class InputIterator>
    inline vector<T, Alloc>::vector(InputIterator first, InputIterator last) :
            start(0), finish(0), end_of_storage(0) {
        //首先要判断InputIterator是不是int类型！
        //如果是int类型，说明这两个参数不是迭代器！而是用first个last来初始化vector！
        typedef typename _Is_integer<InputIterator>::_Integral isIntegral;
        range_initialize_aux(first,last,isIntegral());
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::reserve(vector::size_type n) {
        if (capacity() < n) {
            const size_type old_size = size();
            iterator tmp = allocate_and_copy(n, start, finish);
            destroy(start, finish);
            deallocate();
            start = tmp;
            finish = tmp + old_size;
            end_of_storage = start + n;
        }
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::push_back(const T &x) {
        // 内存满足条件则直接追加元素, 否则需要重新分配内存空间
        if (finish != end_of_storage) {
            construct(finish, x);
            ++finish;
        } else
            insert_aux(end(), x);
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::swap(vector<T, Alloc> &x) {
        mystl::swap(start, x.start);
        mystl::swap(finish, x.finish);
        mystl::swap(end_of_storage, x.end_of_storage);
    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(vector::iterator position, const T &x) {
        size_type n = position - begin();
        if (finish != end_of_storage && position == end()) {
            construct(finish, x);
            ++finish;
        } else
            insert_aux(position, x);
        return begin() + n;
    }

    template<class T, class Alloc>
    template<class InputIterator>
    inline void vector<T, Alloc>::insert(vector::iterator position, InputIterator first, InputIterator last) {
        //迭代器范围插入也要先检查输入参数是否是int，如果是int说明其实输入的不是迭代器
        //而是first个last这种形式！
        typedef typename _Is_integer<InputIterator>::_Integral isIntegral;
        range_insert_aux(position,first,last,isIntegral());
    }

    template<class T, class Alloc>
    void vector<T, Alloc>::insert(vector::iterator position, vector::size_type n, const T &x) {
        // 如果n为0则不进行任何操作
        if (n != 0) {
            if (size_type(end_of_storage - finish) >= n) {      // 剩下的内存够分配
                T x_copy = x;
                const size_type elems_after = finish - position;
                iterator old_finish = finish;
                if (elems_after > n) {
                    uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    mystl::copy_backward(position, old_finish - n, old_finish);
                    mystl::fill(position, position + n, x_copy);
                } else {
                    uninitialized_fill_n(finish, n - elems_after, x_copy);
                    finish += n - elems_after;
                    uninitialized_copy(position, old_finish, finish);
                    finish += elems_after;
                    mystl::fill(position, old_finish, x_copy);
                }
            } else {      // 剩下的内存不够分配, 需要重新分配
                const size_type old_size = size();
                const size_type len = old_size + mystl::max(old_size, n);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                new_finish = uninitialized_copy(start, position, new_start);
                new_finish = uninitialized_fill_n(new_finish, n, x);
                new_finish = uninitialized_copy(position, finish, new_finish);
                destroy(start, finish);
                deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
        }
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::pop_back() {
        --finish;
        destroy(finish);
    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(vector::iterator first, vector::iterator last) {
        iterator i = mystl::copy(last, finish, first);
        destroy(i, finish);
        finish = finish - (last - first);
        return first;

    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(vector::iterator position) {
        if (position + 1 != cend()) {
            mystl::copy(position + 1, finish, position);
        }
        --finish;
        destroy(finish);
        return position;
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::clear() {
        erase(begin(), end());
    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::pointer vector<T, Alloc>::data() {
        return start;
    }



    template<class T, class Alloc>
    inline bool operator==(const vector<T, Alloc> &v1, const vector<T, Alloc> &v2) {
        return v1.size() == v2.size() &&
                mystl::equal(v1.cbegin(),v1.cend(),v2.cbegin());
    }
    template<class T, class Alloc>
    inline bool operator!=(const vector<T, Alloc> &v1, const vector<T, Alloc> &v2) {
        return !(v1==v2);
    }
}

#endif //STL_STL_VECTOR_H
