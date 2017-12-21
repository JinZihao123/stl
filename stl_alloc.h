//
// Created by jzh on 17-12-13.
//

#ifndef STL_STL_ALLOC_H
#define STL_STL_ALLOC_H

#ifndef __THROW_BAD_ALLOC
#  if defined(__STL_NO_BAD_ALLOC) || !defined(__STL_USE_EXCEPTIONS)
#    include <stdio.h>
#    include <stdlib.h>
#    define __THROW_BAD_ALLOC fprintf(stderr, "out of memory\n"); exit(1)
#  else /* Standard conforming out-of-memory handling */
#    include <new>
#    define __THROW_BAD_ALLOC throw std::bad_alloc()
#  endif
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
/*--------------------------------第一级配置器-----------------------------*/
//直接用c语言的malloc，realloc和free来控制内存
//源码的__inst这个参数完全没派上用场，去掉试试看？FIXME
//成员都是static的，因为函数都和实例无关，这样做handler是所有实例共有的，设置一次就可以了
//感觉就是把class当namespace用了
class __malloc_alloc_template {

private:
    //oom：out of memory
    static void* oom_malloc(size_t);
    static void* oom_realloc(void*, size_t);
//如果编译器支持模板类静态成员, 则使用错误处理函数, 类似C++的set_new_handler()
    static void (* __malloc_alloc_oom_handler)();

public:

    static void* allocate(size_t __n)
    {
        void* __result = malloc(__n);
        if (0 == __result) __result = oom_malloc(__n);
        return __result;
    }

    static void deallocate(void* __p, size_t /* __n */)
    {
        free(__p);
    }

    static void* reallocate(void* __p, size_t /* old_sz */, size_t __new_sz)
    {
        void* __result = realloc(__p, __new_sz);
        if (0 == __result) __result = oom_realloc(__p, __new_sz);
        return __result;
    }
    //处理内存不足的handler，由用户设定，没有设定就是NULL
    //FIXME 函数指针的写法太复杂了，考虑用c++11的Functor代替
    static void (* __set_malloc_handler(void (*__f)()))()
    {
        void (* __old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = __f;
        return(__old);
    }

};

void *__malloc_alloc_template::oom_malloc(size_t n) {
    void (* __my_malloc_handler)();
    void* __result;
    //如果设置过oom处理函数，反复调用尝试分配内存，如果没有设置，直接抛异常
    for (;;) {
        __my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == __my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*__my_malloc_handler)();
        __result = malloc(n);
        if (__result) return(__result);
    }
}

void *__malloc_alloc_template::oom_realloc(void *p, size_t n) {
    void (* __my_malloc_handler)();
    void* __result;

    for (;;) {
        __my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == __my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*__my_malloc_handler)();
        __result = realloc(p, n);
        if (__result) return(__result);
    }
}
typedef __malloc_alloc_template malloc_alloc;
/*--------------------------------第一级配置器-----------------------------*/
/*--------------------------------第二级配置器-----------------------------*/
//如果不考虑多线程的话，是否二级配置器也可以做成不是template FIXME

enum {_ALIGN = 8}; //小区快的大小，申请大小会被调成8的倍数
enum {_MAX_BYTES = 128};//大于这个值就直接用一级配置器了
enum {_NFREELISTS = 16};//free list的个数
class _default_alloc_template{
private:

    //这个即当做链表的节点，又当做allocate的返回值，内容就是一个地址，两种解读方式，很精髓
    union obj{
        union obj* fress_list_link;
        char chlien_date[1];
    };

    static size_t round_up(size_t bytes){
        return ((bytes + _ALIGN - 1 ) & ~(_ALIGN -1));//加7，然后砍掉尾巴
    }
    //返回bytes对应的freelist的序号
    static size_t freelist_index(size_t bytes){
        return ((bytes + _ALIGN -1)/_ALIGN -1);
    }
    //当fress_list没有可用的空间时使用， 返回大小为n的空间作为allocate的返回值
    //并且分配大小为n的空间给fress_list
    static void * refill(size_t n);
    static char * chunk_alloc(size_t size ,int &nobjs );


    //注意这是指向数组的指针，而不是一个数组，fress_list类型其实是obj**
    static obj* volatile free_list[_NFREELISTS]=
            {nullptr,nullptr,nullptr,nullptr,
             nullptr,nullptr,nullptr,nullptr,
             nullptr,nullptr,nullptr,nullptr,
             nullptr,nullptr,nullptr,nullptr};
    static char * start_memory_poll = nullptr;//内存池的起始点
    static char * end_memory_poll = nullptr;//内存池的终点
    static size_t heap_size = 0;
public:
    static void * allocate(size_t n);
    static void deallocate(void *p, size_t n);
    static void * reallocate(void *p, size_t old_sz,size_t new_sz);

};

static void * _default_alloc_template::allocate(size_t n) {
    obj * volatile *my_free_list;//FIXME 为什么要volatile？
    obj *result;
    if(n>_MAX_BYTES){//大于128直接用第一级配置
        return __malloc_alloc_template::allocate(n);
    }
    my_free_list = free_list + freelist_index(n);
    result = *my_free_list;
    if(result == nullptr){//free_list没有现成的空间就用refill给free_list填充
        //每次默认填充round_up(n)的20倍的空间给free_list
        void *r = refill(round_up(n));
        return  r;
    }
    //如果有空间就取出链表头部返回
    *my_free_list = result->fress_list_link;
    return  result;
}

