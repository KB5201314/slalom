#ifndef SGXDNN_INTERFACE_H
#define SGXDNN_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif
void sgxdnn_benchmarks(int num_threads);

void load_model_float(char* model_json, float** filters);
void load_model_float_verify(char* model_json, float** filters, bool preproc);

void predict_float(float* input, float* output, int batch_size);
void predict_verify_float(float* input, float* output, float** aux_data, int batch_size);

void slalom_relu(float* inp, float* out, float* blind, int num_elements, char* activation);
void slalom_relu_single_batch(float* inp, float* out, float* blind, int num_elements, char* activation);
void slalom_maxpoolrelu(float* inp, float* out, float* blind, long int dim_in[4], long int dim_out[4],
                        int window_rows, int window_cols, int row_stride, int col_stride, bool same_padding);

void slalom_init(bool integrity, bool privacy, int batch_size);
void slalom_get_r(float* out, int size);
void slalom_set_z(float* z, float* dest, int size);
void slalom_blind_input(float* inp, float* out, int size);
void slalom_blind_input_internal(float* inp, float* out, int size, float* temp);

#ifdef __cplusplus
}
#endif

#endif

