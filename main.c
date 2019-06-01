#include <stdio.h>
#include "simplex.h"
#include "mvg.h"



void print_input(struct Input *input) {                         // друкування вхідних данних
    printf("Вхідні данні: \n");
    printf("func: ");
    for (int i=0; i<input->x_c-1; i++)
        printf("%d*x%d + ", (int)input->func[i], i+1);
    printf("%d*x%d -> ", (int)input->func[input->x_c-1], input->x_c);
    printf(input->type == 1 ? "Максимізація\n" : "Мінімізація\n");

    for (int j=0; j<input->c_c; j++) {
        for (int i=0; i<input->x_c-1; i++)
            printf("%d*x%d + ", (int)input->constrains[j][i], i+1);
        printf("%d*x%d ", (int)input->constrains[j][input->x_c-1], input->x_c);
        printf(input->constrains[j][input->x_c+1] == 1 ? "<=" : ">=");
        printf(" %d \n", (int)input->constrains[j][input->x_c]);
    }
    printf("\n");
}


void print_output(float *roots, int c) {                        // друкування відповіді
    for (int i = 0; i < c; i++)
        printf("x%d = %f \n", i + 1, roots[i]);
    printf("sum: %f ", roots[c]);
}


int main() {
    struct Input input = get_input_from_file();                 // зібрати вхідні данні з файлу
    print_input(&input);

    float *roots = do_simplex(&input);                          // знаходження оптимального рішення
    if (roots == NULL) {
        printf("Система неверішувана");
        return 0;
    }
    printf("Оптимальне рішення: \n");
    print_output(roots, input.x_c);


    roots = mvg(roots, &input);                                 // знаходження оптимального цілочисленного рішення
    printf("\n\nЦілочисленне рішення: \n");
    print_output(roots, input.x_c);

    return 0;
}


