#include <stdio.h>
#include "simplex.h"
#include "mvg.h"



int main() {
    struct Input input = get_input_from_file();             // зібрати вхідні данні з файлу
    float *roots = do_simplex(&input);     // знаходження оптимального рішення

    if (roots == NULL) {
        printf("Система неверішувана");
        return 0;
    }

    printf("Оптимальне рішення: \n");
    print_roots(roots, input.x_c);

    roots = mvg(roots, &input);             // знаходження оптимального цілочисленного рішення

    printf("\n\nЦілочисленне рішення: \n");
    print_roots(roots, input.x_c);

    return 0;
}


