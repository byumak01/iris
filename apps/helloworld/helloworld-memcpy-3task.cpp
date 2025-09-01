#include <iris/iris.h>
#include <iostream>

char a[12] = "hello world";
char b[12];
size_t size = 12;

namespace runtime{
  void memcpy(void*& dest, void*& src, size_t numBytes){

    // h2d stage
    auto* task_h2d= new iris_task;
    iris_task_create(task_h2d);
    
    auto* d2h_mem = new iris_mem;
    iris_mem_create(numBytes, d2h_mem);

    auto* device_mem = static_cast<iris_mem*>(dest);
    iris_task_h2d(*task_h2d, *device_mem, 0, size, src);
    //iris_synchronize();
    printf("src: %s\n", (char*)src);
    //void* params[2] = { d2h_mem, dest };
    //int params_info[2] = { iris_w, iris_r };
    iris_task_submit(*task_h2d, iris_gpu, NULL, 1);

    // kernel stage
    auto* task_kernel= new iris_task;
    iris_task_create(task_kernel);

    auto* kernel = new iris_kernel;
    iris_kernel_create("uppercase", kernel);

    //iris_task_kernel(*task_kernel, "uppercase", 1, NULL, &size, NULL, 2, params, params_info);
    // kernel_object expects iris_kernel as its first argument 
    // which we do not have in this example. In theory it should
    // work in CHARM-SYCL. 
    iris_kernel_setmem(*kernel, 0, *d2h_mem, iris_rw);
    iris_kernel_setmem(*kernel, 1, *(static_cast<iris_mem*>(dest)), iris_rw);
    
    iris_task_kernel_object(*task_kernel, *kernel, 1, NULL, &size, NULL); 
   
    iris_task_submit(*task_kernel, iris_gpu, NULL, 1);

    // d2h stage
    char* host_mem = (char*)std::malloc(12);

    auto* task_d2h = new iris_task;
    iris_task_create(task_d2h);
    iris_task_d2h(*task_d2h, *d2h_mem, 0, size, host_mem);
    iris_task_submit(*task_d2h, iris_gpu, NULL, 1);

    printf("d2h: %s\n", host_mem);

    iris_task_release(*task_h2d);
    iris_task_release(*task_d2h);
    iris_task_release(*task_kernel);
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

