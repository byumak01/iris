#include <iris/iris.h>
#include <stdio.h>

int main(int argc, char** argv) {
    iris_init(&argc, &argv, 1);
    int n = 16;
    int block_size = 8;
    int num_blocks = (n + block_size - 1) / block_size;
    
    
    // Host arrays
    int h_input[n], h_output[n];
    
    
    // Initialize input data
    for (int i = 0; i < n; i++) {
        h_input[i] = i + 1;
        printf("Input[%d] = %d\n", i, h_input[i]);
    }
    // Create Iris memory objects
    iris_mem d_input, d_output;
    
    // Allocate device memory
    iris_mem_create(n * sizeof(int), &d_input);
    iris_mem_create(n * sizeof(int), &d_output);
    printf("mem creation handled\n");
    
    // Copy data to device
    iris_task task_h2d;
    iris_task_create(&task_h2d);
    iris_task_h2d(task_h2d, d_input, 0, n * sizeof(int), h_input);
    iris_task_submit(task_h2d, iris_default, NULL, 0);
    iris_synchronize();
    iris_task_release(task_h2d);
    printf("h2d handled\n");
    
    // Create kernel
    iris_kernel kernel;
    iris_kernel_create("shared_memory_reverse", &kernel);
    
    // Set kernel arguments
    iris_kernel_setmem(kernel, 0, d_input, iris_rw);
    iris_kernel_setmem(kernel, 1, d_output, iris_rw);
    printf("n: %d\n", n); 
    printf("&n: %p\n", &n); 
    //iris_kernel_setarg(kernel, 2, 2048, NULL);
    
    // Set shared memory (pass NULL as last argument with shared memory size)
    int shared_mem_size = block_size * sizeof(int);
    // Create task
    iris_task task;
    iris_task_create(&task);
    
    // Set kernel for task with execution dimensions
    size_t global_work_size = num_blocks * block_size;
    size_t local_work_size = block_size;
    printf("global_work_size: %d \n", global_work_size);
    printf("local_work_size: %d \n", local_work_size);
    iris_task_kernel_object_lmem(task, kernel, 1, NULL, &global_work_size, &local_work_size, 8192);
    
    // Submit task
    iris_task_submit(task, iris_default, NULL, 0);
    iris_synchronize();
    
    printf("kernel handled\n");
    // Copy result back to host
    iris_task task_d2h;
    iris_task_create(&task_d2h);
    iris_task_d2h(task_d2h, d_output, 0, n * sizeof(int), h_output);
    iris_task_submit(task_d2h, iris_default, NULL, 0);
    iris_synchronize();
    iris_task_release(task_d2h);
    printf("d2h handled");
    
    // Print results
    printf("\nOutput (reversed within each block of %d elements):\n", block_size);
    for (int i = 0; i < n; i++) {
        printf("Output[%d] = %d\n", i, h_output[i]);
    }
    printf("Output[0] with p = %p\n", h_output[0]);
    printf("Output[0] with d = %d\n", h_output[0]);
    
    // Cleanup
    iris_task_release(task);
    //iris_kernel_release(kernel);
    iris_mem_release(d_input);
    iris_mem_release(d_output);
    iris_finalize();
    
    return 0;
}

/*
#include <iris/iris.h>
#include <stdio.h>

//char a[12] = "hello world";
char b[24];
size_t size = 24;

int main(int argc, char** argv) {
  iris_init(&argc, &argv, 1);

 // iris_mem mem_a;
  iris_mem mem_b;
  //iris_mem_create(size, &mem_a);
  iris_mem_create(size, &mem_b);

  iris_kernel kernel;
  iris_kernel_create("uppercase", &kernel);
  iris_task task;
  iris_task_create(&task);
//  iris_task_h2d(task, mem_a, 0, size, a);
  
  iris_kernel_setmem_off(kernel, 0, mem_b, 0, iris_w);
  iris_kernel_setarg(kernel, 1, 20, NULL);
  iris_kernel_setarg(kernel, 2, 12, NULL);
 
  iris_task_kernel_object(task, kernel, 1, NULL, &size, NULL); 
  iris_task_d2h(task, mem_b, 0, size, b);
  iris_task_submit(task, iris_roundrobin, NULL, 1);

    printf("%s\n", b);

  iris_task_release(task);
  //iris_mem_release(mem_a);
  iris_mem_release(mem_b);

  iris_finalize();

  return 0;
}
*/

