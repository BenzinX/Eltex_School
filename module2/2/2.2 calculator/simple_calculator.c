#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>

// Прототипы функций калькулятора
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);
double power(double a, double b);

int main() {
    setlocale(LC_ALL, "Russian");
    while (1) {
        printf_s("Выберите операцию\n");
        printf_s("1. Сложение\n");
        printf_s("2. Вычитание\n");
        printf_s("3. Умножение\n");
        printf_s("4. Деление\n");
        printf_s("5. Возведение в степень\n");
        int i;
        double a, b;
        scanf_s("%d", &i);
        switch (i)
        {
        case 1:
            printf_s("Введите два операнда: ");
            scanf_s("%lf %lf", &a, &b);
            printf_s("%lf\n", add(a, b));
            break;
        case 2:
            printf_s("Введите два операнда: ");
            scanf_s("%lf %lf", &a, &b);
            printf_s("%lf\n", subtract(a, b));
            break;
        case 3:
            printf_s("Введите два операнда: ");
            scanf_s("%lf %lf", &a, &b);
            printf_s("%lf\n", multiply(a, b));
            break;
        case 4:
            printf_s("Введите два операнда: ");
            scanf_s("%lf %lf", &a, &b);
            printf_s("%lf\n", divide(a, b));
            break;
        case 5:
            printf_s("Введите два операнда: ");
            scanf_s("%lf %lf", &a, &b);
            printf_s("%lf\n", power(a, b));
            break;
        default:
            printf_s("Сделайте правильный выбор\n");
            break;
        }
    }

    return 0;
}

// Основные функции калькулятора
double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    if (b == 0) {
        printf_s("Ошибка: деление на ноль!\n");
        return 0;
    }
    return a / b;
}

double power(double a, double b) {
    return pow(a, b);
}
