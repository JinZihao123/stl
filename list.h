//
// Created by jzh on 17-12-27.
//

#ifndef STL_LIST_H
#define STL_LIST_H

#include "iterator.h"
#include "alloc.h"
#include "construct.h"
namespace mystl {

    template<class T>
    struct __list_node {
        __list_node<T> *next;
        __list_node<T> *prev;
        T data;
    };

    template<class Item>
    struct list_iterator : public mystl::iterator<mystl::bidirectional_iterator_tag, Item> {
        //List的迭代器继承自mystl::iterator，确保包含迭代器的五种特性
        typedef list_iterator<Item>             iterator;
        typedef list_iterator<Item>             self;
        typedef bidirectional_iterator_tag      iterator_category;
        typedef Item                            value_type;
        typedef Item*                           pointer;
        typedef Item&                           reference;
        typedef ptrdiff_t                       difference_type;
        typedef __list_node<Item> *             point_to_node;
        typedef size_t                          size_type;

        point_to_node node;

        //constructor
        list_iterator() {};
        list_iterator(point_to_node x) : node(x) {};
        list_iterator(const iterator &x) : node(x.node) {};
        bool operator==(const self &x) const { return node == x.node; }
        bool operator!=(const self &x) const { return node != x.node; }
        reference operator*() const { return node->data; }
        pointer operator->() const { return &(operator*()); }
        //前置和后置++操作的标准写法
        self &operator++() {
            node = node->next;
            return *this;
        }
        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }
        self &operator--() {
            node = node->prev;
            return *this;
        }
        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }
    };


    template<class T, class Alloc = alloc>
    class list{
    protected:
        typedef __list_node<T> list_node;
        typedef simple_alloc<list_node,Alloc> list_node_allocator;
    public:
        typedef list_iterator<T> iterator;
        typedef list_node*  point_to_node;
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
    protected:
        point_to_node node;//list只需要自己持有一个node指针就足够了，因为是环状双向链表
    protected:
        point_to_node get_node(){ return list_node_allocator::allocate();}
        void put_node(point_to_node p){list_node_allocator::deallocate(p);}
        point_to_node creat_node(const T& x){
            point_to_node result = get_node();
            construct(&result->data,x);
            return result;
        }
        void destroy_node(point_to_node x){
            destroy(&x->data);
            put_node(x);
        }
        void empty_initialize(){
            node = get_node();
            node->next=node;
            node->prev=node;
        }
        //把first和last的区间移动到pos前面，pos不允许在first和last之间
        void transfer(iterator pos,iterator first,iterator last){
            if(pos!=last){
                last.node->prev->next = pos.node;
                first.node->prev->next = last.node;
                pos.node->prev->next = first.node;
                point_to_node tmp = pos.node->prev;
                pos.node->prev = last.node->prev;
                last.node->prev = first.node->prev;
                first.node->prev = tmp;
            }
        }


        template< class InputIterator>
        void __insert_dispatch(iterator iterator, InputIterator first, InputIterator last, __true_type) ;
        template< class InputIterator>
        void __insert_dispatch(iterator iterator, InputIterator first, InputIterator last, __false_type);
    public:
        iterator begin()const{ return iterator(node->next);}
        iterator end()const{ return iterator(node);}
        bool empty()const{ return node->next == node;}
        size_type size()const{
            auto result = mystl::distance(begin(),end());
            return result;
        }
        reference front(){ return *begin();}
        reference back(){ return *(--end());}
    public:

        iterator insert(iterator pos,const T&x){
            //所有的inser最终都应该调用这个版本进行最后的实际插入
            point_to_node new_node = creat_node(x);
            new_node->next = pos.node;
            new_node->prev= pos.node->prev;
            pos.node->prev->next = new_node;
            pos.node->prev = new_node;
            return new_node;
        }
        void insert(iterator pos,size_type n, const_reference value){
            for ( ; n > 0; --n)
                insert(pos, value);
        }
        template <class InputIterator>
        void insert(iterator pos,InputIterator first,InputIterator last){
            typedef typename _Is_integer<InputIterator>::_Integral isInt;
            __insert_dispatch(pos,first,last,isInt());
        }


        void push_back(const T&x){ insert(end(),x); }
        void push_front(const T&x){insert(begin(),x);}
        iterator erase(iterator pos){
            point_to_node prev_node=pos.node->prev;
            point_to_node next_node=pos.node->next;
            prev_node->next = next_node;
            next_node->prev = prev_node;
            destroy_node(pos.node);
            return iterator(next_node);
        }
        iterator erase(iterator first,iterator last){
            while(first!=last){
                first = erase(first);
            }
        }
        void pop_back(){ erase(--end());}
        void pop_front(){erase(begin());}
        void clear(){
            point_to_node cur = node->next;
            while(cur!=node){
                point_to_node tmp = cur;
                cur = cur->next;
                destroy_node(tmp);
            }
            node->next = node;
            node->prev = node;
        }
        void remove(const T&value){
            iterator first = begin();
            iterator last = end();
            while(first!=last){
                iterator tmp = first;
                ++tmp;
                if(*first == value)erase(first);
                first = tmp;
            }
        }
        void unique(){
            iterator first = begin();
            iterator last = end();
            if(first == last)return;
            iterator next = first;
            while(++next!=last){
                if(*first == *next){
                    erase(next);
                }else{
                    first = next;
                }
                next =first;
            }
        }
        //x链接到pos前面
        void splice(iterator pos,list& x){
            if(!x.empty()){
                transfer(pos,x.begin(),x.end());
            }
        }
        //i所指元素链接到pos前面，i可以和pos属于同一个list
        void splice(iterator pos,list&,iterator i){
            iterator j = i;
            ++j;
            if(pos ==i ||pos ==j)return;
            transfer(pos,i,j);
        }
        void splice(iterator pos,list&,iterator first,iterator last){
            if(first!=last){
                transfer(pos,first,last);
            }
        }
        void reverse(){
            if(node->next == node || node->next->next==node)return;//size==0 or size==1
            iterator first =begin();
            ++first;
            while (first!=end()){
                iterator tmp = first;
                ++first;
                transfer(begin(),tmp,first);
            }
        }
        void swap(list &x){
            point_to_node tmp = x.node;
            x.node = node;
            node = tmp;
        }
        //把已经有序的list x合并到当前list
        //要求两个list都必须是有序的
        void merge(list&x){
            iterator first1 = begin(),last1 = end(),first2 = x.begin(),last2 = x.end();
            while(first1!=last1&&first2!=last2){
                if(*first2 < *first1){
                    iterator tmp = first2;
                    transfer(first1,first2,++tmp);
                    first2 = tmp;
                }else{
                    ++first1;
                }
            }
            if(first2!=last2)transfer(last1,first2,last2);
        }
        //归并排序
        void sort(){
            if(node->next == node || node->next->next == node)return;
            list carry;
            list counter[64];//64个槽，0-64容量分别为1.2.4.8…… 2的n次，每个槽非空即满，不能装一半
            int fill = 0;//最大的满槽的编号+1，初始全空即没有满槽，所以为0
            while(!empty()){
                carry.splice(carry.begin(),*this,begin());
                int i =0;
                //i从0开始增加，如果counter[i]里面有数据，那么counter[i]就是满的（非空即满），就把
                //counter[i]里面的数据merge到carry里面，++i，查看下一个槽是否为空
                //因为fill是最大满槽+1，所以i==fill时，counter[i]必然空了
                while(i<fill && !counter[i].empty()){
                    carry.merge(counter[i]);//如果counter[i]里有数据， 就进入循环，把数据merge到carry里
                    ++i;
                }
                carry.swap(counter[i]);//这里的counter[i]必然是空的
                if(i==fill)++fill;
            }
            for(int i =1;i<fill;++i){
                counter[i].merge(counter[i-1]);
            }
            swap(counter[fill-1]);
        }
    public://constructor
        list(){empty_initialize();};
        list(list&x){
            empty_initialize();
            insert(begin(),x.begin(),x.end());
        }
        //接受两个Iterm类型指针的也包含在这个版本里面了
        template <class InputIterator>
        list(InputIterator first,InputIterator last){
            empty_initialize();
            insert(begin(),first,last);
        }
    };

    template<class T, class Alloc>
    template< class InputIterator>
    void
    list<T, Alloc>::__insert_dispatch(list::iterator pos, InputIterator n, InputIterator value, __true_type) {
        insert(pos,list<T, Alloc>::size_type(n),list<T, Alloc>::const_reference(value));
    }

    template<class T, class Alloc>
    template< class InputIterator>
    void
    list<T, Alloc>::__insert_dispatch(list::iterator pos, InputIterator first, InputIterator last, __false_type) {
        for(;first!=last;++first){
            insert(pos,*first);
        }
    }


    template <class T,class Alloc>
    inline bool operator==(const list< T, Alloc>&x,const list< T, Alloc>&y ){
        typedef typename list< T, Alloc>::iterator it;
        it first1 = x.begin(),last1 = x.end();
        it first2 = y.begin(),last2 = y.end();
        while(first1!=last1 && first2!=last2&& *first1==*first2){
            ++first1;
            ++first2;
        }
        return first1 == last1 && first2 == last2;
    };
    template <class T,class Alloc>
    inline bool operator!=(const list< T, Alloc>&x,const list< T, Alloc>&y ){
        return  !(x==y);
    };
    template <class T,class Alloc>
    inline void swap(list<T,Alloc>&x,list<T,Alloc>&y){
        x.swap(y);
    };

}
#endif //STL_LIST_H
