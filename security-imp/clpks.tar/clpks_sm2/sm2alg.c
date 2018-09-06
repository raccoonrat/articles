#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include "sm2alg.h"
#include "sm2.h"
#include "sm3.h"
#include "time.h"

void sm2_init_thread()
{
	mr_init_threading();
}

/*
 * instruction: random number generator
 * input: length in bytes
 * outbut: rnd pointer
 * return: none
 * NEED srand((unsigned)time(NULL)) outside when call this function 
*/
void SM2_gen_rand(unsigned char rnd[], int len)
{
	int n = 0;
	int i;
	unsigned long ss,ss1,ss2;
    	if(len%4)
		n = len/4 + 1;
	else
		n = len/4;
	for(i=0;i<n;i++)
	{
		ss1 = rand();
		ss2 = rand();
		ss  = ((ss2&0x0000ffff)<<16) | (ss1&0x0000ffff);
		memcpy(rnd+4*i,&ss,4);
	}

}

//backup method
void SM2_gen_rand1(unsigned char rnd[], int len)
{
	int randData;
	int n = 0;

	while(len >= 4)
	{
		randData = rand();
		rnd[n++] = (unsigned char)(randData >> 24);
		rnd[n++] = (unsigned char)(randData >> 16);
		rnd[n++] = (unsigned char)(randData >> 8);
		rnd[n++] = (unsigned char)(randData);

		len -= 4;
	}

	if(len > 0)
	{
		randData = rand();
		for(len = len; len > 0; len--) rnd[n++] = (unsigned char)(randData >> (8 * len));
	}
}


/*
 * instruction : GM sm2 generate private key
 * param:
 * @prikey: [out] : output private key
 * @pulPriLen:[in]: PreSet PrikeyLen
 * return :
 * 0 : success
 * other errcode : operation failed
 */
int GM_GenSM2prikey(unsigned char * prikey, unsigned long * pulPriLen)
{
	if (NULL == prikey || * pulPriLen < 32)
	{
		return ERR_PARAM;
	}

#ifdef _TEST
	unsigned char d[32] =
	{0x12, 0x8B, 0x2F, 0xA8, 0xBD, 0x43, 0x3C, 0x6C, 0x06, 0x8C, 0x8D, 0x80, 0x3D, 0xFF, 0x79, 0x79,
	 0x2A, 0x51, 0x9A, 0x55, 0x17, 0x1B, 0x1B, 0x65, 0x0C, 0x23, 0x66, 0x1D, 0x15, 0x89, 0x72, 0x63};
	memcpy(prikey,d,32);
#else
	SM2_gen_rand(prikey, 32);
#endif

	return 0;
}


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
                 unsigned char* pubkey, unsigned long *pubkeyLen)
{
	if (NULL == prikey || 0 == ulPrikeyLen || NULL == pubkey || NULL == pubkeyLen)
	{
		return ERR_PARAM;
	}
	int ret = 0;
#ifdef _TEST
	unsigned char d[32] = 
	{0x12, 0x8B, 0x2F, 0xA8, 0xBD, 0x43, 0x3C, 0x6C, 0x06, 0x8C, 0x8D, 0x80, 0x3D, 0xFF, 0x79, 0x79,
	 0x2A, 0x51, 0x9A, 0x55, 0x17, 0x1B, 0x1B, 0x65, 0x0C, 0x23, 0x66, 0x1D, 0x15, 0x89, 0x72, 0x63};
		
	ret = SM2_Setup(d, pubkey, pubkey + SM2_NUMWORD);
#else
	ret = SM2_Setup(prikey, pubkey, pubkey + SM2_NUMWORD);
#endif

	* pubkeyLen = SM2_PUBKEYSIZE;
	
	return ret;
}


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
                    unsigned char * szPubkey_XY, unsigned long ulPubkey_XYLen)
{
	if (NULL == Src || 0 == SrcLen || NULL == UserID || 0 == lenUID ||
	        NULL == szPubkey_XY || 64 != ulPubkey_XYLen || NULL == signedData || 64 != ulSigLen)
	{
		return ERR_PARAM;
	}
	int ret;
	unsigned char ZA[32];
	ret = 0;
	if(0 != SM2_pre_ZA(UserID, lenUID, szPubkey_XY, szPubkey_XY + 32, ZA))
		return ERR_UNKNOWN;
	ret = SM2_standard_verify(Src, (int)SrcLen, ZA, szPubkey_XY, szPubkey_XY + 32, signedData, signedData + 32);
	return ret;
}


