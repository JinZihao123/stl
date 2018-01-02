//
// Created by jzh on 18-1-2.
//

#ifndef STL_PRIORITY_QUEUE_TEST_H
#define STL_PRIORITY_QUEUE_TEST_H

#include "../priority_queue.h"
#include <queue>
#include <string>
#include <iostream>
namespace mystl{
    namespace PriorityQueueTest{
        template<class T>
        using stdPQ = std::priority_queue < T > ;
        template<class T>
        using tsPQ = mystl::priority_queue < T > ;

        void testCase1();
        void testCase2();
        void testCase3();
        void testCase4();
        void testCase5();

        void testAllCases();
    }
}

#endif //STL_PRIORITY_QUEUE_TEST_H
