#pragma once

#include <stdlib.h>

struct Input {
    float **constrains;        // x_c + 2  *   c_c
    float *func;               // x_c      *   1

    int x_c;
    int c_c;
    int h;
};


void print_1d_array(float *arr, int w) {
    for (int i = 0; i < w; i++)
        printf("%f\t ", arr[i]);
}


void print_2d_array(float **arr, int w, int h) {
    printf("\n");
    for (int i = 0; i < h; i++) {
        print_1d_array(arr[i], w);
        printf("\n");
    }
}


float *malloc_1d_array(int w) {
    return (float *) malloc(w * sizeof(float));
}

float **malloc_2d_array(int w, int h) {
    float **arr = (float **) malloc(h * sizeof(float *));
    for (int i = 0; i < h; i++)
        arr[i] = malloc_1d_array(w);
    return arr;
}


float *copy_1d_array(float *orig, int w) {
    float *copy = malloc_1d_array(w);
    for (int i = 0; i < w; i++)
        copy[i] = orig[i];
    return copy;
}

float **copy_2d_array(float **orig, int w, int h) {
    float **copy = malloc_2d_array(w, h);
    for (int i = 0; i < w; i++)
        copy[i] = copy_1d_array(orig[i], w);
    return copy;
}


struct Input get_input_from_file() {
    struct Input input;

    int ti;
    char tc;

    FILE *fptr;
    if ((fptr = fopen("input.txt", "r")) == NULL) {
        printf("Error! opening file");
        exit(1);
    }

    fscanf(fptr, "%d", &input.x_c);
    fscanf(fptr, "%d", &input.c_c);
    input.h = input.x_c + input.c_c;


    float **constrains = malloc_2d_array(input.x_c + 2, input.h);
    float *func = malloc_1d_array(input.x_c);
    input.constrains = constrains;
    input.func = func;

    for (int i = 0; i < input.x_c; i++) {
        fscanf(fptr, "%d", &ti);
        func[i] = (float) ti;
    }

    for (int i = 0; i < input.c_c; i++) {
        for (int j = 0; j < input.x_c; j++) {
            fscanf(fptr, "%d", &ti);
            constrains[i][j] = (float) ti;
        }

        fscanf(fptr, " %c", &tc);
        if (tc == '<') constrains[i][input.x_c + 1] = 1;
        else if (tc == '>') constrains[i][input.x_c + 1] = -1;

        fscanf(fptr, "%d", &ti);
        constrains[i][input.x_c] = (float) ti;
    }


    for (int i = 0; i < input.x_c; i++) {
        int r = i + input.c_c;
        for (int j = 0; j < input.x_c + 2; j++)
            constrains[r][j] = 0;

        constrains[r][i] = 1;
        constrains[r][input.x_c + 1] = -1;

    }


    fclose(fptr);

    return input;

}


int is_int(float a) {
    int b = (int) a;
    return (b - a == 0) ? 1 : 0;
}