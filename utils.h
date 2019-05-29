#pragma once

#include <stdlib.h>

struct Input {                                          // структура вхідних данних
    float **constrains;                                 // обмеження       [ x_c + 2 ;  c_c ]
    float *func;                                        // цільова функція [ x_c     ; 1    ]

    int x_c;                                            // кількість коефіцієнтів
    int c_c;                                            // кількість обмежень
    int h;
};


void print_1d_array(float *arr, int w) {                // друкування одновимірного массиву
    for (int i = 0; i < w; i++)
        printf("%f\t ", arr[i]);
}


void print_2d_array(float **arr, int w, int h) {        // друкування двувимірного массиву
    printf("\n");
    for (int i = 0; i < h; i++) {
        print_1d_array(arr[i], w);
        printf("\n");
    }
}


float *malloc_1d_array(int w) {                         // виділення пам'яті на одномірний массив
    return (float *) malloc(w * sizeof(float));
}

float **malloc_2d_array(int w, int h) {                 // виділення пам'яті на двумірний массив
    float **arr = (float **) malloc(h * sizeof(float *));
    for (int i = 0; i < h; i++)
        arr[i] = malloc_1d_array(w);
    return arr;
}


float *copy_1d_array(float *orig, int w) {              // дублювання одновимірного массиву
    float *copy = malloc_1d_array(w);
    for (int i = 0; i < w; i++)
        copy[i] = orig[i];
    return copy;
}

float **copy_2d_array(float **orig, int w, int h) {     // дублювання двувимірного массиву
    float **copy = malloc_2d_array(w, h);
    for (int i = 0; i < w; i++)
        copy[i] = copy_1d_array(orig[i], w);
    return copy;
}


struct Input get_input_from_file() {                    // читання вхідних данних з файлу
    struct Input input;

    int ti;                                             // тимчасові змінні
    char tc;

    FILE *fptr;
    if ((fptr = fopen("input.txt", "r")) == NULL) {     // якщо немає файлу - помилка
        printf("Error! opening file");
        exit(1);
    }

    fscanf(fptr, "%d", &input.x_c);                     // зчитування кількості коефіцієнтів
    fscanf(fptr, "%d", &input.c_c);                     // зчитування кількості обмежень
    input.h = input.x_c + input.c_c;

                                                        // виділення пам'яті для обмежень та функції
    float **constrains = malloc_2d_array(input.x_c + 2, input.h);
    float *func = malloc_1d_array(input.x_c);
    input.constrains = constrains;
    input.func = func;

    for (int i = 0; i < input.x_c; i++) {               // зчитування функції
        fscanf(fptr, "%d", &ti);
        func[i] = (float) ti;
    }

    for (int i = 0; i < input.c_c; i++) {               // зчитування обмежень
        for (int j = 0; j < input.x_c; j++) {           // коефіцієнти
            fscanf(fptr, "%d", &ti);
            constrains[i][j] = (float) ti;
        }

        fscanf(fptr, " %c", &tc);                       // знак більше або менше
        if (tc == '<') constrains[i][input.x_c + 1] = 1;
        else if (tc == '>') constrains[i][input.x_c + 1] = -1;

        fscanf(fptr, "%d", &ti);                        // вільний член
        constrains[i][input.x_c] = (float) ti;
    }


    for (int i = 0; i < input.x_c; i++) {               // додаткові обмеження
        int r = i + input.c_c;                          // що всі коефіцієнти > 0
        for (int j = 0; j < input.x_c + 2; j++)
            constrains[r][j] = 0;

        constrains[r][i] = 1;
        constrains[r][input.x_c + 1] = -1;

    }

    fclose(fptr);
    return input;
}


int is_int(float a) {                                   // перевірка числа на цілочисленність
    int b = (int) a;
    return (b - a == 0) ? 1 : 0;
}

