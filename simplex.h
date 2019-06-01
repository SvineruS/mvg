#pragma once
#include "utils.h"


struct Simplex {                                        // структура для симплексної таблиці
    float **matrix;                                     // таблиця
    int x_c;                                            // кількість коефіцієнтів
    int c_c;                                            // кілкість обмежень
    int w;                                              // кількість стовпців таблиці
    int h;                                              // кількість рядків таблиці
    int error;                                          // флаг помилки
    int type;
};


struct Simplex get_matrix(struct Input *input) {        // генерація таблиці для симплексу
    int h = input->h + 1;                               // кілкість рядків = кількість обмежень + 1
    int w = input->x_c + input->h + 1;                  // стовпці = коефіцієнти + обмеження + 1
    float **matrix = malloc_2d_array(w, h);             // виділення пам'яті
    struct Simplex simplex;                             // створення структури для симплексу
    simplex.x_c = input->x_c;
    simplex.c_c = input->h;
    simplex.w = w;
    simplex.h = h;
    simplex.matrix = matrix;
    simplex.error = 0;
    simplex.type = input->type;
                                                        // створення таблиці
    for (int i = 0; i < input->h; i++) {                // копіювання коефіцієнтів обмежень
        float k = input->constrains[i][input->x_c + 1];
        for (int j = 0; j < input->x_c; j++)
            matrix[i][j] = input->constrains[i][j] * k;
        matrix[i][w - 1] = input->constrains[i][input->x_c] * k;
    }

    for (int j = 0; j < w; j++)                         // копіювання цільової функції
        matrix[h - 1][j] = j < input->x_c ? input->func[j] * input->type : 0;

    for (int i = 0; i < h; i++)                         // створення базисів
        for (int j = 0; j < input->h; j++)
            matrix[i][input->x_c + j] = i == j ? 1 : 0;

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

//    for (int i = 0; i < simplex->h - 1; i++)            // якшо є більше одного підходящого рядка - помилка
//        if (i != row &&
//            simplex->matrix[i][simplex->w - 1] / simplex->matrix[i][column] ==
//            simplex->matrix[row][simplex->w - 1] / simplex->matrix[row][column]
//                ) {
//            simplex->error = 1;
//            return;
//        }

    *row_ = row;
    *col_ = column;
}


void improve(struct Simplex *simplex, int col, int row) {                 // покращення плану
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

void fix_matrix(struct Simplex *simplex) {
    int col = -1;
    int row = -1;

    for (int i = 0; i < simplex->h - 1; i++) {          // пошук рядка
        if (simplex->matrix[i][simplex->w - 1] >= 0)     // тількі від'ємні значення
            continue;
        if (row == -1 ||                                // пошук мінімального значення в останньому стовпці
            simplex->matrix[i][simplex->w - 1] <
            simplex->matrix[row][simplex->w - 1] )
            row = i;
    }
    if (row == -1)
        return;

    for (int i = 0; i < simplex->w - 1; i++) {          // пошук стовпця
        if (simplex->matrix[row][i] >= 0)     // тількі від'ємні значення
            continue;
        if (col == -1 ||                                // пошук мінімального значення в останньому стовпці
            simplex->matrix[row][i] <
            simplex->matrix[row][col] )
            col = i;
    }
    if (col == -1) {
        simplex->error = 1;
        return;
    }

    improve(simplex, col, row);
    fix_matrix(simplex);
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
void get_roots(struct Simplex *simplex, float *roots) {
    for (int i = 0; i < simplex->w - 1; i++) {          // перебираємо всі стовпці
        float root = get_column_root(simplex, i);
        if (simplex->error == 1) return;
        if (root == -1) continue;

        if (i < simplex->x_c)
            roots[i] = root;                    // індекс стовпця = індексу знайденного кореня

    }
    roots[simplex->x_c] = simplex->matrix[simplex->h-1][simplex->w-1] * simplex->type * -1;
}


float* do_simplex(struct Input *input) {  // головна функція
    struct Simplex simplex = get_matrix(input);         // отримання матриці

    float *roots = malloc_1d_array(input->x_c+1);       // виділення пам'яті
    for (int i = 0; i < input->x_c; i++)                // заповення коефіцієнтів нулями
        roots[i] = 0;

    fix_matrix(&simplex);
    if (simplex.error == 1)
        return NULL;

    int col, row;

    while (!is_ideal(&simplex)) {                       // поки план не оптимальний

        find_pivot(&simplex, &col, &row);                    // пошук стовпця та рядка
        if (simplex.error == 1)
            return NULL;
        improve(&simplex, col, row);                              // покращуємо його
        if (simplex.error == 1)                         // але якщо помилка - повертаемо відповідь з нулями
            return NULL;
    }

    get_roots(&simplex, roots);                     // заповнення коренів з матриці
    return roots;                                    // повертаємо відповідь
}