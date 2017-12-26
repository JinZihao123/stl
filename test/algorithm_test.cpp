//
// Created by jzh on 17-12-26.
//

#include "algorithm_test.h"
namespace AlgorithmTest{
    void testCopy(){
        char arr1[] = "hello", res1[6] = { 0 };
        mystl::copy(std::begin(arr1), std::end(arr1), res1);
        assert(mystl::Test::container_equal(arr1, res1));

        wchar_t arr2[] = L"hello", res2[6] = { 0 };
        mystl::copy(std::begin(arr2), std::end(arr2), res2);
        assert(mystl::Test::container_equal(arr2, res2));

        int arr3[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, res3[10] = { 0 };
        mystl::copy(std::begin(arr3), std::end(arr3), res3);
        assert(mystl::Test::container_equal(arr3, res3));

        std::string arr4[3] = { "1", "2", "3" }, res4[3];
        mystl::copy(std::begin(arr4), std::end(arr4), res4);
        assert(mystl::Test::container_equal(arr4, res4));
    }
    void testCopy_Backward(){
        char arr1[] = "hello", res1[6] = { 0 };
        mystl::copy_backward(std::begin(arr1), std::end(arr1), res1 + 6);
        assert(mystl::Test::container_equal(arr1, res1));

        wchar_t arr2[] = L"hello", res2[6] = { 0 };
        mystl::copy_backward(std::begin(arr2), std::end(arr2), res2 + 6);
        assert(mystl::Test::container_equal(arr2, res2));

        int arr3[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, res3[10] = { 0 };
        mystl::copy_backward(std::begin(arr3), std::end(arr3), res3 + 10);
        assert(mystl::Test::container_equal(arr3, res3));

        std::string arr4[3] = { "1", "2", "3" }, res4[3];
        mystl::copy_backward(std::begin(arr4), std::end(arr4), res4 + 3);
        assert(mystl::Test::container_equal(arr4, res4));
    }
    void testAllCases() {
        testCopy();
        testCopy_Backward();
    }

}
int main(){
    AlgorithmTest::testAllCases();
    std::cout<<"Pass"<<std::endl;
}