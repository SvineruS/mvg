#include <stdio.h>

#include "simplex.h"
#include "mvg.h"


int main() {

    struct Input input = get_input_from_file();
    struct SimplexOutput s_output = do_simplex(&input);

    float **mc = get_start_mc(input.x_c);
    float *roots = mvg(s_output.roots, mc, &input);

    int sum = 0;
    for (int i = 0; i < input.x_c; i++) {
        printf("x%d = %d \n", i + 1, (int) roots[i]);
        sum += (int) roots[i] * (int) input.func[i];
    }
    printf("sum: %d ", sum);

    return 0;
}
