//
// Created by jzh on 18-1-4.
//

#ifndef STL_HASH_TABLE_H
#define STL_HASH_TABLE_H

#include <algorithm>
#include "iterator.h"
#include "alloc.h"
#include "construct.h"
#include "vector.h"
namespace mystl {
    template <class T1,class T2>
    struct pair{
        T1 first;
        T2 second;
        pair(const T1&x1,const T2&x2):first(x1),second(x2){}
    };
    template<class Value>
    struct __hashtable_node {
        __hashtable_node<Value> *next;
        Value val;
    };
    template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc = alloc>
    class hashtable ;

    
    template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
    struct __hashtable_iterator:public mystl::iterator<forward_iterator_tag,Value>{
        typedef hashtable< Value,Key, HashFcn, ExtractKey, EqualKey, Alloc > 
                hashtable;
        typedef __hashtable_iterator< Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
                iterator;
        typedef const __hashtable_iterator< Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
                const_iterator;
        typedef __hashtable_node<Value> node;
        
    private:
        node* cur;
        hashtable *ht;
    public:
        __hashtable_iterator(){}
        __hashtable_iterator(node*n,hashtable*tab):cur(n),ht(tab){}
        reference operator*(){ return cur->val;}
        pointer operator->(){ return &(operator*());}
        bool operator==(const iterator &it)const{return cur == it->cur;}
        bool operator!=(const iterator &it)const{return cur != it->cur;}
        iterator& operator++(){
            const node* old = cur;
            cur = cur->next;
            if (!cur) {
                size_type bucket = ht->bkt_num(old->val);
                while (!cur && ++bucket < ht->buckets.size())
                    cur = ht->buckets[bucket];
            }
            return *this;
        }
        iterator&operator++(int){
            iterator tmp =*this;
            ++*this;
            return tmp;
        }
    };

    static const int __stl_num_primes=28;
    static const unsigned long __stl_prime_list[__stl_num_primes] =
            {
                    53ul,         97ul,         193ul,       389ul,       769ul,
                    1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
                    49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
                    1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
                    50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
                    1610612741ul, 3221225473ul, 4294967291ul
            };
    inline inline unsigned long stl_next_prime(unsigned long n)
    {
        const unsigned long* first = __stl_prime_list;
        const unsigned long* last = __stl_prime_list + (int)__stl_num_primes;
        const unsigned long* pos = std::lower_bound(first, last, n);//todo
        return pos == last ? *(last - 1) : *pos;
    }

    template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc = alloc>
    class hashtable{
    public:
        typedef __hashtable_iterator<Value,Key,HashFcn,ExtractKey,EqualKey,Alloc> iterator;
        typedef HashFcn             hasher;
        typedef EqualKey            key_equal;
        typedef size_t              size_type;
        typedef Key                 key_type;
        typedef Value               value_type;
        typedef ptrdiff_t           difference_type;
        typedef value_type*         pointer;
        typedef const value_type*   const_pointer;
        typedef value_type&         reference;
        typedef const value_type&   const_reference;

        size_type max_bucket_count()const{ return __stl_prime_list[__stl_num_primes-1];}

        hashtable(size_type n,const HashFcn&hf,const EqualKey& eql):
                hash(hf),equals(eql),get_key(ExtractKey()),num_elements(0){
            initialize_buckets(n);
        };
        hashtable(const hashtable& ht)
                : hash(ht.hash), equals(ht.equals), get_key(ht.get_key), num_elements(0)
        {
            copy_from(ht);
        }


        hashtable& operator= (const hashtable& ht)
        {
            if (&ht != this) {
                clear();
                hash = ht.hash;
                equals = ht.equals;
                get_key = ht.get_key;
                copy_from(ht);
            }
            return *this;
        }

        //一个允许重复，一个不允许
        pair<iterator,bool> insert_unique(const value_type&obj){
                resize(num_elements+1);//插入后hashtable的size比buckets的size还大了，就要重新组织
                return insert_unique_no_resize(obj);
        };
        iterator insert_equal(const value_type&obj){
            resize(num_elements+1);//插入后hashtable的size比buckets的size还大了，就要重新组织
            return insert_equal_no_resize(obj);
        };
        void clear(){
            for(size_type i =0;i<buckets.size();++i){
                node*cur = buckets[i];
                while(cur){
                    node* next = cur->next;
                    delete_node(cur);
                    cur = next;
                }
                buckets[i] = nullptr ;
            }
            num_elements = 0;
            //node全部删除，但是buckets并不变小
        }
        iterator find(const key_type&key){
            size_type n = bkt_num(key);
            node* first;
            for(first = buckets[n];first&&!equals(get_key(first->val),key);first = first->next);
            return iterator(first,this);

        }
    private:
        typedef __hashtable_node<value_type > node;
        typedef simple_alloc<node,Alloc> node_allocator;

