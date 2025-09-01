extern "C" __global__ void shared_memory_reverse(int* input, int* output) {
    extern __shared__ int shared_data[];
    int n = 16;
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int global_id = bid * blockDim.x + tid;
    
    // Load data from global memory to shared memory
    if (global_id < n) {
        shared_data[tid] = input[global_id];
    } else {
        shared_data[tid] = 0; // Handle out-of-bounds
    }
    // shared_data[tid + 1000] = tid;
    // Synchronize to make sure all threads have loaded their data
    __syncthreads();
    
    // Reverse the data within the block using shared memory
    int reverse_idx = blockDim.x - 1 - tid;
    
    // Write reversed data back to global memory
    // Use ternary operator instead of if statement
    int result = (global_id < n) ? shared_data[reverse_idx] : -999;
    output[global_id] = result;

    //if (tid == 0) {
    //output[0] = (long long)shared_data;  // See what address space this is in
    //}
}

/*
extern "C" __global__ void shared_memory_reverse(int* input, int* output, int n) {
    extern __shared__ int shared_data[];
    
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int global_id = bid * blockDim.x + tid;
    
    // Test 1: Can we write to shared memory?
    if (tid < 16) {
        shared_data[tid] = tid + 100; // Put distinctive values
    }
    
    __syncthreads();
    
    // Test 2: Can we read from shared memory?
    //if (global_id < n) {
        output[global_id] = shared_data[tid]; // Should show 100, 101, 102, etc.
    //}
    
    // Debug info - let's see what we get
    
    if (global_id == 0) {
        output[0] = shared_data[0]; // Should be 100
    }
    if (global_id == 1) {
        output[1] = shared_data[1]; // Should be 101
    }
    if (global_id == 2) {
        output[2] = shared_data[2]; // Should be 102
    }
    
}
*/
// Compile with: nvcc -o shared_memory_test shared_memory_test.cu

/*
extern "C" __global__ void uppercase(char* b, char *a, char *c) {
    
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  extern __shared__ char s[];
  char *t = s;

  char *a = (char*)"hello world dummy text";
  
  if (a[i] >= 'a' && a[i] <= 'z') b[i] = a[i] + 'A' - 'a';
  else b[i] = a[i];
}

*/
