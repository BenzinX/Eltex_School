## Программа синхронизации процессов через семафоры

Программа демонстрирует:
1. Генерацию случайных чисел в дочернем процессе
2. Передачу чисел через pipe в родительский процесс
3. Синхронизацию доступа к файлу через семафоры System V
4. Координацию чтения/записи между процессами

## Требования

- Linux
- CMake (версия 3.10 или выше)

## Установка

```bash
mkdir build && cd build
cmake ..
make
