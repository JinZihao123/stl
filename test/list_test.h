//
// Created by jzh on 17-12-28.
//

#ifndef STL_LIST_TEST_H
#define STL_LIST_TEST_H


#include "test.h"

#include "../list.h"
#include <list>

#include <cassert>
#include <functional>
#include <string>
#include <random>
namespace mystl{
    namespace ListTest{
        template<class T>
        using stdL = std::list < T > ;
        template<class T>
        using tsL = mystl::list < T > ;

        void testCase1();
        void testCase2();
        void testCase3();
        void testCase4();
        //void testCase5();
        void testCase6();
        void testCase7();
        void testCase8();
        void testCase9();
        void testCase10();
        void testCase11();
        void testCase12();
        void testCase13();
        void testCase14();
        void testCase15();

        void testAllCases();
    }
}
#endif //STL_LIST_TEST_H
