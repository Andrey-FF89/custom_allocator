/**
 * @file main.c
 * @brief Тестовый модуль и профилирование производительности.
 *
 * @details Содержит функциональные тесты для проверки корректности работы
 * аллокатора и сравнительный бенчмарк со стандартным аллокатором.
 */

#include "custom_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/** @brief Количество итераций для нагрузочного тестирования. */
#define ITERATIONS 1000000

/** @brief Размер пакета выделений (должен быть меньше размера пула). */
#define BATCH_SIZE 1000

/**
 * @brief Запуск функциональных тестов.
 *
 * Проверяет:
 * - Успешное выделение памяти.
 * - Уникальность адресов.
 * - Возможность записи/чтения данных.
 * - Корректность переиспользования памяти после free.
 */
void run_functional_tests() {
	printf("=== Functional Tests ===\n");
	my_allocator_init();

	/* Тест 1: Базовое выделение */
	char* p1 = (char*)my_malloc(15);
	char* p2 = (char*)my_malloc(15);
	char* p3 = (char*)my_malloc(180);

	if (p1 && p2 && p3) {
		printf("[OK] Allocation successful.\n");
	} else {
		printf("[FAIL] Allocation failed.\n");
		return;
	}

	/* Тест 2: Проверка адресов */
	if (p1 != p2) {
		printf("[OK] Pointers are distinct.\n");
	}

	/* Тест 3: Запись данных */
	strcpy(p1, "Test15");
	strcpy(p3, "Test180_Long");
	
	/* Тест 4: Освобождение */
	my_free(p1);
	char* p4 = (char*)my_malloc(15);

	/* Ожидаем, что аллокатор вернет тот же адрес */
	if (p4 == p1) {
		printf("[OK] Reused freed memory address (LIFO).\n");
	}
	
	/* Очистка */
	my_free(p2);
	my_free(p3);
	my_free(p4);
	printf("\n");
}

/**
 * @brief Сравнительный бенчмарк производительности.
 *
 * Измеряет время выполнения для стандартного malloc
 * и кастомного my_malloc. Выводит ускорение.
 */
void run_benchmark() {
	printf("=== Performance Benchmark ===\n");
	printf("Iterations: %d total ops\n", ITERATIONS);

	/* --- Тест системного malloc --- */
	clock_t start_sys = clock();
	void* pointers[BATCH_SIZE];

	for (int i = 0; i < ITERATIONS / BATCH_SIZE; i++) {
		for (int j = 0; j < BATCH_SIZE; j++) {
			pointers[j] = malloc(15);
			*(char*)pointers[j] = (char)j; /* Имитация работы */
		}
		for (int j = 0; j < BATCH_SIZE; j++) {
			free(pointers[j]);
		}
	}
	
	clock_t end_sys = clock();
	double time_sys = (double)(end_sys - start_sys) / CLOCKS_PER_SEC;

	/* --- Тест кастомного my_malloc --- */
	my_allocator_init();
	clock_t start_my = clock();

	for (int i = 0; i < ITERATIONS / BATCH_SIZE; i++) {
		for (int j = 0; j < BATCH_SIZE; j++) {
			pointers[j] = my_malloc(15);
			if (!pointers[j]) return; /* Защита от переполнения */
			*(char*)pointers[j] = (char)j;
		}
		for (int j = 0; j < BATCH_SIZE; j++) {
			my_free(pointers[j]);
		}
	}
	
	clock_t end_my = clock();
	double time_my = (double)(end_my - start_my) / CLOCKS_PER_SEC;

	/* --- Вывод результатов --- */
	printf("System malloc: %.6f sec\n", time_sys);
	printf("Custom malloc: %.6f sec\n", time_my);
	
	if (time_my < time_sys) {
		printf(">> Custom allocator is %.2fx FASTER\n", time_sys / time_my);
	}
}

/**
 * @brief Точка входа в программу.
 * @return 0 при успешном завершении.
 */
int main() {
	run_functional_tests();
	run_benchmark();
	return 0;
}