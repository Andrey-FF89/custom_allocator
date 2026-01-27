#include "custom_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ITERATIONS 1000000
#define BATCH_SIZE 1000

/*
 * Функциональные тесты
 */
void run_functional_tests() {
	printf("=== Functional Tests ===\n");
	my_allocator_init();

    // Тест выделения памяти
	char* p1 = (char*)my_malloc(15);
	char* p2 = (char*)my_malloc(15);
	char* p3 = (char*)my_malloc(180);

	if (p1 && p2 && p3) {
		printf("[OK] Allocation successful\n");
	} else {
		printf("[FAIL] Allocation failed\n");
		return;
	}

    // Проверка неидентичности указателей
	if (p1 != p2) {
		printf("[OK] Pointers are distinct\n");
	}

	// Тест использования памяти
    strcpy(p1, "Test15");
    strcpy(p3, "Test180_Long_String");
    printf("Data inside: %s, %s\n", p1, p3);

    // Тест освобождения и переиспользования
    my_free(p1); 
    char* p4 = (char*)my_malloc(15);
    
    if (p4 == p1) {
        printf("[OK] Reused freed memory address\n");
    } else {
        printf("[WARN] Memory was not reused immediately\n");
    }

    // Попытка выделить недопустимый размер
    void* p_bad = my_malloc(878);
    if (p_bad == NULL) {
        printf("[OK] Invalid size handled correctly\n");
    }
    
    // Очистка
    my_free(p2);
    my_free(p3);
    my_free(p4);
    printf("\n");
}

/*
 * Тесты производительности
 */
void run_benchmark() {
	printf("=== Performance Benchmark ===\n");
	printf("Iterations: %d total ops\n", ITERATIONS);

	clock_t start_sys = clock();
	void* pointers[BATCH_SIZE];

	for (int i = 0; i < ITERATIONS / BATCH_SIZE; i++) {
		for (int j = 0; j < BATCH_SIZE; j++) {
			pointers[j] = malloc(15);
			*(char*)pointers[j] = (char)j;
		}
		for (int j = 0; j < BATCH_SIZE; j++) {
			free(pointers[j]);
		}
	}
	
	clock_t end_sys = clock();
	double time_sys = (double)(end_sys - start_sys) / CLOCKS_PER_SEC;

	my_allocator_init();
	clock_t start_my = clock();

	for (int i = 0; i < ITERATIONS / BATCH_SIZE; i++) {
		for (int j = 0; j < BATCH_SIZE; j++) {
			pointers[j] = my_malloc(15);
			if (!pointers[j]) return;
			*(char*)pointers[j] = (char)j;
		}
		for (int j = 0; j < BATCH_SIZE; j++) {
			my_free(pointers[j]);
		}
	}
	
	clock_t end_my = clock();
	double time_my = (double)(end_my - start_my) / CLOCKS_PER_SEC;

	printf("System malloc: %.6f sec\n", time_sys);
	printf("Custom malloc: %.6f sec\n", time_my);
	
	if (time_my < time_sys) {
		printf(">> Custom allocator is %.2fx FASTER\n", time_sys / time_my);
	}
}

int main() {
	run_functional_tests();
	run_benchmark();
	return 0;
}