        hasher hash;
        key_equal equals;
        ExtractKey get_key;
        size_type num_elements;
        vector<node*,Alloc>buckets;

        node* new_node(const value_type&v){
            node* n = node_allocator::allocate();
            n->next= nullptr;
            construct(&n->val,v);
            return n;
        }
        void delete_node(node * n){
            destroy(&n->val);
            node_allocator::deallocate(n);
        }

        void initialize_buckets(size_type n){
            const size_type n_buckets = stl_next_prime(n);
            buckets.reserve(n_buckets);
            buckets.insert(buckets.end(),n_buckets,(node*)0);
            num_elements = 0;
        }
        void resize(size_type num_elements_hint){
            const size_type old_n = buckets.size();
            if(num_elements_hint>old_n){
                const size_type n = stl_next_prime(num_elements_hint);
                if(n>old_n){
                    vector<node*,Alloc>tmp(n,(node*)0);
                    for(size_type bucket = 0;bucket <old_n;++bucket){
                        node*first = buckets[bucket];
                        while(first){
                            size_type new_bucket = bkt_num(first->val,n);
                            buckets[bucket]=first->next;
                            first->next=tmp[new_bucket];
                            tmp[new_bucket] = first;
                            first = buckets[bucket];
                        }
                    }
                    buckets.swap(tmp);
                }
            }
        }
        pair<iterator,bool>insert_unique_no_resize(const value_type&obj){
            const size_type n = bkt_num(obj);
            node* first = buckets[n];
            for(node*cur = first;cur;cur = cur->next){
                //在桶上检查是否有相等的key，如果有就直接返回
                if(equals(get_key(cur->val),get_key(obj))){
                    return pair<iterator,bool>(iterator(cur,this),false);
                }
            }
            node*tmp = new_node(obj);
            tmp->next = first;
            buckets[n] = tmp;
            ++num_elements;
            return pair<iterator,bool>(iterator(tmp,this),true);
        };
        iterator insert_equal_no_resize(const value_type&obj){
            const size_type n = bkt_num(obj);
            node* first = buckets[n];
            for(node*cur = first;cur;cur = cur->next){
                //在桶上检查是否有相等的key，就立即插入返回
                //插入位置是在相同节点的后面
                if(equals(get_key(cur->val),get_key(obj))){
                    node*tmp = new_node(obj);
                    tmp->next = cur->next;
                    cur->next = tmp;
                    ++num_elements;
                    return  iterator(tmp,this);
                }
            }
            node*tmp = new_node(obj);
            tmp->next = first;
            buckets[n] = tmp;
            ++num_elements;
            return iterator(tmp,this);
        };

        size_type bkt_num(const value_type&obj,size_t n)const{
            return bkt_num_key(get_key(obj),n);
        }
        size_t bkt_num(const value_type&obj)const{
            return  bkt_num_key(get_key(obj));
        }
        size_type bkt_num_key(const key_type&key)const{
            return  bkt_num_key(key,buckets.size());
        }
        size_type bkt_num_key(const key_type&key,size_t n)const{
            return hash(key)%n ;
        }
        void copy_from(const hashtable&ht){
            //先清除自己的 bucket vector ，这操作是调用 vector::clear。造成所有元素为 0
            buckets.clear();
            //如果自己的空间大于对方，就什么都不做，否则 reserve() 函数会把 vector 的空间变得和 ht.buckets.size() 一样大
            buckets.reserve(ht.buckets.size());
            //将每个 bucket 初始化为 NULL
            buckets.insert(buckets.end(), ht.buckets.size(), (node*) 0);
            for(size_type i = 0; i < ht.buckets.size(); ++i){
                //如果 bucket 的链表不空，就复制它的每一个节点
                if(const node* cur = ht.buckets[i]){
                    node* copy = new_node(cur->val);
                    buckets[i] = copy;
                    for (node* next = cur->next; next; cur = next, next = cur->next) {
                        copy->next = new_node(next->val);
                        copy = copy->next;
                    }
                }
            }
            num_elements = ht.num_elements;
        }

    };
    
    
}

#endif //STL_HASH_TABLE_H
