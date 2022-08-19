#ifndef TA_SGXDNN_WRAP_H
#define TA_SGXDNN_WRAP_H


/*
 * This UUID is generated with uuidgen
 * the ITU-T UUID generator at http://www.itu.int/ITU-T/asn1/uuid.html
 */
#define TA_SGXDNN_WRAP_UUID \
	{ 0x0c997be1, 0x7e4e, 0x4cfc, \
	{ 0xad, 0xdb, 0xb2, 0xaa, 0x41, 0xd8, 0x88, 0x04} }

/* The function IDs implemented in this TA */
#define TA_SGXDNN_WRAP_CMD_LOAD_MODEL_FLOAT			0
#define TA_SGXDNN_WRAP_CMD_PREDICT_FLOAT			1
#define TA_SGXDNN_WRAP_CMD_LOAD_MODEL_FLOAT_VERIFY	2
#define TA_SGXDNN_WRAP_CMD_PREDICT_VERIFY_FLOAT		3
#define TA_SGXDNN_WRAP_CMD_SLALOM_RELU				4
#define TA_SGXDNN_WRAP_CMD_SLALOM_MAXPOOLRELU		5
#define TA_SGXDNN_WRAP_CMD_SLALOM_INIT				6
#define TA_SGXDNN_WRAP_CMD_SLALOM_GET_R				7
#define TA_SGXDNN_WRAP_CMD_SLALOM_SET_Z				8
#define TA_SGXDNN_WRAP_CMD_SLALOM_BLIND_INPUT		9
#define TA_SGXDNN_WRAP_CMD_SGXDNN_BENCHMARKS		11

#endif /*TA_SGXDNN_WRAP_H*/
