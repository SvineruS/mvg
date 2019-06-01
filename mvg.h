#pragma once
#include "utils.h"
#include "simplex.h"


void get_mc(int i, float r, float **mc1, float **mc2) {         // отримання допоміжного обмеження
    mc1[i][0] = (int) r;                                        // перше обмеження: x < [xk]
    mc1[i][1] = 1;
    mc2[i][0] = (int) r + 1;                                    // друге обмеження: x > [xk] + 1
    mc2[i][1] = -1;
}

                                                                // обгортка для симплекс методу
float* simplex_wrap(struct Input *input, float **mc) {
    for (int i = 0; i < input->x_c; i++) {                      // додає додаткові обмеження до початкових
        int r = i + input->c_c;                                 //   вхідних данних
        input->constrains[r][i] = 1;
        input->constrains[r][input->x_c + 1] = mc[i][1];
        input->constrains[r][input->x_c] = mc[i][0];
    }
    return do_simplex(input);                                   // безпосередньо симплекс метод
}


float *mvg_(float *roots, float **mc, struct Input *input) {    // метод віток та границь (рекурсивний)

    for (int i = 0; i < input->x_c; i++) {
        float r = roots[i];
        if (is_int(r))                                          // серед усіх коефіцієнтів шукаємо не цілочисленний
            continue;

        float **mc1 = copy_2d_array(mc, 2, input->x_c);         // копіюємо попередні обмеження
        float **mc2 = copy_2d_array(mc, 2, input->x_c);
        get_mc(i, r, mc1, mc2);                                 // генеруємо 2 нових

        float* r1 = simplex_wrap(input, mc1);                   // шукаємо оптимальне рішення з новими обмеженнями
        float* r2 = simplex_wrap(input, mc2);

        if (r1 == NULL && r2 == NULL) return NULL;              // якщо у обох підзадач немає рішень - повертаємо NULL
        else if (r1 == NULL) return mvg_(r2, mc2, input);       // якщо рішення немає у однієї з підзадач -
        else if (r2 == NULL) return mvg_(r1, mc1, input);       //    розглядаємо іншу
        else {                                                  // якщо у обох підзадач є рішення
            float f1 = r1[input->x_c] * input->type;
            float f2 = r2[input->x_c] * input->type;

            if (f1 > f2)                                        // обираємо підзадачу з більшим (меншим) значенням
                return mvg_(r1, mc1, input);                    // цільової функції та рекурсивно викликаємо
            else                                                // метод віток і границь для нових коренів
                return mvg_(r2, mc2, input);
        }
    }

    return roots;                                               // якщо всі коефіцієнті цілочисленні повертаємо їх
}


float *mvg(float *roots, struct Input *input) {                 // метод віток та границь
    float **mc = malloc_2d_array(2, input->x_c);                // генерація початкових додаткових обмежень
    for (int i = 0; i < input->x_c; i++) {                      // типу xi > 0
        mc[i][0] = 0;
        mc[i][1] = -1;
    }
    return mvg_(roots, mc, input);                              // рекурсивний метод віток і границь
}

