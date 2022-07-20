/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include "tee_client_api.h"

/* For the UUID (found in the TA's h-file(s)) */
#include "sgxdnn_wrap.h"

#include "binn.h"

TEEC_Context ctx;
TEEC_Session sess;
TEEC_UUID uuid = TA_SGXDNN_WRAP_UUID;

/*
 * Initialize the enclave
 */
void initialize_enclave(void) {

    printf("Initializing TA Session...\n");

    TEEC_Result res;
    uint32_t ret_origin;

    /* Initialize a context connecting us to the TEE */
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    /*
     * Open a session to the "hello world" TA, the TA will print "hello
     * world!" in the log when the session is created.
     */
    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL,
                           &ret_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res,
             ret_origin);

    printf(
        "Initializing TA Session finished... TEEC_Result: %d ret_origin: %d\n",
        res, ret_origin);
}

/*
 * Destroy the enclave
 */
void destroy_enclave() {
    /*
     * We're done with the TA, close the session and
     * destroy the context.
     */

    TEEC_CloseSession(&sess);

    TEEC_FinalizeContext(&ctx);
}

void load_model_float(char *model_json, float **filters,
                      size_t *filters_content_size, size_t filters_size) {
    TEEC_Operation op;
    TEEC_Result res;
    uint32_t err_origin;

    memset(&op, 0, sizeof(op));

    binn *list = binn_list();
    binn_list_add_str(list, model_json);
    binn_list_add_uint32(list, filters_size);
    for (int idx = 0; idx < filters_size; idx++) {
        binn_list_add_blob(list, filters[idx], filters_content_size[idx]);
    }

    op.paramTypes =
        TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = binn_ptr(list);
    op.params[0].tmpref.size = binn_size(list);

    res = TEEC_InvokeCommand(&sess, TA_SGXDNN_WRAP_CMD_LOAD_MODEL_FLOAT, &op,
                             &err_origin);
	binn_free(list);
    if (res != TEEC_SUCCESS)
        errx(1,
             "TEEC_InvokeCommand cmdid: TA_SGXDNN_WRAP_CMD_LOAD_MODEL_FLOAT "
             "failed with code 0x%x origin 0x%x",
             res, err_origin);
}

// void predict_float(float* input, float* output, int batch_size) {

// 	for (int i=0; i<batch_size; i++) {
// 		//sgx_status_t ret = ecall_predict_float(eid, input, output,
// batch_size); 		sgx_status_t ret = ecall_predict_float(eid,
// input, output, 1); 		printf("predict returned!\n"); 		if (ret
// != SGX_SUCCESS) { 			print_error_message(ret);
// throw ret;
// 		}
// 	}
// 	printf("returning...\n");
// }

void load_model_float_verify(char *model_json, float **filters,
                             size_t *filters_content_size, size_t filters_size,
                             bool preproc) {
    TEEC_Operation op;
    TEEC_Result res;
    uint32_t err_origin;

    memset(&op, 0, sizeof(op));

    binn *list = binn_list();
    binn_list_add_str(list, model_json);
    binn_list_add_uint32(list, filters_size);
    for (int idx = 0; idx < filters_size; idx++) {
        binn_list_add_blob(list, filters[idx], filters_content_size[idx]);
    }
	binn_list_add_bool(list, preproc);

    op.paramTypes =
        TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = binn_ptr(list);
    op.params[0].tmpref.size = binn_size(list);

    res = TEEC_InvokeCommand(&sess, TA_SGXDNN_WRAP_CMD_LOAD_MODEL_FLOAT, &op,
                             &err_origin);
	binn_free(list);
    if (res != TEEC_SUCCESS)
        errx(1,
             "TEEC_InvokeCommand cmdid: TA_SGXDNN_WRAP_CMD_LOAD_MODEL_FLOAT "
             "failed with code 0x%x origin 0x%x",
             res, err_origin);
}

// void predict_verify_float(float* input, float* output, float** aux_data, int
// batch_size) {

// 	sgx_status_t ret = ecall_predict_verify_float(eid, input, output,
// aux_data, batch_size); 	if (ret != SGX_SUCCESS) {
// print_error_message(ret); 		throw ret;
// 	}
// 	printf("returning...\n");
// }

