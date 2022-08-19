#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

// #define USE_EIGEN_TENSOR

#include "interface.h"
// #include "tensor_types.h"
// #include "Crypto.h"

#include "sgxdnn_wrap.h"
#include "binn.h"

// using namespace tensorflow;

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void) {
    DMSG("has been called");

    return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void) { DMSG("has been called"); }

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
                                    TEE_Param __maybe_unused params[4],
                                    void __maybe_unused **sess_ctx) {
    uint32_t exp_param_types =
        TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE,
                        TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

    DMSG("has been called");

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    /* Unused parameters */
    (void)&params;
    (void)&sess_ctx;

    /*
     * The DMSG() macro is non-standard, TEE Internal API doesn't
     * specify any means to logging from a TA.
     */
    IMSG("Hello World!\n");

    /* If return value != TEE_SUCCESS the session will not be created. */
    return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx) {
    (void)&sess_ctx; /* Unused parameter */
    IMSG("Goodbye!\n");
}

static TEE_Result cmd_load_model_float(uint32_t param_types,
                                       TEE_Param params[4]) {
    uint32_t exp_param_types =
        TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE,
                        TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    void *buf = params[0].memref.buffer;
    char *model_json = binn_list_str(buf, 1); // libbinn use base 1 for 'pos'
    size_t filters_size = binn_list_uint32(buf, 2);
    float **filters = (float **)TEE_Malloc(sizeof(float *) * filters_size, TEE_MALLOC_FILL_ZERO);
    for (int idx = 0; idx < filters_size; idx++) {
        filters[idx] = binn_list_blob(buf, 3 + idx, NULL);
    }
    load_model_float(model_json, filters);
    TEE_Free(filters);
    return TEE_SUCCESS;
}

// void cmd_predict_float(float *input, float *output, int batch_size) {
//     predict_float(input, output, batch_size);
// }

static TEE_Result cmd_load_model_float_verify(uint32_t param_types,
                                              TEE_Param params[4]) {
    uint32_t exp_param_types =
        TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE,
                        TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    void *buf = params[0].memref.buffer;
    char *model_json = binn_list_str(buf, 1); // libbinn use base 1 for 'pos'
    size_t filters_size = binn_list_uint32(buf, 2);
    float **filters = (float **)TEE_Malloc(sizeof(float *) * filters_size, TEE_MALLOC_FILL_ZERO);
    for (int idx = 0; idx < filters_size; idx++) {
        filters[idx] = binn_list_blob(buf, 3 + idx, NULL);
    }
    bool preproc = binn_list_add_bool(buf, 3 + filters_size);
    load_model_float_verify(model_json, filters, preproc);
    TEE_Free(filters);
    return TEE_SUCCESS;
}

// void cmd_predict_verify_float(float *input, float *output, float **aux_data,
//                               int batch_size) {
//     predict_verify_float(input, output, aux_data, batch_size);
// }

// void cmd_slalom_relu(float *in, float *out, float *blind, int numElements,
//                      char *activation) {
//     slalom_relu(in, out, blind, numElements, activation);
// }

// void cmd_slalom_maxpoolrelu(float *in, float *out, float *blind,
//                             long int dim_in[4], long int dim_out[4],
//                             int window_rows, int window_cols, int row_stride,
//                             int col_stride, int is_padding_same) {
//     slalom_maxpoolrelu(in, out, blind, dim_in, dim_out, window_rows,
//                        window_cols, row_stride, col_stride, is_padding_same);
// }

// void cmd_slalom_init(int integrity, int privacy, int batch_size) {
//     slalom_init(integrity, privacy, batch_size);
// }

// void cmd_slalom_get_r(float *out, int size) { slalom_get_r(out, size); }

// void cmd_slalom_set_z(float *z, float *z_enc, int size) {
//     slalom_set_z(z, z_enc, size);
// }

// void cmd_slalom_blind_input(float *inp, float *out, int size) {
//     slalom_blind_input(inp, out, size);
// }

// void cmd_sgxdnn_benchmarks(int num_threads) { sgxdnn_benchmarks(num_threads);
// }

static TEE_Result inc_value(uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types =
        TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT, TEE_PARAM_TYPE_NONE,
                        TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

    DMSG("has been called");

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    IMSG("Got value: %u from NW", params[0].value.a);
    params[0].value.a++;
    IMSG("Increase value to: %u", params[0].value.a);

    return TEE_SUCCESS;
}

static TEE_Result dec_value(uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types =
        TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT, TEE_PARAM_TYPE_NONE,
                        TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

    DMSG("has been called");

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    IMSG("Got value: %u from NW", params[0].value.a);
    params[0].value.a--;
    IMSG("Decrease value to: %u", params[0].value.a);

    return TEE_SUCCESS;
}
/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
                                      uint32_t cmd_id, uint32_t param_types,
                                      TEE_Param params[4]) {
    (void)&sess_ctx; /* Unused parameter */

    switch (cmd_id) {
    case TA_SGXDNN_WRAP_CMD_LOAD_MODEL_FLOAT:
        return cmd_load_model_float(param_types, params);
    // case TA_SGXDNN_WRAP_CMD_PREDICT_FLOAT:
    //     return cmd_predict_float(param_types, params);
    // case TA_SGXDNN_WRAP_CMD_LOAD_MODEL_FLOAT_VERIFY:
    //     return cmd_load_model_float_verify(param_types, params);
    // case TA_SGXDNN_WRAP_CMD_PREDICT_VERIFY_FLOAT:
    //     return cmd_predict_verify_float(param_types, params);
    // case TA_SGXDNN_WRAP_CMD_SLALOM_RELU:
    //     return cmd_slalom_relu(param_types, params);
    // case TA_SGXDNN_WRAP_CMD_SLALOM_MAXPOOLRELU:
    //     return cmd_slalom_maxpoolrelu(param_types, params);
    // case TA_SGXDNN_WRAP_CMD_SLALOM_INIT:
    //     return cmd_slalom_init(param_types, params);
    // case TA_SGXDNN_WRAP_CMD_SLALOM_GET_R:
    //     return cmd_slalom_get_r(param_types, params);
    // case TA_SGXDNN_WRAP_CMD_SLALOM_SET_Z:
    //     return cmd_slalom_set_z(param_types, params);
    // case TA_SGXDNN_WRAP_CMD_SLALOM_BLIND_INPUT:
    //     return cmd_slalom_blind_input(param_types, params);
    // case TA_SGXDNN_WRAP_CMD_SGXDNN_BENCHMARKS:
    //     return cmd_sgxdnn_benchmarks(param_types, params);
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
