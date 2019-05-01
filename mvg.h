#pragma once

#include "utils.h"
#include "simplex.h"


float **get_start_mc(int x_c) {
    float **mc = malloc_2d_array(2, x_c);
    for (int i = 0; i < x_c; i++) {
        mc[i][0] = 0;
        mc[i][1] = -1;
    }
    return mc;
}


void get_mc(int i, float r, float **mc1, float **mc2) {
    int l = (int) r;
    int b = (int) r + 1;

    if (b < l) {
        int temp = b;
        b = l;
        l = temp;
    }

    mc1[i][0] = l;
    mc1[i][1] = 1;

    mc2[i][0] = b;
    mc2[i][1] = -1;

}


struct SimplexOutput simplex_wrap(struct Input *input, float **mc) {
    for (int i = 0; i < input->x_c; i++) {
        int r = i + input->c_c;
        input->constrains[r][i] = 1;
        input->constrains[r][input->x_c + 1] = mc[i][1];
        input->constrains[r][input->x_c] = mc[i][0];
    }
    return do_simplex(input);
}


float *mvg(float *roots, float **mc, struct Input *input) {

    for (int i = 0; i < input->x_c; i++) {
        float r = roots[i];

        if (is_int(r))
            continue;

        float **mc1 = copy_2d_array(mc, 2, input->x_c);
        float **mc2 = copy_2d_array(mc, 2, input->x_c);
        get_mc(i, r, mc1, mc2);

        struct SimplexOutput so1 = simplex_wrap(input, mc1);
        struct SimplexOutput so2 = simplex_wrap(input, mc2);


        if (so1.f > so2.f)
            return mvg(so1.roots, mc1, input);
        else
            return mvg(so2.roots, mc2, input);

    }
    return roots;
}



