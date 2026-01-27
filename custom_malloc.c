#include "custom_malloc.h"
#include <stdint.h>

#define POOL_SIZE_15 2000
#define POOL_SIZE_180 500

typedef union Block15 {
	union Block15* next;
	char data[15];
	long align_dummy;
} Block15;

typedef union Block180 {
	union Block180* next;
	char data[180];
	long align_dummy;
} Block180;

static Block15 pool15[POOL_SIZE_15];
static Block180 pool180[POOL_SIZE_180];

static Block15* head15 = NULL;
static Block180* head180 = NULL;

static size_t idx15 = 0;
static size_t idx180 = 0;

/*
 * Сброс глобальных переменных аллокатора
 */
void my_allocator_init(void) {
	head15 = NULL;
	head180 = NULL;
	idx15 = 0;
	idx180 = 0;
}

/*
 * Реализация кастомного malloc
 *
 * @param size размер в байтах (поддерживается <= 15 и <= 180).
 * @return void* указатель на память.
 */
void* my_malloc(size_t size) {
	if (size <= 15) {
		if (head15) {
			Block15* ptr = head15;
			head15 = head15->next;
			return (void*)ptr;
		}
		if (idx15 < POOL_SIZE_15) {
			return (void*)&pool15[idx15++];
		}
		return NULL;
	}
    if (size <= 180) {
		if (head180) {
			Block180* ptr = head180;
			head180 = head180->next;
			return (void*)ptr;
		}
		if (idx180 < POOL_SIZE_180) {
			return (void*)&pool180[idx180++];
		}
		return NULL;
	}
	return NULL;
}

/*
 * Реализация кастомного free
 *
 * @param ptr указатель, который нужно вернуть в пул.
 */
void my_free(void* ptr) {
	if (!ptr) {
		return;
	}
	
	uintptr_t addr = (uintptr_t)ptr;
	uintptr_t start15 = (uintptr_t)pool15;
	uintptr_t end15 = (uintptr_t)(pool15 + POOL_SIZE_15);

	if (addr >= start15 && addr < end15) {
		Block15* block = (Block15*)ptr;
		block->next = head15;
		head15 = block;
		return;
	}

	uintptr_t start180 = (uintptr_t)pool180;
	uintptr_t end180 = (uintptr_t)(pool180 + POOL_SIZE_180);

	if (addr >= start180 && addr < end180) {
		Block180* block = (Block180*)ptr;
		block->next = head180;
		head180 = block;
		return;
	}
}