void _default_alloc_template::deallocate(void *p, size_t n) {
    if(n>_MAX_BYTES){
        //大于128
        __malloc_alloc_template::deallocate(p,n);
        return;
    }
    //归还的空间重新放到链表头部
    obj * volatile *my_free_list = free_list + freelist_index(n);
    obj*  q = static_cast<obj*>(p);
    q->fress_list_link = *my_free_list;
    *my_free_list = q;
}

void *_default_alloc_template::refill(size_t n) {
    int nobjs = 20;//默认分配20倍的n给free_list;
    char * chunk =_default_alloc_template::chunk_alloc(n,nobjs);//nobjs是个值—结果参数
    if(nobjs ==1){//就申请到一个大小为n内存，就直接返回给调用者
        return chunk;
    }
    //申请到一个以上大小为n的内存，就排进链表里面
    obj * volatile * my_free_list = free_list + freelist_index(n);
    //申请到的第一块空间是返回给用户的，第二块开始才装进链表
    obj * result = static_cast<obj*> (chunk);
    *my_free_list = static_cast<obj*>(chunk +n);//+n就是下一个块的地址头部了
    obj * next = static_cast<obj*>(chunk +n);
    obj * cur;
    for(int i =1;;i++){
        cur = next;
        next = static_cast<obj*>(static_cast<char*>(next) +n);
        if(nobjs -1 ==i){
            cur->fress_list_link = nullptr;
            break;
        }
        else{
            cur->fress_list_link = next;
        }
    }
    return result;
}

char *_default_alloc_template::chunk_alloc(size_t size, int &nobjs) {
    //size必须是8的倍数
    char *result;
    size_t need_bytes = size*nobjs;
    size_t left_bytes = end_memory_poll - start_memory_poll;
    if(left_bytes >= need_bytes){
        //如果内存池空间足够就直接分配,不需要申请新内存了
        result = start_memory_poll;
        start_memory_poll += need_bytes;
        return  result;
    }
    else if( left_bytes >=size){
        //内存池剩余空间不够全部，但够分配一块
        nobjs = left_bytes * size;
        need_bytes = nobjs * size;
        result = start_memory_poll;
        start_memory_poll += need_bytes;
        return  result;
    }else{
        //内存池剩余空间小于一块size了
        if(left_bytes >0){
            //把内存池剩余的空间放到合适的free list上去，避免浪费
            obj* volatile *my_free_list = free_list + freelist_index(left_bytes);
            static_cast<obj*> (start_memory_poll)->fress_list_link = *my_free_list;
            *my_free_list = static_cast<obj*>(start_memory_poll);
        }
        //至此，内存池里面没有任何内存了，准备重新分配

        //内存池申请策略：heap_size初始值为0，每次成功申请了就heap_size+=bytes_to_get
        //而bytes_to_get至少为heap_size的16倍
        //也就是每次申请内存池空间，申请的容器都至少为上一次的16倍
        size_t bytes_to_get = 2*need_bytes + round_up(heap_size>>4);
        start_memory_poll = static_cast<char*>(malloc(bytes_to_get));
        if(start_memory_poll == nullptr){
            //申请失败的情况， 系统内存不足了
            //策略是在free_list的比当前size大的节点里面寻找有没有不用的节点
            //如果有，就取出链表的第一节拿来当做内存池（其实做多也就128bytes），
            // 虽然少一点，好歹是申请到够这次用的内存池了
            obj* volatile * my_free_list ;
            obj* p;
            for(int i = size;i<=_MAX_BYTES;i+=_ALIGN){
                my_free_list = free_list + freelist_index(i);
                p = *my_free_list;
                if(p!=nullptr){
                    *my_free_list = p->fress_list_link;
                    start_memory_poll = static_cast<char*>(p);
                    end_memory_poll = start_memory_poll + i;
                    //现在内存池又有内存了，递归调用自己
                    return  chunk_alloc(size,nobjs);
                }
            }
            //运行到这里说明free_list里面都没有可用内存了，基本山穷水尽
            //调用一级配置器，也许一级配置器的out of memory机制会有所处理
            start_memory_poll = static_cast<char*>(__malloc_alloc_template::allocate(bytes_to_get);
        }
        heap_size += bytes_to_get;
        end_memory_poll = start_memory_poll +bytes_to_get;
        //现在内存池又有内存了，递归调用自己
        return chunk_alloc(size,nobjs);
    }

}

void *_default_alloc_template::reallocate(void *p, size_t old_sz, size_t new_sz) {
    //大于128bytes就直接realloc
    //小于128bytes的自己分配新内存并拷贝数据
    void* result;
    size_t copy_sz;

    if (old_sz > (size_t) _MAX_BYTES && new_sz > (size_t) _MAX_BYTES) {
        return(realloc(p, new_sz));
    }
    if (round_up(old_sz) == round_up(new_sz)) return(p);
    result = allocate(new_sz);
    copy_sz = new_sz > old_sz? old_sz : new_sz;
    memcpy(result, p, copy_sz);
    deallocate(p, old_sz);
    return(result);
}

typedef _default_alloc_template alloc;
/*---------------------------------第二级配置器-----------------------------*/
//把alloc封装成符合stl规范的模板
template <class T,class Alloc>
class simple_alloc{
public:
    static T* allocate(size_t n){
        return n==0? nullptr: static_cast<T*>(Alloc::allocate(n*sizeof(T)));
    }
    static T* allocate(){
        return static_cast<T*>(Alloc::allocate(sizeof(T)));
    }
    static void deallocate(T*p,size_t n){
        if(n!=0){
            Alloc::deallocate(p,n*sizeof(T));
        }
    }
    static void deallocate(T*p){
        Alloc::deallocate(p,sizeof(T));
    }
};

#endif //STL_STL_ALLOC_H
