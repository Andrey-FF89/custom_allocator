/**
 * @file custom_malloc.c
 * @brief Реализация кастомного аллокатора памяти.
 */

#include "custom_malloc.h"
#include <stdint.h>

/** @brief Максимальное количество блоков размером 15 байт. */
#define POOL_SIZE_15 2000

/** @brief Максимальное количество блоков размером 180 байт. */
#define POOL_SIZE_180 500

/**
 * @struct Block15
 * @brief Структура блока памяти для 15 байт.
 *
 * @details Используется union для перекрытия памяти: когда блок свободен,
 * память используется для хранения указателя на следующий элемент.
 * Когда занят — для данных пользователя.
 */
typedef union Block15 {
	/** @brief Указатель на следующий свободный блок. */
	union Block15* next;
	
	/** @brief Данные пользователя. */
	char data[15];
	
	/**
	 * @brief Фиктивное поле для принудительного выравнивания.
	 * @note Гарантирует корректную работу на 32-битных и 64-битных
	 * архитектурах, выравнивая начало блока по границе машинного слова.
	 */
	long align_dummy;
} Block15;

/**
 * @struct Block180
 * @brief Структура блока памяти для 180 байт.
 */
typedef union Block180 {
	union Block180* next;   ///< Ссылка на следующий элемент стека
	char data[180];         ///< Данные
	long align_dummy;       ///< Выравнивание
} Block180;

/* --- Статические пулы памяти --- */

static Block15 pool15[POOL_SIZE_15];     ///< Пул блоков 15 байт
static Block180 pool180[POOL_SIZE_180];  ///< Пул блоков 180 байт

/* --- Головы free-листов --- */

static Block15* head15 = NULL;    ///< Вершина стека свободных блоков (15 байт)
static Block180* head180 = NULL;  ///< Вершина стека свободных блоков (180 байт)

/* --- Индексы для первичного распределения --- */

static size_t idx15 = 0;   ///< Индекс следующего нового блока (15 байт)
static size_t idx180 = 0;  ///< Индекс следующего нового блока (180 байт)


void my_allocator_init(void) {
	head15 = NULL;
	head180 = NULL;
	idx15 = 0;
	idx180 = 0;
}

void* my_malloc(size_t size) {
	/* Логика для малого пула (15 байт) */
	if (size <= 15) {
		/* 1. Пытаемся взять из списка возвращенных (LIFO) */
		if (head15) {
			Block15* ptr = head15;
			head15 = head15->next;
			return (void*)ptr;
		}
		/* 2. Если стек пуст, берем новый блок из массива */
		if (idx15 < POOL_SIZE_15) {
			return (void*)&pool15[idx15++];
		}
		return NULL; /* Память исчерпана */
	} 
	/* Логика для большого пула (180 байт) */
	else if (size <= 180) {
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

	return NULL; /* Запрошен неверный размер */
}

void my_free(void* ptr) {
	if (!ptr) {
		return;
	}
	
	/* Используем uintptr_t для безопасной адресной арифметики */
	uintptr_t addr = (uintptr_t)ptr;

	/* Проверка диапазона пула 15 байт */
	uintptr_t start15 = (uintptr_t)pool15;
	uintptr_t end15 = (uintptr_t)(pool15 + POOL_SIZE_15);

	if (addr >= start15 && addr < end15) {
		Block15* block = (Block15*)ptr;
		block->next = head15; /* Push в стек */
		head15 = block;
		return;
	}

	/* Проверка диапазона пула 180 байт */
	uintptr_t start180 = (uintptr_t)pool180;
	uintptr_t end180 = (uintptr_t)(pool180 + POOL_SIZE_180);

	if (addr >= start180 && addr < end180) {
		Block180* block = (Block180*)ptr;
		block->next = head180;
		head180 = block;
		return;
	}
	
	/* Если адрес не принадлежит ни одному пулу — игнорируем */
}