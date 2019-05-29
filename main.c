#include <stdio.h>
#include "simplex.h"
#include "mvg.h"


int main() {
    struct Input input = get_input_from_file();             // зібрати вхідні данні з файлу
    struct SimplexOutput s_output = do_simplex(&input);     // знаходження оптимального рішення
    float *roots = mvg(s_output.roots, &input);             // знаходження оптимального цілочисленного рішення

    int sum = 0;
    for (int i = 0; i < input.x_c; i++) {                   // друкування відповіді
        printf("x%d = %d \n", i + 1, (int) roots[i]);
        sum += (int) roots[i] * (int) input.func[i];
    }
    printf("sum: %d ", sum);
    return 0;
}
