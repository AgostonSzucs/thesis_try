#include <openssl/sha.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <stdio.h>
#include <string.h>
int pass_cb(char *buf, int size, int rwflag, void *u);

int main(void){
	BIO               *outbio = NULL;
	BIO	*inbio = NULL;
	BIO	*inbio_sign = NULL;
	EVP_PKEY	*key   = NULL;
	EC_KEY            *eckey  = NULL;	        
        char *data;
	size_t length = sizeof(data);
	int ret;
	ECDSA_SIG *sig;	
	FILE *fp;
	FILE *fp_in;
	long lSize;
	int c;
	int i = 0,j = 0;
	unsigned char *buffer;
	unsigned char *buffer_temp;
	
	unsigned char hash[SHA_DIGEST_LENGTH];
	
	/* ---------------------------------------------------------
	* READ input file
	*  --------------------------------------------------------- */			
	fp_in = fopen ("/home/agoston/openssltry/elliptic/input.txt", "rb");
	if( !fp ){
		perror("input");
		return 1;
	}

	fseek( fp_in , 0L , SEEK_END);
	lSize = ftell(fp_in);
	printf("%d \n",(int)lSize);
	rewind(fp_in);

	data = (char*)malloc(lSize);
	if( 1!=fread(data,lSize,1,fp_in)){
		printf("File_ERROR");
		return 1;
	}

	fclose(fp_in);

	printf("hello");
		
	ERR_load_BIO_strings();
	SHA1(data,lSize,hash);

	/* ---------------------------------------------------------- *
	* Create the Input/Output BIO's.                             *
	* ---------------------------------------------------------- */
	outbio = BIO_new(BIO_s_file());
	outbio = BIO_new_fp(stdout, BIO_NOCLOSE);
	
	/* ---------------------------------------------------------
	* READ signed file
	*  --------------------------------------------------------- */			
	fp = fopen ("/home/agoston/openssltry/elliptic/signed_files/sign.out", "rb");
	if( !fp ){
		perror("sign.out");
		return 1;
	}	

	fseek( fp , 0L , SEEK_END);
	lSize = ftell(fp);
	printf("%d \n",(int)lSize);
	rewind(fp);

	buffer = (char*)malloc(lSize);
	printf("%d \n",(int)strlen(buffer));
	/*if( 1!=fread(buffer,lSize,1,fp)){
		printf("File_ERROR");
		return 1;
	}*/
	printf("START of display \n");
	while(!feof(fp)){
		printf("i value: %d \n", i);
		c = fgetc(fp);
		buffer_temp = NULL;
		buffer_temp = malloc (sizeof (int) * (i+1));
		for(j = 0 ;  j < i ; j++){
			//printf("hello");
			buffer_temp[j] = buffer[j];
		}
		buffer_temp[i]=c;
		for(j = 0 ;  j < i ; j++){
			printf("%c",buffer_temp[j]);
		}
		printf(" \n");
		for(j = 0 ;  j < i ; j++){
			//printf("hello");
			//printf("%d -- %c",j,buffer_temp[j]);	
		}
		printf("BUFFER_TEMP: %s SIZE: %lu \n",buffer_temp, strlen(buffer_temp));
		//printf("\n");
		//printf("%d -- %c \n",i,buffer_temp[i]);
		buffer = malloc (sizeof (int) * (i+1));
		for(j = 0 ;  j <= i ; j++){
			//printf("hello");
			buffer[j] = buffer_temp[j];
		}
		printf("BUFFER: %s \n",buffer);
		//buffer = buffer_temp;	
		//printf("%s",buffer);
		//c = fgetc(fp);
		//printf("%c",c);
		printf("\n");
		i++;
	}
	printf("\n");
	printf("%s \n",buffer);
	printf("%lu \n",sizeof (buffer));	
	for(j = 0 ;  j < (int)strlen(buffer) ; j++){
			printf("%c",buffer[j]);
	}
	fclose(fp);
	/*
	//inbio = BIO_new(BIO_s_file());	
	inbio = BIO_new_file("/home/agoston/openssltry/elliptic/openssl_keys_201603142010/private.pem", "r");	
	key = PEM_read_bio_PrivateKey(inbio, NULL, pass_cb, "My private Key");
	if(!PEM_write_bio_PrivateKey(outbio, key, NULL, NULL, 0, 0, NULL))
		BIO_printf(outbio, "Error writing private key data in PEM format");
	
	if (key == NULL){
		printf("%s \n", "EVP_ERROR");
	}
	
	eckey = EVP_PKEY_get1_EC_KEY(key);
	if(eckey == NULL) {
		printf("%s \n", "EC_ERROR");
	}

	sig = ECDSA_do_sign(hash, strlen(hash), eckey);
 	if (sig == NULL) {
		printf("%s \n", "SIGN_ERROR");
		printf("%s \n", ERR_reason_error_string(ERR_get_error()));
 	}
	else{
		printf("r: %s\n", BN_bn2hex(sig->r));
    		printf("s: %s\n", BN_bn2hex(sig->s));
	}

	*/
	inbio = BIO_new_file("/home/agoston/openssltry/elliptic/openssl_keys_201603142010/public.pem", "r");	
	key = PEM_read_bio_PUBKEY(inbio, NULL, pass_cb, "My public Key");
	
	if(!PEM_write_bio_PUBKEY(outbio, key))
		BIO_printf(outbio, "Error writing public key data in PEM format");

	if (key == NULL){
		printf("%s \n", "EVP_ERROR");
	}
	
	eckey = EVP_PKEY_get1_EC_KEY(key);
	if(eckey == NULL) {
		printf("%s \n", "EC_ERROR");
	}
	ret = ECDSA_verify(0, hash, strlen(hash), buffer, lSize, eckey);
	//ret = ECDSA_do_verify(hash, strlen(hash), sig, eckey);
	
	printf("%d", ret);	
	if (ret == 1) {
		/* signature ok */
		printf("%s \n", "VER_OK");
	} else if (ret == 0) {
    		/* incorrect signature */
		printf("%s \n", "VER_NOK");
 	} else {
  		/* error */
		printf("%s \n", "VER_ERROR");
 	}
	
	return 0;
}

int pass_cb(char *buf, int size, int rwflag, void *u){
	int len;
	char *tmp;

	/* We'd probably do something else if 'rwflag' is 1 */
	printf("Enter pass phrase for \"%s\"\n", (char *)u); 

	/* get pass phrase, length 'len' into 'tmp' */
	tmp = "hello";
	len = strlen(tmp);
	if (len <= 0)
		return 0;

	if (len > size)
		len = size;
	memcpy(buf, tmp, len);
	
	return len;
}
