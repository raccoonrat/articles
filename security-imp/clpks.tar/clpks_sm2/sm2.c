#include "sm2alg.h"
#include "sm2.h"

#ifdef _TEST
// sm2 test parameters
static const char SM2_p[32] =
{
	0x85, 0x42, 0xD6, 0x9E, 0x4C, 0x04, 0x4F, 0x18, 0xE8, 0xB9, 0x24, 0x35, 0xBF, 0x6F, 0xF7, 0xDE,
	0x45, 0x72, 0x83, 0x91, 0x5C, 0x45, 0x51, 0x7D, 0x72, 0x2E, 0xDB, 0x8B, 0x08, 0xF1, 0xDF, 0xC3
};
static const char SM2_a[32] =
{
	0x78, 0x79, 0x68, 0xB4, 0xFA, 0x32, 0xC3, 0xFD, 0x24, 0x17, 0x84, 0x2E, 0x73, 0xBB, 0xFE, 0xFF,
	0x2F, 0x3C, 0x84, 0x8B, 0x68, 0x31, 0xD7, 0xE0, 0xEC, 0x65, 0x22, 0x8B, 0x39, 0x37, 0xE4, 0x98
};
static const char SM2_b[32] =
{
	0x63, 0xE4, 0xC6, 0xD3, 0xB2, 0x3B, 0x0C, 0x84, 0x9C, 0xF8, 0x42, 0x41, 0x48, 0x4B, 0xFE, 0x48,
	0xF6, 0x1D, 0x59, 0xA5, 0xB1, 0x6B, 0xA0, 0x6E, 0x6E, 0x12, 0xD1, 0xDA, 0x27, 0xC5, 0x24, 0x9A
};
static const char SM2_n[32] =
{
	0x85, 0x42, 0xD6, 0x9E, 0x4C, 0x04, 0x4F, 0x18, 0xE8, 0xB9, 0x24, 0x35, 0xBF, 0x6F, 0xF7, 0xDD,
	0x29, 0x77, 0x20, 0x63, 0x04, 0x85, 0x62, 0x8D, 0x5A, 0xE7, 0x4E, 0xE7, 0xC3, 0x2E, 0x79, 0xB7
};
static const char SM2_Gx[32] =
{
	0x42, 0x1D, 0xEB, 0xD6, 0x1B, 0x62, 0xEA, 0xB6, 0x74, 0x64, 0x34, 0xEB, 0xC3, 0xCC, 0x31, 0x5E,
	0x32, 0x22, 0x0B, 0x3B, 0xAD, 0xD5, 0x0B, 0xDC, 0x4C, 0x4E, 0x6C, 0x14, 0x7F, 0xED, 0xD4, 0x3D
};
static const char SM2_Gy[32] =
{
	0x06, 0x80, 0x51, 0x2B, 0xCB, 0xB4, 0x2C, 0x07, 0xD4, 0x73, 0x49, 0xD2, 0x15, 0x3B, 0x70, 0xC4,
	0xE5, 0xD7, 0xFD, 0xFC, 0xBF, 0xA3, 0x6E, 0xA1, 0xA8, 0x58, 0x41, 0xB9, 0xE4, 0x6E, 0x09, 0xA2
};

