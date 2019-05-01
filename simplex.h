#pragma once

#include "utils.h"


struct Simplex {
    float **matrix;
    int x_c;
    int c_c;
    int w;
    int h;
    int error;
};


struct SimplexOutput {
    float *roots;
    int c;
    float f;
};


struct Simplex get_matrix(struct Input *input) {

    int w = input->x_c + input->h + 1;
    int h = input->h + 1;
    float **matrix = malloc_2d_array(w, h);

    struct Simplex simplex;
    simplex.x_c = input->x_c;
    simplex.c_c = input->h;
    simplex.w = w;
    simplex.h = h;
    simplex.matrix = matrix;
    simplex.error = 0;


    for (int i = 0; i < input->h; i++) {                                        // copy main constrains
        for (int j = 0; j < input->x_c; j++)                                    // copy 0 -- x_c columns
            matrix[i][j] = input->constrains[i][j];
        matrix[i][w - 1] = input->constrains[i][input->x_c];                        // copy last column
    }

    for (int j = 0; j < w; j++)
        matrix[h - 1][j] = j < input->x_c ? input->func[j] : 0;                   // lasts row for function

    for (int i = 0; i < h; i++)                                                    //
        for (int j = 0; j < input->h; j++)                                        //
            matrix[i][input->x_c + j] = i == j ? input->constrains[i][input->x_c + 1] : 0;   // copy w-end columns

//    print_2d_array(simplex.matrix, simplex.w, simplex.h);

    return simplex;
}


void find_pivot(struct Simplex *simplex, int *col_, int *row_) {
    int column = -1;
    int row = -1;

    for (int i = 0; i < simplex->w - 1; i++) {
        if (simplex->matrix[simplex->h - 1][i] <= 0)
            continue;
        if (column == -1 ||
            simplex->matrix[simplex->h - 1][i] <
            simplex->matrix[simplex->h - 1][column])     // search main column
            column = i;
    }

    if (column == -1) {
        simplex->error = 1;
        return;
    }

    for (int i = 0; i < simplex->h - 1; i++) {
        if (simplex->matrix[i][column] <= 0)
            continue;
        if (row == -1 ||
            simplex->matrix[i][simplex->w - 1] / simplex->matrix[i][column] <
            simplex->matrix[row][simplex->w - 1] / simplex->matrix[row][column])
            row = i;                                // search minimum ratio: last column / main column
    }

    if (row == -1) {
        simplex->error = 1;
        return;
    }

    for (int i = 0; i < simplex->h - 1; i++)
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


void improve(struct Simplex *simplex) {
    int col;
    int row;

    find_pivot(simplex, &col, &row);
    if (simplex->error == 1)
        return;


    float pivot_value = simplex->matrix[row][col];

    for (int j = 0; j < simplex->w; j++) {
        simplex->matrix[row][j] /= pivot_value;
    }

    for (int i = 0; i < simplex->h; i++) {
        if (i != row) {
            float k = simplex->matrix[i][col];
            for (int j = 0; j < simplex->w; j++) {
                simplex->matrix[i][j] -= simplex->matrix[row][j] * k;
            }

        }
    }

//    printf("pivot c: %d r: %d \n", col, row);
//    print_2d_array(simplex->matrix, simplex->w, simplex->h);
}


int is_ideal(struct Simplex *simplex) {
    for (int i = 0; i < simplex->w - 1; i++)
        if (simplex->matrix[simplex->h - 1][i] > 0)
            return 0;
    return 1;
}


float get_column_root(struct Simplex *simplex, int col) {
    int piv_row = -1;
    int zero_count = 0;

    for (int i = 0; i < simplex->h; i++) {
        float el = simplex->matrix[i][col];
        if (el == 0) zero_count++;
        if (el == 1 || el == -1) piv_row = i;
    }

    if (zero_count != simplex->c_c ||
        piv_row == -1)
        return -1;

    if (simplex->matrix[piv_row][col] == -1) {
        simplex->error = 1;
        return -1;
    }

    return (float) simplex->matrix[piv_row][simplex->w - 1];
}


void get_roots(struct Simplex *simplex, struct SimplexOutput *s_output) {
    int j = 0;
    for (int i = 0; i < simplex->w - 1; i++) {
        float root = get_column_root(simplex, i);
        if (simplex->error == 1) return;
        if (root == -1) continue;

        s_output->roots[j++] = root;
    }
    s_output->f = simplex->matrix[simplex->h - 1][simplex->w - 1] * -1;

}


struct SimplexOutput do_simplex(struct Input *input) {
    struct Simplex simplex = get_matrix(input);
    struct SimplexOutput s_output;

    s_output.c = input->x_c;
    s_output.roots = malloc_1d_array(s_output.c);
    for (int i = 0; i < s_output.c; i++)
        s_output.roots[i] = 0;
    s_output.f = 0;

    while (!is_ideal(&simplex)) {
        improve(&simplex);
        if (simplex.error == 1) {
//            printf("SIMPLEX ERROR");
            return s_output;
        }
    }
    get_roots(&simplex, &s_output);
    return s_output;
}