#ifndef NIST_SM2_H
#define NIST_SM2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "miracl.h"
#include "mirdef.h"

#include "sm3.h"

#define ERR_INFINITY_POINT    0x00000001
#define ERR_NOT_VALID_ELEMENT 0x00000002
#define ERR_NOT_VALID_POINT   0x00000003
#define ERR_ORDER             0x00000004
#define ERR_ECURVE_INIT       0x00000005
#define ERR_KEYEX_RA          0x00000006
#define ERR_KEYEX_RB          0x00000007
#define ERR_EQUAL_S1SB        0x00000008
#define ERR_EQUAL_S2SA        0x00000009
#define ERR_SELFTEST_Z        0x0000000A
#define ERR_SELFTEST_INI_I    0x0000000B
#define ERR_SELFTEST_RES_I    0x0000000C
#define ERR_SELFTEST_INI_II   0x0000000D
#define ERR_GENERATE_R        0x0000000E
#define ERR_GENERATE_S        0x0000000F
#define ERR_OUTRANGE_R        0x00000010
#define ERR_OUTRANGE_S        0x00000011
#define ERR_GENERATE_T        0x00000012
#define ERR_PUBKEY_INIT       0x00000013
#define ERR_DATA_MEMCMP       0x00000014
#define ERR_ARRAY_NULL        0x00000015
#define ERR_C3_MATCH          0x00000016
#define ERR_SELFTEST_KG       0x00000017
#define ERR_SELFTEST_ENC      0x00000018
#define ERR_SELFTEST_DEC      0x00000019

#define SM2_WORDSIZE 8
#define SM2_NUMBITS 256
#define SM2_NUMWORD	(SM2_NUMBITS / SM2_WORDSIZE) //32
#define SM2_PUBKEYSIZE 64


#ifdef __cplusplus
extern "C" {
#endif

int SM2_standard_init(miracl *mip, big para_p, big para_a, big para_b, big para_n, big para_Gx, big para_Gy, epoint *G);
int Test_Point(epoint* point);
int Test_PubKey(epoint *pubKey);
int Test_PrivKey(unsigned char privkey[]);
int Test_Range(big x);
int Test_Null(unsigned char array[], int len);
int Test_Zero(big x);
int Test_n(big x);
int SM2_Setup(unsigned char* prikey, unsigned char Px[], unsigned char Py[]);
int SM2_keygeneration_2(unsigned char PriKey[], unsigned char Px[], unsigned char Py[]);
int SM2_pre_ZA(unsigned char IDA[], int IDlen, unsigned char Px[], unsigned char Py[], unsigned char ZA[]);
int SM2_standard_sign(unsigned char *message, int len, unsigned char *UserID, int IDlen, unsigned char rand[], unsigned char d[], unsigned char R[], unsigned char S[]);
int SM2_standard_verify(unsigned char *message, int len, unsigned char ZA[], unsigned char Px[], unsigned char Py[], unsigned char R[], unsigned char S[]);

int KGC_gen_partial_keys(unsigned char* XA, unsigned char* IDA, unsigned int IDAlen, unsigned char*s, unsigned int slen, unsigned char* PA, unsigned char* za);
int client_gen_keys(unsigned char* IDA, unsigned int IDAlen, unsigned char* xa, unsigned char* PA, unsigned char* za, unsigned char* da, unsigned char* QA);
int get_real_pubkey(unsigned char* IDA, unsigned int IDAlen, unsigned char* PA, unsigned char* P, unsigned int Plen, unsigned char* QA);
void mhash(unsigned char* input, int len, int m, unsigned char* output);

void hex2bytes(unsigned char* hex, int hlen, unsigned char* bytes);
void bytes2hex(unsigned char* bytes, int blen, char* hex);
#ifdef __cplusplus
}
#endif



#endif
