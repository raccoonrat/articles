#ifndef NIST_SM_ALG_H
#define NIST_SM_ALG_H

#include <stdio.h>

//err code
#define ERR_PARAM  			-2
#define ERR_MEM_ALLOC 			-3
#define ERR_NEED_RAND_REGEN 		-4
#define ERR_MEM_LOW    			-5
#define ERR_DECRYPTION_FAILED 		-6
#define ERR_UNKNOWN     		-7
#define ERR_GENKEY_FAILED  		-8
#define ERR_INFINITE_POINT 		-10
#define ERR_POINT_NOT_ON_CURVE  	-11

#define CHECK_RET(x) if (x != 0){ret = x; \
fprintf(stderr, "%s(%d):err:%04x;desr:%d;\n", __FILE__, __LINE__, x, ret); \
goto END; }

#ifdef __cplusplus
extern "C" {
#endif

void sm2_init_thread();

/*
 * instruction: generate random num
 * input: len: length of random number
 * output: rnd[]: random number
 * return: none
 * NEED srang(time(NULL)) before when called
 */
void SM2_gen_rand(unsigned char rnd[], int len);


/*
 * instruction : GM sm2 generate private key
 * param:
 * @prikey: [out] : output private key
 * @pulPriLen:[in]: PreSet PrikeyLen
 * return :
 * 0 : success
 * other errcode : operation failed
 */
int GM_GenSM2prikey(unsigned char * prikey, unsigned long * pulPriLen);


/*
 * instruction : GM_sm2 signature Setup, generate public key
 * param:
 * @prikey, @ulPrikeyLen : [in] : private key (Byte Stream) of a random number
 * @pubkey, @ulPubkeyLen : [out] : public key (Byte Stream) generated, *ulPulkeyLen will always return 64
 * return :
 * 0 : success
 * other errcode : operation failed
 */

int GM_SM2_Setup(unsigned char* prikey, unsigned long ulPrikeyLen,
                 unsigned char* pubkey, unsigned long *pubkeyLen);


/*
 * instruction : sm2 signature with GM predefined curve params
 * param:
 * @signedData ,@pulSigLen : [out] : output sig data *LenDgst will always return 64,
 *      ###signedData = [r,s] = [---32 bytes of r ---][---32 bytes of s ---]
 * @Src ,@SrcLen: [in] : source data to digest and signature
 * @UserID, @lenUID :  [in] : user id
 * @prikey, @ulPrikeyLen : [in] : private key (Byte Stream) of a random number
 * return :
 * 0 : success
 * other errcode : operation failed
 *
 */
int GM_SM2Sign(unsigned char * signedData, unsigned long * pulSigLen,
               unsigned char * Src, unsigned long SrcLen,
               unsigned char * UserID, unsigned long lenUID,
               unsigned char * prikey, unsigned long ulPrikeyLen);


/*
 * instruction : sm2 signature verify with GM predefined curve params
 * param:
 * @signedData ,@ulSigLen : [in] : sig data to verify , ulSigLen should be 64
 *    ###signedData's structure: [r,s] = [---32 bytes of r ---][---32 bytes of s ---]
 * @Src ,@SrcLen: [in] : source data to digest and signature
 * @UserID, @lenUID :  [in] : user id
 * @szPubkey_XY, @ulPubkey_XYLen : [in] : public key of a (XA,YA),ulPubkey_XYLen should be 64
 *    ###structure of pubkey should be : [---32 byte of X coordinate---][---32 byte of Y coordinate---]
 * return :
 * 0 : success
 * other errcode : operation failed
 */
int GM_SM2VerifySig(unsigned char * signedData, unsigned long ulSigLen,
                    unsigned char * Src, unsigned long SrcLen,
                    unsigned char * UserID, unsigned long lenUID,
                    unsigned char * szPubkey_XY, unsigned long ulPubkey_XYLen);


/*
 * instruction: KGC setup
 * param:
 * 	@m: [in] number of pieces of system prikey and pubkey
 * 	@s: [in] system prime private key
 * 	@slen: [in] length of s in bytes, should be 32*m
 * 	@P: [out] system prime public key, with the shape of P1X||P1Y||P2X||P2Y...
 * 	@Plen: [in] length of P in bytes, should be 64*m
 * return:
 * 	0: success
 * 	other: error
 */
int CLPK_KGC_setup(int m, unsigned char* s, unsigned int slen, unsigned char* P, unsigned int Plen);


/*
 * instruction: client generate XA = xa*G
 * param:
 * 	@xa: [in] a random num
 * 	@xalen: [in] length of xa in bytes, should be 32
 * 	@XA: [out] XA = xa*G
 * 	@XAlen: [in] length of XA in bytes, should be 64
 * return:
 * 	0: success
 * 	other: error
 */
int CLPK_client_gen_XA(unsigned char* xa, unsigned int xalen, unsigned char* XA, unsigned int XAlen);


/*
 * instruction: KGC receive IDA and XA, caculate PA and generate partial client prikey za
 * param:
 * 	@XA: [in] XA, received from client
 * 	@XAlen: [in] length of XA in bytes, should be 64
 * 	@IDA: [in] client identity
 * 	@IDAlen: [in] length of IDA
 * 	@s: [in] system prime private key, with the shape s1||s2||...
 * 	@slen: [in] length of s in bytes, should be 32*m
 *	@PA: [out] partial user public key
 *	@PAlen: [in] length of PA in bytes, should be 64
 *	@za: [out] partial user private key
 *	@zalen: [in] length of za in bytes, should be 32
 * return:
 * 	0: success
 * 	other: error
 */
int CLPK_KGC_gen_partial_keys(unsigned char* XA, unsigned int XAlen,
			      unsigned char* IDA, unsigned int IDAlen,
			      unsigned char* s, unsigned int slen,
			      unsigned char* PA, unsigned int PAlen,
			      unsigned char* za, unsigned int zalen);

/*
 * instruction: client generate final private and public key
 * param:
 * 	@IDA: [in] client identity
 * 	@IDAlen: [in] length of IDA
 * 	@xa: [in] a random num used in client_gen_XA()
 * 	@xalen: [in] length of xa in bytes, should be 32
 *	@PA: [in] partial user public key, received from KGC
 *	@PAlen: [in] length of PA in bytes, should be 64
 *	@za: [in] partial user private key, received from KGC
 *	@zalen: [in] length of za in bytes, should be 32
 *	@da: [out] user final private key
 *	@dalen: [in] length of da, should be 32
 *	@QA: [out] user final public key
 *	@QAlen: [in] length of QA, should be 64
 * return:
 * 	0: success
 * 	otherL error
 */
int CLPK_client_gen_keys(unsigned char* IDA, unsigned int IDAlen,
			 unsigned char* xa, unsigned int xalen,
			 unsigned char* PA, unsigned int PAlen,
			 unsigned char* za, unsigned int zalen,
			 unsigned char* da, unsigned int dalen,
			 unsigned char* QA, unsigned int QAlen);

/*
 * instruction: caculate real public key from IDA,PA,P
 * param:
 * 	@IDA: [in] client identity
 * 	@IDAlen: [in] length of IDA
 *	@PA: [in] partial user public key, received from KGC
 *	@PAlen: [in] length of PA in bytes, should be 64
 * 	@P: [in] system prime public key, with the shape of P1X||P1Y||P2X||P2Y...
 * 	@Plen: [in] length of P in bytes, should be 64*m
 *	@QA: [out] user final public key
 *	@QAlen: [in] length of QA, should be 64
 * return:
 * 	0: success
 * 	otherL error
 */
int CLPK_get_real_pubkey(unsigned char* IDA, unsigned int IDAlen,
			 unsigned char* PA, unsigned int PAlen,
			 unsigned char* P, unsigned int Plen,
			 unsigned char* QA, unsigned int QAlen);
#ifdef __cplusplus
}
#endif


#endif
