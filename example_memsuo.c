
#include <stdio.h>
#include "memsuo/m_memsuo.h"

int main() {
    // Basic memory allocation
    int *data = (int*)MALLOC(10 * sizeof(int));
    
    if (!data) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    // Use the allocated memory
    for (int i = 0; i < 10; i++) {
        data[i] = i * 10;
    }
    
    // Display the values
    printf("Values: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
    
    // Free the memory
    FREE(data);
    
    return 0;
}
