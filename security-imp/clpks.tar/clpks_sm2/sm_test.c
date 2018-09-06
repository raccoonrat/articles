#include "sm2.h"
#include "math.h"
#include "time.h"
#include "sys/time.h"
#include "sm2alg.h"
#include <pthread.h>

int test_GM_signature_and_verify()
{
	srand((unsigned)time(NULL));
	printf("\n********\n* GM sm2 signature and verification test\n********\n");
	printf("To check details, make under \'-D_DEBUG\' when compile...\nIn addition, make under \'-D_TEST\' to avoid randomness\n\n");

	int m = 1;
	unsigned char *KGC_prikey = NULL;//s
	unsigned int KGC_prilen = m*32;
	unsigned char *KGC_pubkey = NULL;//P
	unsigned int KGC_publen = m*64;
	unsigned char client_xa[32] = {0};
	unsigned int xalen = SM2_NUMWORD;
	unsigned char client_XA[64] = {0};
	unsigned int XAlen = SM2_NUMWORD*2;
	unsigned char PA[64] = {0};
	unsigned int PAlen = SM2_NUMWORD*2;
	unsigned char za[32] = {0};
	unsigned int zalen = SM2_NUMWORD;
	unsigned char client_final_prikey[32] = {0};
	unsigned int dlen = SM2_NUMWORD;
	unsigned char QA[64] = {0};
	unsigned int QAlen = SM2_NUMWORD*2;
	unsigned char bQA[64] = {0};
	unsigned int bQAlen = SM2_NUMWORD*2;
	int ret = 0;
	int i,j;
	const char *IDA = "ALICE123@YAHOO.COM";
	int IDAlen = strlen(IDA);
	const char *src = "message digest";
	int  lenSrc = strlen(src);
	unsigned char sig[100] = {0};
	unsigned long ulsiglen = 100;
	KGC_prikey = (unsigned char*)malloc(sizeof(char) * (m*SM2_NUMWORD));
	KGC_pubkey = (unsigned char*)malloc(sizeof(char) * (m*SM2_NUMWORD*2));

	//gen random system prime private key
	SM2_gen_rand(KGC_prikey, m*SM2_NUMWORD);

	//run_time test
	double duration;
	struct timeval t1, t2;
	int time_factor = 1000000;
	gettimeofday(&t1, NULL);

//---------------------------------------------------------------------------
	//test KGC setup
	ret = CLPK_KGC_setup(m, KGC_prikey, KGC_prilen, KGC_pubkey, KGC_publen);
	if(ret == 0)
	{
		printf("\n...setup ok...\n");
		gettimeofday(&t2, NULL);
		duration = (t2.tv_sec - t1.tv_sec) * time_factor + t2.tv_usec - t1.tv_usec;
		duration /= time_factor;
		for(j=0;j<m;j++)
		{
			printf("prime prikey%d:\n",j+1);
			for(i = 0; i < SM2_NUMWORD; i++)
			{
				printf("%02X", KGC_prikey[j*SM2_NUMWORD + i]);
			}
			printf("\n");
			printf("prime pubkey%d:\n",j+1);
			for(i = 0; i < SM2_NUMWORD*2; i++)
			{
				printf("%02X", KGC_pubkey[j*SM2_NUMWORD*2 + i]);
			}
			printf("\n");
		}
		printf("***Setup time cost: %f\n", duration);
		gettimeofday(&t1, NULL);
	}
	else
	{
		printf("\n...KGC setup failed!\n");
		CHECK_RET(ret);
	}

//---------------------------------------------------------------------------
	//test client generate XA
	gettimeofday(&t1, NULL);
	SM2_gen_rand(client_xa, xalen);
	ret = CLPK_client_gen_XA(client_xa, xalen, client_XA, XAlen);
	gettimeofday(&t2, NULL);
	if (ret == 0)
	{
		printf("\n... client gen XA ok!\n");
		duration = (t2.tv_sec - t1.tv_sec) * time_factor + t2.tv_usec - t1.tv_usec;
		duration /= time_factor;
		printf("***client gen XA time cost: %f\n", duration);
		printf("xa: ");
		for(i = 0; i < SM2_NUMWORD; i++)
		{
			printf("%02X", client_xa[i]);
		}
		printf("\nXA: ");
		for(i = 0; i < SM2_NUMWORD*2; i++)
		{
			printf("%02X", client_XA[i]);
		}
		printf("\n");

	}
	else
	{
		printf("\n...client gen XA failed!\n");
		CHECK_RET(ret);
	}
//---------------------------------------------------------------------------
	//test KGC gen partial keys
	gettimeofday(&t1, NULL);
	ret = CLPK_KGC_gen_partial_keys(client_XA, XAlen, (unsigned char*)IDA, IDAlen, KGC_prikey, KGC_prilen, PA, PAlen, za, zalen);
	gettimeofday(&t2, NULL);
	if (ret == 0)
	{
		printf("\n... KGC gen partial keys ok!\n");
		duration = (t2.tv_sec - t1.tv_sec) * time_factor + t2.tv_usec - t1.tv_usec;
		duration /= time_factor;
		printf("***KGC gen partial keys time cost: %f\n", duration);
		printf("PA: ");
		for(i = 0; i < PAlen; i++)
		{
			printf("%02X", PA[i]);
		}
		printf("\nza: ");
		for(i = 0; i < zalen; i++)
		{
			printf("%02X", za[i]);
		}
		printf("\n");

	}
	else
	{
		printf("\n...KGC gen partial keys failed!\n");
		CHECK_RET(ret);
	}

//---------------------------------------------------------------------------
	//test client gen final keys
	gettimeofday(&t1, NULL);
	ret = CLPK_client_gen_keys((unsigned char*)IDA, IDAlen, client_xa, xalen,
		       	PA, PAlen, za, zalen, client_final_prikey, dlen, QA, QAlen);
	gettimeofday(&t2, NULL);
	if (ret == 0)
	{
		printf("\n... client gen final keys ok!\n");
		duration = (t2.tv_sec - t1.tv_sec) * time_factor + t2.tv_usec - t1.tv_usec;
		duration /= time_factor;
		printf("***client gen final keys time cost: %f\n", duration);
		printf("da: ");
		for(i = 0; i < dlen; i++)
		{
			printf("%02X", client_final_prikey[i]);
		}
		printf("\nQA: ");
		for(i = 0; i < QAlen; i++)
		{
			printf("%02X", QA[i]);
		}
		printf("\n");

	}
	else
	{
		printf("\n...client gen final keys failed!\n");
		CHECK_RET(ret);
	}

//---------------------------------------------------------------------------
	//test get real pubkey
	gettimeofday(&t1, NULL);
	ret = CLPK_get_real_pubkey((unsigned char*)IDA, IDAlen, PA, PAlen, KGC_pubkey, KGC_publen, bQA, bQAlen);
	gettimeofday(&t2, NULL);
	if (ret == 0)
	{
		printf("\n... get real pubkey ok!\n");
		duration = (t2.tv_sec - t1.tv_sec) * time_factor + t2.tv_usec - t1.tv_usec;
		duration /= time_factor;
		printf("***get real pubkey time cost: %f\n", duration);
		printf("\nreal QA: ");
		for(i = 0; i < bQAlen; i++)
		{
			printf("%02X", bQA[i]);
		}
		printf("\n");

	}
	else
	{
		printf("\n...get real pubkey failed!\n");
		CHECK_RET(ret);
	}
/*
//-----------------------------------------------------------------------------------------------
	ret = GM_SM2VerifySig(sig, ulsiglen, (unsigned char *)src, lenSrc, (unsigned char *)uid, lenUid,
	                      set_pub_key, set_pubkeyLen);
	gettimeofday(&t2, NULL);
	duration = (t2.tv_sec - t1.tv_sec) * time_factor + t2.tv_usec - t1.tv_usec;
	duration /= time_factor;
	printf("***Verufy Signature time cost: %f\n", duration);
	if (ret == 0)
	{
		printf("\n...verify ok...\n");
	}
	else
	{
		printf("\n...verify failed!\n");
		CHECK_RET(ret);
	}
*/

END:
	free(KGC_prikey);
	free(KGC_pubkey);
	printf("********\n* test end\n********\n");
	return ret;

}

