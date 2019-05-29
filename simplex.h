#pragma once

#include "utils.h"


struct Simplex {                                        // структура для симплексної таблиці
    float **matrix;                                     // таблиця
    int x_c;                                            // кількість коефіцієнтів
    int c_c;                                            // кілкість обмежень
    int w;                                              // кількість стовпців таблиці
    int h;                                              // кількість рядків таблиці
    int error;                                          // флаг помилки
};


struct SimplexOutput {                                  // структура рішення симлексу
    float *roots;                                       // оптимальні коефіцієнти
    int c;                                              // кількість коефіцієнтів
    float f;                                            // значення функції при цих коефіцієнтах
};


struct Simplex get_matrix(struct Input *input) {        // генерація таблиці для симплексу
    int w = input->x_c + input->h + 1;                  // кількість стовпців = кількість коефіцієнтів +
                                                        //      + кількість обмежень (з додатковими) + 1

    int h = input->h + 1;                               // кілкість рядків = кількість обмежень + 1

    float **matrix = malloc_2d_array(w, h);             // виділення пам'яті

    struct Simplex simplex;                             // створення структури для симплексу
    simplex.x_c = input->x_c;
    simplex.c_c = input->h;
    simplex.w = w;
    simplex.h = h;
    simplex.matrix = matrix;
    simplex.error = 0;

                                                        // створення таблиці

    for (int i = 0; i < input->h; i++) {                // копіювання коефіцієнтів обмежень
        for (int j = 0; j < input->x_c; j++)
            matrix[i][j] = input->constrains[i][j];
        matrix[i][w - 1] = input->constrains[i][input->x_c];
    }

    for (int j = 0; j < w; j++)                         // копіювання цільової функції
        matrix[h - 1][j] = j < input->x_c ? input->func[j] : 0;

    for (int i = 0; i < h; i++)                         // створення базисів
        for (int j = 0; j < input->h; j++)
            matrix[i][input->x_c + j] = i == j ? input->constrains[i][input->x_c + 1] : 0;

    return simplex;
}

                                                        // пошук головного стовпця та рядка
void find_pivot(struct Simplex *simplex, int *col_, int *row_) {
    int column = -1;
    int row = -1;

    for (int i = 0; i < simplex->w - 1; i++) {          // пошук стовпця
        if (simplex->matrix[simplex->h - 1][i] <= 0)    // тількі додатні значення
            continue;
        if (column == -1 ||                             // пошук мінімального значення в останньому рядку
            simplex->matrix[simplex->h - 1][i] <
            simplex->matrix[simplex->h - 1][column])
            column = i;
    }

    if (column == -1) {                                 // якшо немає додатніх значень - помилка
        simplex->error = 1;
        return;
    }

    for (int i = 0; i < simplex->h - 1; i++) {          // пошук рядка
        if (simplex->matrix[i][column] <= 0)            // тількі додатні значення
            continue;
        if (row == -1 ||                                // пошук мінімального відношення індексного елементу
                                                        // до вільного члена
            simplex->matrix[i][simplex->w - 1] / simplex->matrix[i][column] <
            simplex->matrix[row][simplex->w - 1] / simplex->matrix[row][column])
            row = i;
    }

    if (row == -1) {                                    // якшо немає додатніх значень - помилка
        simplex->error = 1;
        return;
    }

    for (int i = 0; i < simplex->h - 1; i++)            // якшо є більше одного підходящого рядка - помилка
        if (i != row &&
            simplex->matrix[i][simplex->w - 1] / simplex->matrix[i][column] ==
            simplex->matrix[row][simplex->w - 1] / simplex->matrix[row][column]
                ) {
            simplex->error = 1;
            return;
        }

    *row_ = row;
    *col_ = column;
}


void improve(struct Simplex *simplex) {                 // покращення плану
    int col;
    int row;

    find_pivot(simplex, &col, &row);                    // пошук стовпця та рядка
    if (simplex->error == 1)
        return;

    float pivot_value = simplex->matrix[row][col];      // індексне значення

    for (int j = 0; j < simplex->w; j++) {              // ділимо головний стовпець на індексне значення
        simplex->matrix[row][j] /= pivot_value;
    }

    for (int i = 0; i < simplex->h; i++) {              // віднімаємо від всіх рядків головний рядок
        if (i != row) {
            float k = simplex->matrix[i][col];
            for (int j = 0; j < simplex->w; j++)
                simplex->matrix[i][j] -= simplex->matrix[row][j] * k;
        }
    }
}


int is_ideal(struct Simplex *simplex) {                 // перевірка на оптимальність плану
    for (int i = 0; i < simplex->w - 1; i++)
        if (simplex->matrix[simplex->h - 1][i] > 0)     // план не оптимальний якщо є додатні члени в останньому рядку
            return 0;
    return 1;
}

                                                        // отримання коренів зі стовпця (якщо є)
float get_column_root(struct Simplex *simplex, int col) {
                                                        // якщо стовпець базисним, то коренем буде
    int piv_row = -1;                                   // вільний член що знаходится в тому ж рядку,
    int zero_count = 0;                                 // в якому міститься значення 1 цього стовпця

    for (int i = 0; i < simplex->h; i++) {              // перевірка що рядок базисний
        float el = simplex->matrix[i][col];
        if (el == 0) zero_count++;                      // підрахунок кількості нулів
        if (el == 1 || el == -1) piv_row = i;           // якщо стовпець містить -1 або 1 - він претиндент
    }

    if (zero_count != simplex->c_c || piv_row == -1)    // якщо стовпець не відповідає умові - далі не шукаємо
        return -1;

    if (simplex->matrix[piv_row][col] == -1) {          // якщо стовпець базисний але містить -1 замість 1
        simplex->error = 1;                             // то це помилка
        return -1;
    }
                                                        // інакше - коренем є вільний член в знайденному рядку
    return (float) simplex->matrix[piv_row][simplex->w - 1];
}

                                                        // пошук всіх коренів
void get_roots(struct Simplex *simplex, struct SimplexOutput *s_output) {
    int j = 0;
    for (int i = 0; i < simplex->w - 1; i++) {          // перебираємо всі стовпці
        float root = get_column_root(simplex, i);
        if (simplex->error == 1) return;
        if (root == -1) continue;
                                                        // якщо є корінь то записуємо його у відповідь
        s_output->roots[j++] = root;                    // індекс стовпця = індексу знайденного кореня
    }
                                                        // значення функції = значенню останнього рядка та стовпця * -1
    s_output->f = simplex->matrix[simplex->h - 1][simplex->w - 1] * -1;
}


struct SimplexOutput do_simplex(struct Input *input) {  // головна функція
    struct Simplex simplex = get_matrix(input);         // отримання матриці
    struct SimplexOutput s_output;                      // створення структури відповіді

    s_output.c = input->x_c;
    s_output.roots = malloc_1d_array(s_output.c);       // виділення пам'яті
    for (int i = 0; i < s_output.c; i++)                // заповення коефіцієнтів нулями
        s_output.roots[i] = 0;
    s_output.f = 0;

    while (!is_ideal(&simplex)) {                       // поки план не оптимальний
        improve(&simplex);                              // покращуємо його
        if (simplex.error == 1)                         // але якщо помилка - повертаемо відповідь з нулями
            return s_output;
    }
    get_roots(&simplex, &s_output);                     // заповнення коренів з матриці
    return s_output;                                    // повертаємо відповідь
}