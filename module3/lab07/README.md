## Чат на POSIX Message Queues

Две программы для обмена сообщениями через механизм очередей сообщений POSIX.

## Требования

- Linux (с поддержкой POSIX message queues)
- CMake (версия 3.10 или выше)
- Библиотека `rt` (обычно установлена по умолчанию)

## Установка

```bash
mkdir build && cd build
cmake ..
make
