#include <iris/iris.h>
#include <iostream>

char a[12] = "hello world";
char b[12];
size_t size = 12;

namespace runtime{
  void memcpy(void*& dest, void*& src, size_t numBytes){
    auto* task = new iris_task;
    
    auto* d2h_mem = new iris_mem;
    char* host_mem = (char*)std::malloc(12);
    iris_mem_create(numBytes, d2h_mem);
    auto* device_mem = static_cast<iris_mem*>(dest);
    iris_task_create(task);
    
    iris_task_h2d(*task, *device_mem, 0, size, src);
    void* params[2] = { d2h_mem, dest };
    int params_info[2] = { iris_w, iris_r };
    iris_task_kernel(*task, "uppercase", 1, NULL, &size, NULL, 2, params, params_info);
    iris_task_d2h(*task, *d2h_mem, 0, size, host_mem);
    iris_task_submit(*task, iris_gpu, NULL, 1);

    printf("%s\n", host_mem);

    iris_task_release(*task);
    iris_mem_release(*device_mem);
    iris_mem_release(*d2h_mem);

    iris_finalize();

  }
}

template <typename T, typename U>
inline void memcpy(T*& dest, U*& src, size_t numBytes){
    void* void_dest = dest;
    void* void_src = src;
    runtime::memcpy(void_dest, void_src, numBytes);
    dest = static_cast<T*>(void_dest);
    src = static_cast<U*>(void_src);
}

void *new_malloc(size_t numBytes){
   
    auto* mem = new iris_mem;
    auto create_result = iris_mem_create(numBytes, mem);
    return mem;
}

int main(int argc, char** argv) {
  iris_init(&argc, &argv, 1);

  char* a = (char*)new_malloc(12);
  char* b = (char*)std::malloc(12);
  b="hello world";
 
  memcpy(a, b, 12);

  return 0;
}