#else
// SM2 standard params
static const char SM2_p[32] = {
	0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const char SM2_a[32] = {
	0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC};
static const char SM2_b[32] = {
	0x28, 0xE9, 0xFA, 0x9E, 0x9D, 0x9F, 0x5E, 0x34, 0x4D, 0x5A, 0x9E, 0x4B, 0xCF, 0x65, 0x09, 0xA7,
	0xF3, 0x97, 0x89, 0xF5, 0x15, 0xAB, 0x8F, 0x92, 0xDD, 0xBC, 0xBD, 0x41, 0x4D, 0x94, 0x0E, 0x93};
static const char SM2_n[32] = {
	0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x72, 0x03, 0xDF, 0x6B, 0x21, 0xC6, 0x05, 0x2B, 0x53, 0xBB, 0xF4, 0x09, 0x39, 0xD5, 0x41, 0x23};
static const char SM2_Gx[32] = {
	0x32, 0xC4, 0xAE, 0x2C, 0x1F, 0x19, 0x81, 0x19, 0x5F, 0x99, 0x04, 0x46, 0x6A, 0x39, 0xC9, 0x94,
	0x8F, 0xE3, 0x0B, 0xBF, 0xF2, 0x66, 0x0B, 0xE1, 0x71, 0x5A, 0x45, 0x89, 0x33, 0x4C, 0x74, 0xC7};
static const char SM2_Gy[32] = {
	0xBC, 0x37, 0x36, 0xA2, 0xF4, 0xF6, 0x77, 0x9C, 0x59, 0xBD, 0xCE, 0xE3, 0x6B, 0x69, 0x21, 0x53,
	0xD0, 0xA9, 0x87, 0x7C, 0xC6, 0x2A, 0x47, 0x40, 0x02, 0xDF, 0x32, 0xE5, 0x21, 0x39, 0xF0, 0xA0};
	
#endif


void BYTE_print(unsigned char * tar, unsigned long l)
{
	int i;
	for (i = 0; i < l; i++)
	{
		if (i % 4 == 0)
		{
			printf(" ");
		}
		printf("%02X", tar[i]);
	}
	printf("\n");
}

int SM2_standard_clear(big para_p, big para_a,big para_b, big para_n, big para_Gx, big para_Gy, epoint *G)
{
	mirkill(para_p);
	mirkill(para_a);
	mirkill(para_b);
	mirkill(para_n);
	mirkill(para_Gx);
	mirkill(para_Gy);
	epoint_free(G);
	return 0;
}

/*
 * 	function: SM2 standard initialization
 * 	in/out: mip, para_p, para_a, para_b, para_n, para_Gx, para_Gy, G
 * 	return:
 * 		0:success
 * 		!0:fail
*/
int SM2_standard_init(miracl *mip, big para_p, big para_a, big para_b, big para_n, big para_Gx, big para_Gy, epoint *G)
{
	//epoint *nG;

	mip = mirsys(10000, 16);
	mip->IOBASE = 16;

	para_p = mirvar(0);
	para_a = mirvar(0);
	para_b = mirvar(0);
	para_n = mirvar(0);
	para_Gx = mirvar(0);
	para_Gy = mirvar(0);

	G = epoint_init();
	//nG = epoint_init();

	bytes_to_big(SM2_NUMWORD, SM2_p, para_p);
	bytes_to_big(SM2_NUMWORD, SM2_a, para_a);
	bytes_to_big(SM2_NUMWORD, SM2_b, para_b);
	bytes_to_big(SM2_NUMWORD, SM2_n, para_n);
	bytes_to_big(SM2_NUMWORD, SM2_Gx, para_Gx);
	bytes_to_big(SM2_NUMWORD, SM2_Gy, para_Gy);

	/*Initialises GF(p) elliptic curve.(MR_PROJECTIVE specifying projective coordinates)*/
	ecurve_init(para_a, para_b, para_p, MR_PROJECTIVE);

	/*initialise point G*/
	if (!epoint_set(para_Gx, para_Gy, 0, G)) return ERR_ECURVE_INIT;

	//ecurve_mult(para_n, G, nG);

	/*test if the order of the point is n*/
	//if (!point_at_infinity(nG)) return ERR_ORDER;

	return 0;
}


//test if point is on curve
/*
int Test_Point(epoint* point)
{
	big x, y, x_3, tmp;

	x = mirvar(0);
	y = mirvar(0);
	x_3 = mirvar(0);
	tmp = mirvar(0);

	//test if y^2 = x^3 + ax + b
	epoint_get(point, x, y);
	power(x, 3, para_p, x_3);	//x_3 = x^3 mod p
	multiply(x, para_a, x); 	//x = a * x
	divide(x, para_p, tmp); 	//x = a * x mod p, tmp = a * x / p
	add(x_3, x, x);				//x = x^3 + ax
	add(x, para_b, x);			//x = x^3 + ax + b
	divide(x, para_p, tmp);		//x = x^3 + ax + b mod p
	power(y, 2, para_p, y);		//y = y^2 mod p

	if (mr_compare(x, y) != 0) return ERR_NOT_VALID_POINT;

	return 0;
}

//test if pubkey is on curve
int Test_PubKey(epoint *pubKey)
{
	big x, y, x_3, tmp;

	epoint *nP;
	x = mirvar(0);
	y = mirvar(0);
	x_3 = mirvar(0);
	tmp = mirvar(0);

	nP = epoint_init();

	if (point_at_infinity(pubKey)) return ERR_INFINITY_POINT;

	//test if x < p and y<p both hold
	epoint_get(pubKey, x, y);
	if ((mr_compare(x, para_p) != -1) || (mr_compare(y, para_p) != -1)) return ERR_NOT_VALID_ELEMENT;

	if (Test_Point(pubKey) != 0) return ERR_NOT_VALID_POINT;

	//test if the order of pubKey is equal to n
	//nP=[n]P if np is point NOT at infinity, return error
	ecurve_mult(para_n, pubKey, nP);
	if (!point_at_infinity(nP))	return ERR_ORDER;

	mirkill(x);
	mirkill(y);
	mirkill(x_3);
	mirkill(tmp);
	epoint_free(nP);
	return 0;
}

//test if private key is in range[1, n-2]
int Test_PrivKey(unsigned char privkey[])
{
	big one, decr_n;
	big d;

	one = mirvar(0);
	decr_n = mirvar(0);
	d = mirvar(0);

	SM2_standard_init();

	bytes_to_big(SM2_NUMWORD, privkey, d);

	convert(1, one);
	decr(para_n, 2, decr_n);

	if ((mr_compare(d, one) < 0) | (mr_compare(d, decr_n) > 0)) return 1;

	mirkill(one);
	mirkill(decr_n);
	mirkill(d);
	return 0;
}


//test if x is in range [1, n-1]
int Test_Range(big x)
{
	big one, decr_n;

	one = mirvar(0);
	decr_n = mirvar(0);

	convert(1, one);
	decr(para_n, 1, decr_n);

	if ((mr_compare(x, one) < 0) | (mr_compare(x, decr_n) > 0)) return 1;
	mirkill(one);
	mirkill(decr_n);

	return 0;
}

// test if the given array is all zero
int Test_Null(unsigned char array[], int len)
{
	int i;

	for (i = 0; i < len; i++) if (array[i] != 0x00) return 0;

	return 1;
}

// test if the big x is zero 
int Test_Zero(big x)
{
	big zero;

	zero = mirvar(0);
	if (mr_compare(x, zero) == 0) return 1;

	mirkill(zero);
	return 0;
}

// test if the big x is order n 
int Test_n(big x)
{
	if (mr_compare(x, para_n) == 0) return 1;

	return 0;
}*/

/* instruction: SM2 setup to generate public key
 * input: prikey: private key pointer
 * output:
 *		Px[]:x label of P
 *		Py[]:y label of P
 * return:
 *		0:success
 *		other:error
 */
int SM2_Setup(unsigned char* PriKey, unsigned char Px[], unsigned char Py[])
{
	big para_p, para_a, para_b, para_n, para_Gx, para_Gy;
	epoint *G;
	miracl *mip;
	big d, PAx, PAy, rem;
	epoint *PA;
	mip = mirsys(10000, 16);
	mip->IOBASE = 16;

	para_p = mirvar(0);
	para_a = mirvar(0);
	para_b = mirvar(0);
	para_n = mirvar(0);
	para_Gx = mirvar(0);
	para_Gy = mirvar(0);

	G = epoint_init();

	bytes_to_big(SM2_NUMWORD, SM2_p, para_p);
	bytes_to_big(SM2_NUMWORD, SM2_a, para_a);
	bytes_to_big(SM2_NUMWORD, SM2_b, para_b);
	bytes_to_big(SM2_NUMWORD, SM2_n, para_n);
	bytes_to_big(SM2_NUMWORD, SM2_Gx, para_Gx);
	bytes_to_big(SM2_NUMWORD, SM2_Gy, para_Gy);

	/*Initialises GF(p) elliptic curve.(MR_PROJECTIVE specifying projective coordinates)*/
	ecurve_init(para_a, para_b, para_p, MR_PROJECTIVE);

	/*initialise point G*/
	if (!epoint_set(para_Gx, para_Gy, 0, G)) return ERR_ECURVE_INIT;

	PA = epoint_init();

	d = mirvar(0);
	rem = mirvar(0);
	PAx = mirvar(0);
	PAy = mirvar(0);

	bytes_to_big(SM2_NUMWORD, (const char*)PriKey, d);

	ecurve_mult(d, G, PA);
	epoint_get(PA, PAx, PAy);

	big_to_bytes(SM2_NUMWORD, PAx, (char*)Px, TRUE);
	big_to_bytes(SM2_NUMWORD, PAy, (char*)Py, TRUE);

	//if(0 != (i = Test_PubKey(PA))) return i;
	
	SM2_standard_clear(para_p,para_a,para_b,para_n,para_Gx,para_Gy, G);
	mirexit();
	mirkill(d);
	mirkill(PAx);
	mirkill(PAy);
	mirkill(rem);
	epoint_free(PA);
	return 0;
}


/*
 * 	function: caculate ZA
 * 	input: 
 * 		IDA: ID
 * 		IDlen: length of ID
 * 		Px: public key x label
 * 		Py: public key y label
 * 	output: ZA
 * 	return: 0
*/
int SM2_pre_ZA(unsigned char IDA[], int IDlen, unsigned char Px[], unsigned char Py[], unsigned char ZA[])
{
	unsigned char ENTLA[2];
	int tmplen = IDlen * 8;
	unsigned char tmp[5];
	sprintf((char*)tmp, "%04X", tmplen);
	hex2bytes(tmp, 4, ENTLA);

	unsigned char Msg[IDlen + 2 + SM2_NUMWORD * 6];	//IDA_len + 2 + SM2_NUMWORD * 6

	//ZA = Hash(ENTLA || IDA || a || b || Gx || Gy || xA|| yA)
	memcpy(Msg, ENTLA, 2);
	memcpy(Msg + 2, IDA, IDlen);
	memcpy(Msg + 2 + IDlen, SM2_a, SM2_NUMWORD);
	memcpy(Msg + 2 + IDlen + SM2_NUMWORD, SM2_b, SM2_NUMWORD);
	memcpy(Msg + 2 + IDlen + SM2_NUMWORD * 2, SM2_Gx, SM2_NUMWORD);
	memcpy(Msg + 2 + IDlen + SM2_NUMWORD * 3, SM2_Gy, SM2_NUMWORD);
	memcpy(Msg + 2 + IDlen + SM2_NUMWORD * 4, Px, SM2_NUMWORD);
	memcpy(Msg + 2 + IDlen + SM2_NUMWORD * 5, Py, SM2_NUMWORD);

	SM3_256(Msg, sizeof(Msg), ZA);
	return 0;
}


/*
 * 	function: SM2 standard signature
 * 	input: 
 * 		message: message to be signed
 * 		len: lenght of message
 * 		ZA: ZA value
 * 		rand: random number k
 * 		d: private key
 * 	output: R, S
 * 	retuen: 
 * 		0:success
 * 		!0: fail
*/
int SM2_standard_sign(unsigned char *message, int len, unsigned char *UserID, int IDlen, unsigned char rand[], unsigned char d[], unsigned char R[], unsigned char S[])
{
	big para_p, para_a, para_b, para_n, para_Gx, para_Gy;
	epoint *G;
	miracl *mip;
	unsigned char hash[SM3_len / 8];
	int M_len = len + SM3_len / 8;
	unsigned char *M = NULL;
	unsigned char Px[32], Py[32], ZA[32];

	big dA, r, s, e, k, KGx, KGy, PAx, PAy;
	big rem, rk, z1, z2;
	epoint *KG, *PA;

	mip = mirsys(10000, 16);
	mip->IOBASE = 16;

	para_p = mirvar(0);
	para_a = mirvar(0);
	para_b = mirvar(0);
	para_n = mirvar(0);
	para_Gx = mirvar(0);
	para_Gy = mirvar(0);

	G = epoint_init();

	bytes_to_big(SM2_NUMWORD, SM2_p, para_p);
	bytes_to_big(SM2_NUMWORD, SM2_a, para_a);
	bytes_to_big(SM2_NUMWORD, SM2_b, para_b);
	bytes_to_big(SM2_NUMWORD, SM2_n, para_n);
	bytes_to_big(SM2_NUMWORD, SM2_Gx, para_Gx);
	bytes_to_big(SM2_NUMWORD, SM2_Gy, para_Gy);

	/*Initialises GF(p) elliptic curve.(MR_PROJECTIVE specifying projective coordinates)*/
	ecurve_init(para_a, para_b, para_p, MR_PROJECTIVE);

	/*initialise point G*/
	if (!epoint_set(para_Gx, para_Gy, 0, G)) return ERR_ECURVE_INIT;
	//if(0 != (i = SM2_standard_init(mip, para_p, para_a, para_b, para_n, para_Gx, para_Gy, G))) return i;

	//initiate
	dA = mirvar(0);
	e = mirvar(0);
	k = mirvar(0);
	KGx = mirvar(0);
	KGy = mirvar(0);
	r = mirvar(0);
	s = mirvar(0);
	rem = mirvar(0);
	rk = mirvar(0);
	z1 = mirvar(0);
	z2 = mirvar(0);
	PAx = mirvar(0);
	PAy = mirvar(0);

	bytes_to_big(SM2_NUMWORD, (const char*)d, dA);	//cinstr(dA, d);

	KG = epoint_init();
	PA = epoint_init();
	ecurve_mult(dA, G, PA);
	epoint_get(PA, PAx, PAy);
	big_to_bytes(SM2_NUMWORD, PAx, (char*)Px, TRUE);
	big_to_bytes(SM2_NUMWORD, PAy, (char*)Py, TRUE);
	SM2_pre_ZA(UserID, IDlen, Px, Py, ZA);

	//step1, set M = ZA || M
	M = (unsigned char *)malloc(sizeof(char) * (M_len + 1));
	memcpy(M, ZA, SM3_len / 8);
	memcpy(M + SM3_len / 8, message, len);

	//step2, generate e = H(M)
	SM3_256(M, M_len, hash);
	bytes_to_big(SM3_len / 8, (const char*)hash, e);

	//step3:generate k
	bytes_to_big(SM3_len / 8, (const char*)rand, k);

	//step4:calculate kG
	ecurve_mult(k, G, KG);

	//step5:calculate r
	epoint_get(KG, KGx, KGy);
	add(e, KGx, r);
	divide(r, para_n, rem);

	//judge r = 0 or n + k = n?
	add(r, k, rk);
	//if (Test_Zero(r) | Test_n(rk)) return ERR_GENERATE_R;

	//step6:generate s
	incr(dA, 1, z1);
	xgcd(z1, para_n, z1, z1, z1);
	multiply(r, dA, z2);
	divide(z2, para_n, rem);
	subtract(k, z2, z2);
	add(z2, para_n, z2);
	multiply(z1, z2, s);
	divide(s, para_n, rem);

	//judge s = 0?
	//if (Test_Zero(s)) return ERR_GENERATE_S ;

	big_to_bytes(SM2_NUMWORD, r, (char*)R, TRUE);
	big_to_bytes(SM2_NUMWORD, s, (char*)S, TRUE);

	free(M);
	mirexit();
	SM2_standard_clear(para_p,para_a,para_b,para_n,para_Gx,para_Gy, G);
	mirkill(dA);
	mirkill(r);
	mirkill(s);
	mirkill(e);
	mirkill(k);
	mirkill(KGx);
	mirkill(KGy);
	mirkill(PAx);
	mirkill(PAy);
	mirkill(rem);
	mirkill(rk);
	mirkill(z1);
	mirkill(z2);
	epoint_free(KG);
	epoint_free(PA);
	return 0;
}


/*
 * 	function: verify signature
 * 	input:
 * 		message: sign message
 * 		len: lenght of message
 * 		ZA: ZA value
 * 		Px,Py: public key
 * 		R,S: signature
 * 	return:
 * 		0: success
 * 		!0: fail
*/
int SM2_standard_verify(unsigned char *message, int len, unsigned char ZA[], unsigned char Px[], unsigned char Py[], unsigned char R[], unsigned char S[])
{
	big para_p, para_a, para_b, para_n, para_Gx, para_Gy;
	epoint *G;
	miracl *mip;
	unsigned char hash[SM3_len / 8];
	int M_len = len + SM3_len / 8;
	unsigned char *M = NULL;

	big PAx, PAy, r, s, e, t, rem, x1, y1;
	big RR;
	epoint *PA, *sG, *tPA;

	mip = mirsys(10000, 16);
	mip->IOBASE = 16;

	para_p = mirvar(0);
	para_a = mirvar(0);
	para_b = mirvar(0);
	para_n = mirvar(0);
	para_Gx = mirvar(0);
	para_Gy = mirvar(0);

	G = epoint_init();

	bytes_to_big(SM2_NUMWORD, SM2_p, para_p);
	bytes_to_big(SM2_NUMWORD, SM2_a, para_a);
	bytes_to_big(SM2_NUMWORD, SM2_b, para_b);
	bytes_to_big(SM2_NUMWORD, SM2_n, para_n);
	bytes_to_big(SM2_NUMWORD, SM2_Gx, para_Gx);
	bytes_to_big(SM2_NUMWORD, SM2_Gy, para_Gy);

	/*Initialises GF(p) elliptic curve.(MR_PROJECTIVE specifying projective coordinates)*/
	ecurve_init(para_a, para_b, para_p, MR_PROJECTIVE);

	/*initialise point G*/
	if (!epoint_set(para_Gx, para_Gy, 0, G))
		return ERR_ECURVE_INIT;
	//if(0 != (i = SM2_standard_init(mip, para_p, para_a, para_b, para_n, para_Gx, para_Gy, G))) return i;

	PAx = mirvar(0);
	PAy = mirvar(0);
	r = mirvar(0);
	s = mirvar(0);
	e = mirvar(0);
	t = mirvar(0);
	x1 = mirvar(0);
	y1 = mirvar(0);
	rem = mirvar(0);
	RR = mirvar(0);

	PA = epoint_init();
	sG = epoint_init();
	tPA = epoint_init();

	bytes_to_big(SM2_NUMWORD, (const char*)Px, PAx);
	bytes_to_big(SM2_NUMWORD, (const char*)Py, PAy);

	bytes_to_big(SM2_NUMWORD, (const char*)R, r);
	bytes_to_big(SM2_NUMWORD, (const char*)S, s);

	//initialise public key
	//if (!epoint_set(PAx, PAy, 0, PA)) return ERR_PUBKEY_INIT;
	epoint_set(PAx, PAy, 0, PA);

	//step1: test if r belong to [1, n-1]
	//if (Test_Range(r)) return ERR_OUTRANGE_R;

	//step2: test if s belong to [1, n-1]
	//if (Test_Range(s)) return ERR_OUTRANGE_S;

	//step3, generate M
	M = (unsigned char *)malloc(sizeof(char) * (M_len + 1));
	memcpy(M, ZA, SM3_len / 8);
	memcpy(M + SM3_len / 8, message, len);

	//step4, generate e = H(M)
	SM3_256(M, M_len, hash);
	bytes_to_big(SM3_len / 8, (const char*)hash, e);

	//step5:generate t
	add(r, s, t);
	divide(t, para_n, rem);

	//if (Test_Zero(t)) return ERR_GENERATE_T;

	//step 6: generate(x1, y1)
	//ecurve_mult(s, G, sG);
	//ecurve_mult(t, PA, tPA);
	//ecurve_add(sG, tPA);
	ecurve_mult2(s, G, t, PA, tPA);
	epoint_get(tPA, x1, y1);

	//step7:generate RR
	add(e, x1, RR);
	divide(RR, para_n, rem);

	free(M);
	if (0 != mr_compare(RR, r))
	{
		mirexit();
		SM2_standard_clear(para_p,para_a,para_b,para_n,para_Gx,para_Gy, G);
		mirkill(PAx);
		mirkill(PAy);
		mirkill(r);
		mirkill(s);
		mirkill(e);
		mirkill(t);
		mirkill(rem);
		mirkill(x1);
		mirkill(y1);
		mirkill(RR);
		epoint_free(PA);
		epoint_free(sG);
		epoint_free(tPA);
		return ERR_DATA_MEMCMP;
	}

	mirexit();
	SM2_standard_clear(para_p,para_a,para_b,para_n,para_Gx,para_Gy, G);
	mirkill(PAx);
	mirkill(PAy);
	mirkill(r);
	mirkill(s);
	mirkill(e);
	mirkill(t);
	mirkill(rem);
	mirkill(x1);
	mirkill(y1);
	mirkill(RR);
	epoint_free(PA);
	epoint_free(sG);
	epoint_free(tPA);
	return 0;
}


//util functions
void hex2bytes(unsigned char* hex, int hlen, unsigned char* bytes)
{
	int n;
	char cH4, cL4;

	for (n = 0; n < hlen / 2; n++)
	{
		cH4 = hex[2 * n];
		cL4 = hex[2 * n + 1];

		cH4 = (cH4 >= '0' && cH4 <= '9') ? (cH4 - '0') : (cH4);
		cH4 = (cH4 >= 'a' && cH4 <= 'f') ? (cH4 - 'a' + 10) : (cH4);
		cH4 = (cH4 >= 'A' && cH4 <= 'F') ? (cH4 - 'A' + 10) : (cH4);

		cL4 = (cL4 >= '0' && cL4 <= '9') ? (cL4 - '0') : (cL4);
		cL4 = (cL4 >= 'a' && cL4 <= 'f') ? (cL4 - 'a' + 10) : (cL4);
		cL4 = (cL4 >= 'A' && cL4 <= 'F') ? (cL4 - 'A' + 10) : (cL4);

		bytes[n] = (unsigned char)(cH4 << 4 | cL4);
	}
}

void bytes2hex(unsigned char* bytes, int blen, char* hex)
{
	int n, m = 0;
	char hexMap[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

	for (n = 0; n < blen; n++)
	{
		hex[m++] = hexMap[(bytes[n] >> 4) & 0x0f];
		hex[m++] = hexMap[bytes[n] & 0x0f];
	}
}


void mhash(unsigned char* input, int len, int m, unsigned char* output)
{
	unsigned char ENTLA[2];
	unsigned char tmp[5];
	sprintf((char*)tmp, "%04X", m);
	hex2bytes(tmp, 4, ENTLA);
	memcpy(input + len + 2*SM2_NUMWORD, ENTLA, 2);
	SM3_256(input, len + 2*SM2_NUMWORD + 2, output);
}


int KGC_gen_partial_keys(unsigned char* XA, unsigned char* IDA, unsigned int IDAlen,
			 unsigned char* s, unsigned int slen, unsigned char* PA, unsigned char* za)
{
	unsigned char ya[32] = {0};
	unsigned char ebuff[32] = {0};
	unsigned char *mbuff = NULL;
	int m, i;
	big para_p, para_a, para_b, para_n, para_Gx, para_Gy;
	epoint *G;
	miracl *mip;
	big big_s, y, XAx, XAy, PAx, PAy, big_za, e, rem, tmp;
	epoint *ePA, *eXA;
	mip = mirsys(10000, 16);
	mip->IOBASE = 16;

	para_p = mirvar(0);
	para_a = mirvar(0);
	para_b = mirvar(0);
	para_n = mirvar(0);
	para_Gx = mirvar(0);
	para_Gy = mirvar(0);

	G = epoint_init();

	bytes_to_big(SM2_NUMWORD, SM2_p, para_p);
	bytes_to_big(SM2_NUMWORD, SM2_a, para_a);
	bytes_to_big(SM2_NUMWORD, SM2_b, para_b);
	bytes_to_big(SM2_NUMWORD, SM2_n, para_n);
	bytes_to_big(SM2_NUMWORD, SM2_Gx, para_Gx);
	bytes_to_big(SM2_NUMWORD, SM2_Gy, para_Gy);

	/*Initialises GF(p) elliptic curve.(MR_PROJECTIVE specifying projective coordinates)*/
	ecurve_init(para_a, para_b, para_p, MR_PROJECTIVE);

	/*initialise point G*/
	if (!epoint_set(para_Gx, para_Gy, 0, G)) return ERR_ECURVE_INIT;

	ePA = epoint_init();
	eXA = epoint_init();

	big_s = mirvar(0);
	y = mirvar(0);
	XAx = mirvar(0);
	XAy = mirvar(0);
	PAx = mirvar(0);
	PAy = mirvar(0);
	big_za = mirvar(0);
	e = mirvar(0);
	rem = mirvar(0);
	tmp = mirvar(0);

	SM2_gen_rand(ya, 32);
	bytes_to_big(SM2_NUMWORD, (char*)ya, y);

	bytes_to_big(SM2_NUMWORD, (const char*)XA, XAx);
	bytes_to_big(SM2_NUMWORD, (const char*)XA + SM2_NUMWORD, XAy);
	epoint_set(XAx, XAy, 0, eXA);

	//PA = XA + yG
	ecurve_mult(y, G, ePA);
	ecurve_add(eXA, ePA);

	epoint_get(ePA, PAx, PAy);

	big_to_bytes(SM2_NUMWORD, PAx, (char*)PA, TRUE);
	big_to_bytes(SM2_NUMWORD, PAy, (char*)PA + SM2_NUMWORD, TRUE);
	
	m = slen/32;

	mbuff = (unsigned char *)malloc(sizeof(char) * (IDAlen + 2*SM2_NUMWORD + 5));
	memcpy(mbuff, IDA, IDAlen);
	memcpy(mbuff + IDAlen, PA, 2*SM2_NUMWORD);
	
	for(i=0;i<m+1;i++)
	{
		mhash(mbuff, IDAlen, i, ebuff);
		bytes_to_big(SM2_NUMWORD, (const char*)ebuff, e);
		if(i==0)
		{
			multiply(e, y, tmp);
			divide(tmp, para_n, rem);
		}
		else
		{
			bytes_to_big(SM2_NUMWORD, (char*)s + (i-1)*SM2_NUMWORD, big_s);
			multiply(e, big_s, tmp);
			divide(tmp, para_n, rem);
		}
		add(tmp, big_za, big_za);
		divide(big_za, para_n, rem);
	}
	big_to_bytes(SM2_NUMWORD, big_za, (char*)za, TRUE);

	free(mbuff);
	mirexit();
	SM2_standard_clear(para_p,para_a,para_b,para_n,para_Gx,para_Gy, G);
	mirkill(big_s);
	mirkill(y);
	mirkill(XAx);
	mirkill(PAx);
	mirkill(PAy);
	mirkill(big_za);
	mirkill(e);
	mirkill(rem);
	mirkill(tmp);
	epoint_free(ePA);
	epoint_free(eXA);

	return 0;
}

int client_gen_keys(unsigned char* IDA, unsigned int IDAlen, unsigned char* xa,
		    unsigned char* PA, unsigned char* za, unsigned char* da, unsigned char* QA)
{
	unsigned char *mbuff = NULL;
	unsigned char ebuff[32] = {0};
	big para_p, para_a, para_b, para_n, para_Gx, para_Gy;
	epoint *G;
	miracl *mip;
	big e0, big_za, big_xa, big_da, QAx, QAy, rem, tmp;
	epoint *eQA;
	mip = mirsys(10000, 16);
	mip->IOBASE = 16;

	para_p = mirvar(0);
	para_a = mirvar(0);
	para_b = mirvar(0);
	para_n = mirvar(0);
	para_Gx = mirvar(0);
	para_Gy = mirvar(0);

	G = epoint_init();

	bytes_to_big(SM2_NUMWORD, SM2_p, para_p);
	bytes_to_big(SM2_NUMWORD, SM2_a, para_a);
	bytes_to_big(SM2_NUMWORD, SM2_b, para_b);
	bytes_to_big(SM2_NUMWORD, SM2_n, para_n);
	bytes_to_big(SM2_NUMWORD, SM2_Gx, para_Gx);
	bytes_to_big(SM2_NUMWORD, SM2_Gy, para_Gy);

	/*Initialises GF(p) elliptic curve.(MR_PROJECTIVE specifying projective coordinates)*/
	ecurve_init(para_a, para_b, para_p, MR_PROJECTIVE);

	/*initialise point G*/
	if (!epoint_set(para_Gx, para_Gy, 0, G)) return ERR_ECURVE_INIT;

	eQA = epoint_init();

	e0 = mirvar(0);
	big_za = mirvar(0);
	big_xa = mirvar(0);
	big_da = mirvar(0);
	QAx = mirvar(0);
	QAy = mirvar(0);
	rem = mirvar(0);
	tmp = mirvar(0);

	bytes_to_big(SM2_NUMWORD, (const char*)xa, big_xa);
	bytes_to_big(SM2_NUMWORD, (const char*)za, big_za);

	mbuff = (unsigned char *)malloc(sizeof(char) * (IDAlen + 2*SM2_NUMWORD + 5));
	memcpy(mbuff, IDA, IDAlen);
	memcpy(mbuff + IDAlen, PA, 2*SM2_NUMWORD);
	mhash(mbuff, IDAlen, 0, ebuff);
	bytes_to_big(SM2_NUMWORD, (const char*)ebuff, e0);

	//da = e0*xa +za
	multiply(e0, big_xa, tmp);
	divide(tmp, para_n, rem);
	add(tmp, big_za, big_da);
	divide(big_da, para_n, rem);
	big_to_bytes(SM2_NUMWORD, big_da, (char*)da, TRUE);

	//QA = da*G
	ecurve_mult(big_da, G, eQA);
	epoint_get(eQA, QAx, QAy);

	big_to_bytes(SM2_NUMWORD, QAx, (char*)QA, TRUE);
	big_to_bytes(SM2_NUMWORD, QAy, (char*)QA + SM2_NUMWORD, TRUE);

	free(mbuff);
	mirexit();
	SM2_standard_clear(para_p,para_a,para_b,para_n,para_Gx,para_Gy, G);
	mirkill(e0);
	mirkill(big_za);
	mirkill(big_xa);
	mirkill(big_da);
	mirkill(QAx);
	mirkill(QAy);
	mirkill(rem);
	mirkill(tmp);
	epoint_free(eQA);
	return 0;
}


int get_real_pubkey(unsigned char* IDA, unsigned int IDAlen, unsigned char* PA,
		    unsigned char* P, unsigned int Plen, unsigned char* QA)
{
	unsigned char *mbuff = NULL;
	unsigned char ebuff[32] = {0};
	int i,m;
	big para_p, para_a, para_b, para_n, para_Gx, para_Gy;
	epoint *G;
	miracl *mip;
	big e, Px, Py, QAx, QAy;
	epoint *eP, *eQA, *etmp;
	mip = mirsys(10000, 16);
	mip->IOBASE = 16;

	para_p = mirvar(0);
	para_a = mirvar(0);
	para_b = mirvar(0);
	para_n = mirvar(0);
	para_Gx = mirvar(0);
	para_Gy = mirvar(0);

	G = epoint_init();

	bytes_to_big(SM2_NUMWORD, SM2_p, para_p);
	bytes_to_big(SM2_NUMWORD, SM2_a, para_a);
	bytes_to_big(SM2_NUMWORD, SM2_b, para_b);
	bytes_to_big(SM2_NUMWORD, SM2_n, para_n);
	bytes_to_big(SM2_NUMWORD, SM2_Gx, para_Gx);
	bytes_to_big(SM2_NUMWORD, SM2_Gy, para_Gy);

	/*Initialises GF(p) elliptic curve.(MR_PROJECTIVE specifying projective coordinates)*/
	ecurve_init(para_a, para_b, para_p, MR_PROJECTIVE);

	/*initialise point G*/
	if (!epoint_set(para_Gx, para_Gy, 0, G)) return ERR_ECURVE_INIT;

	eP = epoint_init();
	eQA = epoint_init();
	etmp = epoint_init();

	e = mirvar(0);
	Px = mirvar(0);
	Py = mirvar(0);
	QAx = mirvar(0);
	QAy = mirvar(0);

	m = Plen/64;
	mbuff = (unsigned char *)malloc(sizeof(char) * (IDAlen + 2*SM2_NUMWORD + 5));
	memcpy(mbuff, IDA, IDAlen);
	memcpy(mbuff + IDAlen, PA, 2*SM2_NUMWORD);
	
	for(i=0;i<m+1;i++)
	{
		mhash(mbuff, IDAlen, i, ebuff);
		bytes_to_big(SM2_NUMWORD, (const char*)ebuff, e);
		if(i==0)
		{
			bytes_to_big(SM2_NUMWORD, (const char*)PA, Px);
			bytes_to_big(SM2_NUMWORD, (const char*)PA + SM2_NUMWORD, Py);
			epoint_set(Px, Py, 0, eP);
			ecurve_mult(e, eP, etmp);
		}
		else
		{
			bytes_to_big(SM2_NUMWORD, (const char*)P + 2*(i-1)*SM2_NUMWORD, Px);
			bytes_to_big(SM2_NUMWORD, (const char*)P + 2*i*SM2_NUMWORD - SM2_NUMWORD, Py);
			epoint_set(Px, Py, 0, eP);
			ecurve_mult(e, eP, etmp);
		}
		ecurve_add(etmp, eQA);
	}
	//ecurve_mult2(e0, PA, e1, P, eQA);

	epoint_get(eQA, QAx, QAy);
	big_to_bytes(SM2_NUMWORD, QAx, (char*)QA, TRUE);
	big_to_bytes(SM2_NUMWORD, QAy, (char*)QA + SM2_NUMWORD, TRUE);

	free(mbuff);
	mirexit();
	SM2_standard_clear(para_p,para_a,para_b,para_n,para_Gx,para_Gy, G);
	mirkill(e);
	mirkill(Px);
	mirkill(Py);
	mirkill(QAx);
	mirkill(QAy);
	epoint_free(eP);
	epoint_free(eQA);
	epoint_free(etmp);

	return 0;
}
