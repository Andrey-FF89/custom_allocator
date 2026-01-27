#ifndef CUSTOM_MALLOC_H
#define CUSTOM_MALLOC_H

#include <stddef.h>

/*
 * Сброс состояния аллокатора (для тестов)
 */
void my_allocator_init(void);

/*
 * Выделение памяти фиксированного размера
 *
 * @param size требуемый размер памяти в байтах.
 * @return указатель на выделенный блок или NULL, если памяти нет.
 */
void* my_malloc(size_t size);

/*
 * Освобождение памяти
 *
 * @param ptr указатель на блок памяти для освобождения.
 */
void my_free(void* ptr);

#endif