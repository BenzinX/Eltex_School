## Генератор случайных чисел

Программа генерирует указанное количество случайных чисел с использованием:
- Межпроцессного взаимодействия через pipe
- Разделения на родительский и дочерний процессы
- Сохранения результатов в файл

## Требования

- Linux
- CMake (версия 3.10 или выше)

## Установка

```bash
mkdir build && cd build
cmake ..
make