/*
 * instruction : sm2 signature with GM predefined curve params
 * param:
 * @signedData ,@pulSigLen : [out] : output sig data *LenDgst will always return 64,
 *      ###signedData = [r,s] = [---32 bytes of r ---][---32 bytes of s ---]
 * @Src ,@SrcLen: [in] : source data to digest and signature
 * @UserID, @lenUID :  [in] : user id
 * @prikey, @ulPrikeyLen : [in] : private key (Byte Stream) of a random number
 * return :
 * 	0 : success
 * 	other errcode : operation failed
 *
 */
int GM_SM2Sign(unsigned char * signedData, unsigned long * pulSigLen,
               unsigned char * Src, unsigned long SrcLen,
               unsigned char * UserID, unsigned long lenUID,
               unsigned char * prikey, unsigned long ulPrikeyLen)
{
	if (NULL == Src || 0 == SrcLen || NULL == UserID || 0 == lenUID ||
	        NULL == prikey || 0 == ulPrikeyLen || NULL == pulSigLen || NULL == signedData)
	{
		*pulSigLen = 0;
		return ERR_PARAM;
	}
	int ret;
	ret = 0;
#ifdef _TEST
	unsigned char k[32] =
	{0x6C, 0xB2, 0x8D, 0x99, 0x38, 0x5C, 0x17, 0x5C, 0x94, 0xF9, 0x4E, 0x93, 0x48, 0x17, 0x66, 0x3F,
	 0xC1, 0x76, 0xD9, 0x25, 0xDD, 0x72, 0xB7, 0x27, 0x26, 0x0D, 0xBA, 0xAE, 0x1F, 0xB2, 0xF9, 0x6F};
	ret = SM2_standard_sign(Src, (int)SrcLen, UserID, (int)lenUID, k, prikey, signedData, signedData + 32);
#else
	unsigned char k1[32];
	SM2_gen_rand(k1, 32);
	ret = SM2_standard_sign(Src, (int)SrcLen, UserID, (int)lenUID, k1, prikey, signedData, signedData + 32);
#endif

	* pulSigLen = SM2_PUBKEYSIZE;
	return ret;
}


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
int CLPK_KGC_setup(int m, unsigned char* s, unsigned int slen, unsigned char* P, unsigned int Plen)
{
	if (m < 1 || NULL == s || NULL == P || slen != 32*m || Plen != 64*m)
	{
		return ERR_PARAM;
	}

	int i;
	int ret = 0;
	for(i=0;i<m;i++)
	{
		ret = SM2_Setup(s + i*SM2_NUMWORD, P + i*SM2_NUMWORD*2, P + i*SM2_NUMWORD*2 + SM2_NUMWORD);
		if(ret)
			break;
	}

	return ret;
}


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
int CLPK_client_gen_XA(unsigned char* xa, unsigned int xalen, unsigned char* XA, unsigned int XAlen)
{
	if (NULL == xa || 32 != xalen || NULL == XA ||  64 != XAlen)
	{
		return ERR_PARAM;
	}

	int ret = 0;
	ret = SM2_Setup(xa, XA, XA + SM2_NUMWORD);
	return ret;
}


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
			      unsigned char* za, unsigned int zalen)
{
	if (NULL == XA || 64 != XAlen || NULL == IDA || NULL == s || slen%32 != 0 || NULL == PA ||
			64 != PAlen || NULL == za || 32 != zalen)
	{
		return ERR_PARAM;
	}
	int ret = 0;
	ret = KGC_gen_partial_keys(XA, IDA, IDAlen, s, slen, PA, za);
	return ret;
}


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
			 unsigned char* QA, unsigned int QAlen)
{
	if (NULL == IDA || NULL == xa || 32 != xalen || NULL == PA || 64 != PAlen || NULL == za ||
			32 != zalen || NULL == da || 32 != dalen || NULL == QA || 64 != QAlen)
	{
		return ERR_PARAM;
	}
	int ret = 0;
	ret = client_gen_keys(IDA, IDAlen, xa, PA, za, da, QA);
	return ret;
}


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
			 unsigned char* QA, unsigned int QAlen)
{
	if(NULL == IDA || NULL == PA || 64!= PAlen || NULL == P || Plen%64 != 0 || NULL == QA)
	{
		return ERR_PARAM;
	}
	int ret = 0;
	ret = get_real_pubkey(IDA, IDAlen, PA, P, Plen, QA);
	return ret;
}
#ifdef __cplusplus
}
#endif