void *PthreadGetCpu(void *arg)
{
	int ret = 0;
	int i;
	int count;
	count = 0;
	for(i = 0; i < 1; i++)
	{
		ret = test_GM_signature_and_verify();
		if(ret == 0)
			count += 1;
		else
			break;
		printf("\nsuccess rate: %d/%d\n", count, i + 1);
	}
	return NULL;
}
void *PthreadGetCpu1(void *arg)
{
	int ret = 0;
	int i;
	int count;
	count = 0;
	for(i = 0; i < 1; i++)
	{
		ret = test_GM_signature_and_verify();
		if(ret == 0)
			count += 1;
		else
			break;
		printf("\nsuccess rate: %d/%d\n", count, i + 1);
	}
	return NULL;
}

int main()
{
	sm2_init_thread();
	pthread_t thread1;//thread2;
	pthread_attr_t attributes;
	void *status;
	pthread_attr_init(&attributes);
	pthread_create(&thread1, &attributes, (void* (*)(void*))PthreadGetCpu1, NULL);
	//pthread_create(&thread2, &attributes, (void* (*)(void*))PthreadGetCpu1, NULL);
	pthread_join(thread1,&status);
	//pthread_join(thread2,&status);
	pthread_attr_destroy(&attributes);
	pthread_cancel(thread1);
	//pthread_cancel(thread2);
	pthread_detach(thread1);
	//pthread_detach(thread2);
	return 0;
}

