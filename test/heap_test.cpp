//
// Created by jzh on 18-1-2.
//

#include "../heap.h"
#include "../vector.h"
#include "test.h"
int main(){
    int ia[9]={0,1,2,3,4,8,9,3,5};
    mystl::vector<int>ivec(ia,ia+9);

    mystl::make_heap(ivec.begin(),ivec.end());
    int ans1[9]={9,5,8,3,4,0,2,1,3};
    assert(mystl::Test::container_equal(ivec,ans1));

    ivec.push_back(7);
    mystl::push_heap(ivec.begin(),ivec.end());
    int ans2[10] = {9,7,8,3,5,0,2,1,3,4};
    assert(mystl::Test::container_equal(ivec,ans2));

    mystl::pop_heap(ivec.begin(),ivec.end());
    assert(ivec.back()==9);
    ivec.pop_back();

    int ans3[9] = {8,7,4,3,5,0,2,1,3};
    assert(mystl::Test::container_equal(ivec,ans3));

    mystl::sort_heap(ivec.begin(),ivec.end());
    int ans4[9] ={0,1,2,3,3,4,5,7,8};
    assert(mystl::Test::container_equal(ivec,ans4));

    std::cout<<"pass"<<std::endl;
}