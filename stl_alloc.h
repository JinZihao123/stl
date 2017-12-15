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
//第一级配置器
//直接用c语言的malloc，realloc和free来控制内存
//源码的__inst这个参数完全没派上用场，去掉试试看？
//成员都是static的，因为函数都和实例无关，这样做handler是所有实例共有的，设置一次就可以了
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
    //函数指针的写法太复杂了，考虑用c++11的Functor代替
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


#endif //STL_STL_ALLOC_H
