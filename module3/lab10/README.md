## Программа синхронизации процессов через POSIX семафоры

Программа демонстрирует:
1. Взаимодействие между родительским и дочерним процессами
2. Синхронизацию через именованные семафоры POSIX
3. Координацию доступа к общему файлу
4. Передачу данных через pipe

## Требования

- Linux (с поддержкой POSIX семафоров)
- CMake (версия 3.10 или выше)
- Библиотека `rt` (обычно установлена по умолчанию)

## Установка

```bash
mkdir build && cd build
cmake ..
make
