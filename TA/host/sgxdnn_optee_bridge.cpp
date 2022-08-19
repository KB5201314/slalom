
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <omp.h>

#include "sgxdnn_optee_u.h"
#include <openenclave/host.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define TOKEN_FILENAME "enclave.token"
#define ENCLAVE_FILENAME "enclave.signed.so"

using namespace std::chrono;

/* Check error conditions for loading enclave */
void print_error_message(oe_result_t result) {
    printf("Error: %s\n", oe_result_str(result));
}

/* OCall functions */
void ocall_print_string(const char *str) {
    /* Proxy/Bridge will check the length and null-terminate
     * the input string to prevent buffer overflow.
     */
    printf("%s", str);
}

thread_local std::chrono::time_point<std::chrono::high_resolution_clock> start;

void ocall_start_clock() { start = std::chrono::high_resolution_clock::now(); }

void ocall_end_clock(const char *str) {
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    printf(str, elapsed.count());
}

double ocall_get_time() {
    auto now = std::chrono::high_resolution_clock::now();
    return time_point_cast<microseconds>(now).time_since_epoch().count();
}

extern "C" {

static oe_enclave_t *enclave = NULL;

/*
 * Initialize the enclave
 */
oe_result_t initialize_enclave(void) {

    std::cout << "Initializing Enclave..." << std::endl;

    if (enclave) {
        std::cout << "WARNING: It seems that enclave has already been "
                     "initialized !!!!!!"
                  << std::endl;
        std::cout << "WARNING: value of enclave:" << std::hex << enclave
                  << std::endl;
    }

    oe_result_t result;
    oe_enclave_t *enclave = NULL;

    /* call oe_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    result = oe_create_sgxdnn_optee_enclave(
        ENCLAVE_FILENAME, OE_ENCLAVE_TYPE_OPTEE, 0, NULL, 0, &enclave);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }

    std::cout << "Enclave: " << enclave << std::endl;

    return result;
}

/*
 * Destroy the enclave
 */
void destroy_enclave() {
    std::cout << "Destroying Enclave: " << enclave << std::endl;
    oe_terminate_enclave(enclave);
    enclave = NULL;
}

void load_model_float(char *model_json, float **filters) {
    oe_result_t result = ecall_load_model_float(enclave, model_json, filters);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }
}

void predict_float(float *input, float *output, int batch_size) {
    for (int i = 0; i < batch_size; i++) {
        // oe_result_t result = ecall_predict_float(enclave, input, output,
        // batch_size);
        oe_result_t result = ecall_predict_float(enclave, input, output, 1);
        printf("predict returned!\n");
        if (result != OE_OK) {
            print_error_message(result);
            throw result;
        }
    }
    printf("returning...\n");
}

void load_model_float_verify(char *model_json, float **filters, bool preproc) {
    oe_result_t result =
        ecall_load_model_float_verify(enclave, model_json, filters, preproc);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }
}

void predict_verify_float(float *input, float *output, float **aux_data,
                          int batch_size) {

    oe_result_t result = ecall_predict_verify_float(enclave, input, output,
                                                    aux_data, batch_size);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }
    printf("returning...\n");
}

void slalom_relu(float *input, float *output, float *blind, int num_elements,
                 char *activation) {
    oe_result_t result = ecall_slalom_relu(enclave, input, output, blind,
                                           num_elements, activation);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }
}

void slalom_maxpoolrelu(float *input, float *output, float *blind,
                        long int dim_in[4], long int dim_out[4],
                        int window_rows, int window_cols, int row_stride,
                        int col_stride, bool same_padding) {
    oe_result_t result = ecall_slalom_maxpoolrelu(
        enclave, input, output, blind, dim_in, dim_out, window_rows,
        window_cols, row_stride, col_stride, same_padding);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }
}

void slalom_init(bool integrity, bool privacy, int batch_size) {
    oe_result_t result =
        ecall_slalom_init(enclave, integrity, privacy, batch_size);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }
}

void slalom_get_r(float *out, int size) {
    oe_result_t result = ecall_slalom_get_r(enclave, out, size);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }
}

void slalom_set_z(float *z, float *z_enc, int size) {
    oe_result_t result = ecall_slalom_set_z(enclave, z, z_enc, size);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }
}

void slalom_blind_input(float *inp, float *out, int size) {
    oe_result_t result = ecall_slalom_blind_input(enclave, inp, out, size);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }
}

void sgxdnn_benchmarks(int num_threads) {
    oe_result_t result = ecall_sgxdnn_benchmarks(enclave, num_threads);
    if (result != OE_OK) {
        print_error_message(result);
        throw result;
    }
}
}

/* Application entry */
int main(int argc, char *argv[]) {
    (void)(argc);
    (void)(argv);

    try {
        initialize_enclave();

        const unsigned int filter_sizes[] = {
            3 * 3 * 3 * 64,    64,   3 * 3 * 64 * 64,    64,
            3 * 3 * 64 * 128,  128,  3 * 3 * 128 * 128,  128,
            3 * 3 * 128 * 256, 256,  3 * 3 * 256 * 256,  256,
            3 * 3 * 256 * 256, 256,  3 * 3 * 256 * 512,  512,
            3 * 3 * 512 * 512, 512,  3 * 3 * 512 * 512,  512,
            3 * 3 * 512 * 512, 512,  3 * 3 * 512 * 512,  512,
            3 * 3 * 512 * 512, 512,  7 * 7 * 512 * 4096, 4096,
            4096 * 4096,       4096, 4096 * 1000,        1000};

        float **filters = (float **)malloc(2 * 16 * sizeof(float *));
        for (int i = 0; i < 2 * 16; i++) {
            filters[i] = (float *)malloc(filter_sizes[i] * sizeof(float));
        }

        const unsigned int output_sizes[] = {224 * 224 * 64,
                                             224 * 224 * 64,
                                             112 * 112 * 128,
                                             112 * 112 * 128,
                                             56 * 56 * 256,
                                             56 * 56 * 256,
                                             56 * 56 * 256,
                                             28 * 28 * 512,
                                             28 * 28 * 512,
                                             28 * 28 * 512,
                                             14 * 14 * 512,
                                             14 * 14 * 512,
                                             14 * 14 * 512,
                                             4096,
                                             4096,
                                             1000};

        float **extras = (float **)malloc(16 * sizeof(float *));
        for (int i = 0; i < 16; i++) {
            extras[i] = (float *)malloc(output_sizes[i] * sizeof(float));
        }

        float *img = (float *)malloc(224 * 224 * 3 * sizeof(float));
        float *output = (float *)malloc(1000 * sizeof(float));
        printf("filters initalized\n");

        std::ifstream t("App/vgg16.json");
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::cout << buffer.str() << std::endl;
        printf("Loading model...\n");
        load_model_float((char *)buffer.str().c_str(), filters);
        printf("Model loaded!\n");

        for (int i = 0; i < 4; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            // predict_float(img, output, 1);
            predict_verify_float(img, output, extras, 1);
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            printf("predict required %4.2f sec\n", elapsed.count());
        }
        printf("Enter a character to destroy enclave ...\n");
        getchar();

        // Destroy the enclave
        destroy_enclave();

        printf("Info: Enclave Launcher successfully returned.\n");
        printf("Enter a character before exit ...\n");
        getchar();
        return 0;
    } catch (int e) {
        printf("Info: Enclave Launch failed!.\n");
        printf("Enter a character before exit ...\n");
        getchar();
        return -1;
    }
}
