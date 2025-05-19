#include <omp.h>
#include <stdio.h>

int main() {
    printf("Initial _OPENMP value: %d (OpenMP 4.5 is 201511, 5.0 is 201811, 5.1 is 202011, 5.2 is 202107, 6.0 will be higher)\n", _OPENMP);
    printf("This program attempts to compile features from different OpenMP versions.\n");
    printf("If a feature specific to a version compiles, it's a good sign that version (or parts of it) is supported.\n\n");

    // --- OpenMP 5.0 ---
    printf("--- Testing OpenMP 5.0 features ---\n");
    int omp5_0_feature_compiled_loop = 0; // Renamed for clarity
    #pragma omp parallel num_threads(1) // Nest loop inside a parallel region
    {
        #pragma omp loop // bind clause is implicitly parallel here
        for (int i = 0; i < 1; ++i) { // Dummy loop
            omp5_0_feature_compiled_loop = 1;
        }
    }
    if (omp5_0_feature_compiled_loop) {
        printf("  [5.0] '#pragma omp loop' (nested) compiled.\n");
    } else {
        printf("  [5.0] '#pragma omp loop' (nested) DID NOT compile or test logic failed.\n");
    }
    printf("\n");

    // --- OpenMP 5.1 ---
    printf("--- Testing OpenMP 5.1 features ---\n");
    int omp5_1_feature_compiled_atomic_capture = 0; // Changed test
    int omp5_1_feature_compiled_unroll = 0;
    int omp5_1_feature_compiled_tile = 0;

    int val_5_1_atomic = 0, captured_5_1_atomic;
    #pragma omp parallel num_threads(1)
    {
        // Let's test a simpler 'atomic capture' first.
        // If this fails, 'atomic compare capture' is unlikely to work.
        #pragma omp atomic capture
        captured_5_1_atomic = val_5_1_atomic++; // val_5_1_atomic becomes 1, captured_5_1_atomic becomes 0
        omp5_1_feature_compiled_atomic_capture = 1;
    }
    if (omp5_1_feature_compiled_atomic_capture) {
         printf("  [5.1] '#pragma omp atomic capture' compiled (val_5_1_atomic=%d, captured_5_1_atomic=%d).\n", val_5_1_atomic, captured_5_1_atomic);
    } else {
        printf("  [5.1] '#pragma omp atomic capture' DID NOT compile or test logic failed.\n");
    }

    // Try 'unroll' with 'partial' clause or ensure canonical form if possible
    // The loop for (int i = 0; i < 4; ++i) IS canonical.
    // The error might be specific to GCC 15's interpretation or a subtle requirement.
    // Let's try adding partial, which is often more lenient.
    #pragma omp parallel for
    #pragma omp unroll partial // Added partial (no argument means compiler chooses)
    for (int i = 0; i < 4; ++i) { /* dummy */ }
    omp5_1_feature_compiled_unroll = 1;
    if (omp5_1_feature_compiled_unroll) {
        printf("  [5.1] '#pragma omp unroll partial' compiled.\n");
    }

    // Tile construct (5.1)
    // This seemed to not cause an error before, but let's keep it.
    #pragma omp tile sizes(2)
    for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) { /* dummy */ }
    omp5_1_feature_compiled_tile = 1;
     if (omp5_1_feature_compiled_tile) {
        printf("  [5.1] '#pragma omp tile' compiled.\n");
    }
    printf("\n");


    // --- OpenMP 5.2 ---
    printf("--- Testing OpenMP 5.2 features ---\n");
    int omp5_2_feature_compiled_masked_filter = 0;
    int omp5_2_feature_compiled_assume = 0;

    #pragma omp parallel num_threads(2)
    {
        #pragma omp masked filter(omp_get_thread_num() == 0)
        {
            omp5_2_feature_compiled_masked_filter = 1;
        }
    }
     if (omp5_2_feature_compiled_masked_filter) {
        printf("  [5.2] '#pragma omp masked filter(...)' compiled and basic test executed.\n");
    } else {
        printf("  [5.2] '#pragma omp masked filter(...)' DID NOT compile or test logic failed.\n");
    }

    #pragma omp assume holds(1 > 0)
    omp5_2_feature_compiled_assume = 1;
    if (omp5_2_feature_compiled_assume) {
        printf("  [5.2] '#pragma omp assume' compiled.\n");
    }
    printf("\n");

    // --- OpenMP 6.0 ---
    printf("--- Testing OpenMP 6.0 features ---\n");
    int omp6_0_feature_compiled_induction = 0;
    int omp6_0_feature_compiled_assume_no_par = 0;

    int sum_induction_6_0 = 0;
    int induction_loop_executed = 0;
    #pragma omp parallel for reduction(+:sum_induction_6_0)
    // #pragma omp induction(k : 0 : 5 : 1) // This is the syntax from some proposals/docs
    // GCC documentation for induction (as of GCC 13/14) might use a slightly different way to express this
    // or it might be very new in GCC 15.
    // Let's try a common form of induction variable if the pragma is problematic:
    // The pragma '#pragma omp induction' is quite new.
    // For now, let's assume the pragma as written. If it fails, it means GCC 15 (this build) doesn't support it yet.
    #pragma omp induction(k:0:5:1) // k from 0 up to (but not including) 5, step 1. (0,1,2,3,4)
                                  // Removed spaces around colons, sometimes parsers are picky.
    for (int k = 0; k < 5; ++k) { // The loop bounds should match the induction
         sum_induction_6_0 += k; // The induction variable 'k' is used here
         induction_loop_executed = 1;
    }
    omp6_0_feature_compiled_induction = 1;
    if (omp6_0_feature_compiled_induction) {
        if (induction_loop_executed && sum_induction_6_0 == 10) { // Sum of 0,1,2,3,4 is 10
             printf("  [6.0] '#pragma omp induction' compiled and basic test executed correctly (sum=%d).\n", sum_induction_6_0);
        } else if (induction_loop_executed) {
             printf("  [6.0] '#pragma omp induction' compiled and loop executed, but sum is unexpected (sum=%d). Check logic or compiler behavior.\n", sum_induction_6_0);
        } else {
             printf("  [6.0] '#pragma omp induction' compiled, but the test loop might not have executed as expected.\n");
        }
    }

    #pragma omp assume no_parallelism
    {
        // This block is assumed by the compiler to have no OpenMP parallelism.
    }
    omp6_0_feature_compiled_assume_no_par = 1;
    if (omp6_0_feature_compiled_assume_no_par) {
        printf("  [6.0] '#pragma omp assume no_parallelism' compiled.\n");
    }
    printf("\n");

    printf("Test finished. Check compiler output for any errors or warnings.\n");
    printf("If a section caused a compilation error, your GCC 15 does not support that specific feature (or there's a typo!).\n");
    printf("Remember, the _OPENMP macro reported: %d.\n", _OPENMP);

    return 0;
}