// void slalom_relu(float* input, float* output, float* blind, int num_elements,
// char* activation) { 	sgx_status_t ret = ecall_slalom_relu(eid, input, output,
// blind, num_elements, activation); 	if (ret != SGX_SUCCESS) {
// 		print_error_message(ret);
// 		throw ret;
// 	}
// }

// void slalom_maxpoolrelu(float* input, float* output, float* blind,
// 						long int dim_in[4], long int
// dim_out[4], 						int window_rows, int
// window_cols, int row_stride, int col_stride,
// bool same_padding)
// {
// 	sgx_status_t ret = ecall_slalom_maxpoolrelu(eid, input, output, blind,
// 												dim_in,
// dim_out,
// window_rows, window_cols,
// row_stride, col_stride,
// same_padding); 	if (ret != SGX_SUCCESS) {
// print_error_message(ret); 		throw ret;
// 	}
// }

// void slalom_init(bool integrity, bool privacy, int batch_size) {
// 	sgx_status_t ret = ecall_slalom_init(eid, integrity, privacy,
// batch_size); 	if (ret != SGX_SUCCESS) {
// print_error_message(ret); 		throw ret;
// 	}
// }

// void slalom_get_r(float* out, int size) {
// 	sgx_status_t ret = ecall_slalom_get_r(eid, out, size);
// 	if (ret != SGX_SUCCESS) {
// 		print_error_message(ret);
// 		throw ret;
// 	}
// }

// void slalom_set_z(float* z, float* z_enc, int size) {
// 	sgx_status_t ret = ecall_slalom_set_z(eid, z, z_enc, size);
// 	if (ret != SGX_SUCCESS) {
// 		print_error_message(ret);
// 		throw ret;
// 	}
// }

// void slalom_blind_input(float* inp, float* out, int size) {
// 	sgx_status_t ret = ecall_slalom_blind_input(eid, inp, out, size);
// 	if (ret != SGX_SUCCESS) {
// 		print_error_message(ret);
// 		throw ret;
// 	}
// }

// void sgxdnn_benchmarks(int num_threads) {
// 	sgx_status_t ret = ecall_sgxdnn_benchmarks(eid, num_threads);
// 	if (ret != SGX_SUCCESS) {
// 		print_error_message(ret);
// 		throw ret;
// 	}
// }

int main(void) {
    // TEEC_Result res;
    // TEEC_Context ctx;
    // TEEC_Session sess;
    // TEEC_Operation op;
    // TEEC_UUID uuid = TA_SGXDNN_WRAP_UUID;
    // uint32_t err_origin;

    // /* Initialize a context connecting us to the TEE */
    // res = TEEC_InitializeContext(NULL, &ctx);
    // if (res != TEEC_SUCCESS)
    // 	errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    // /*
    //  * Open a session to the "hello world" TA, the TA will print "hello
    //  * world!" in the log when the session is created.
    //  */
    // res = TEEC_OpenSession(&ctx, &sess, &uuid,
    // 		       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    // if (res != TEEC_SUCCESS)
    // 	errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
    // 		res, err_origin);

    // /*
    //  * Execute a function in the TA by invoking it, in this case
    //  * we're incrementing a number.
    //  *
    //  * The value of command ID part and how the parameters are
    //  * interpreted is part of the interface provided by the TA.
    //  */

    // /* Clear the TEEC_Operation struct */
    // memset(&op, 0, sizeof(op));

    // /*
    //  * Prepare the argument. Pass a value in the first parameter,
    //  * the remaining three parameters are unused.
    //  */
    // op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
    // 				 TEEC_NONE, TEEC_NONE);
    // op.params[0].value.a = 42;

    // /*
    //  * TA_SGXDNN_WRAP_CMD_INC_VALUE is the actual function in the TA to be
    //  * called.
    //  */
    // printf("Invoking TA to increment %d\n", op.params[0].value.a);
    // res = TEEC_InvokeCommand(&sess, TA_SGXDNN_WRAP_CMD_INC_VALUE, &op,
    // 			 &err_origin);
    // if (res != TEEC_SUCCESS)
    // 	errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
    // 		res, err_origin);
    // printf("TA incremented value to %d\n", op.params[0].value.a);

    // /*
    //  * We're done with the TA, close the session and
    //  * destroy the context.
    //  *
    //  * The TA will print "Goodbye!" in the log when the
    //  * session is closed.
    //  */

    // TEEC_CloseSession(&sess);

    // TEEC_FinalizeContext(&ctx);

    return 0;
}
