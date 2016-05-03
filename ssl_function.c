#include <openssl/sha.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int pass_cb(char *buf, int size, int rwflag, void *u);
int verify_token(char *input_data, char *signed_data, char *pubkey_path);
size_t base64DecodeLength(char *b64);


int main(void){

	FILE *fp_in;
	char *data;
	long lSize;
	char pub_path [300];
	
	char signed_input[200];
	int result;	

	strcpy(pub_path,"/home/agoston/openssltry/thesis_try/openssl_keys_201605031332/public.pem");
	printf("Public:%s \n", pub_path);
	
	/* ---------------------------------------------------------
	* READ input file
	*  --------------------------------------------------------- */			
	fp_in = fopen ("/home/agoston/openssltry/thesis_try/input_files/input.json", "r");
	if( !fp_in ){
		perror("input");
		return 1;
	}

	fseek( fp_in , 0L , SEEK_END);
	lSize = ftell(fp_in);
	//printf("%d \n",(int)lSize);
	rewind(fp_in);

	data = (char*)malloc(lSize);
	if( 1!=fread(data,lSize,1,fp_in)){
		printf("File_ERROR");
		return 1;
	}
	fclose(fp_in);
	printf("\n Input string:");
	puts(data);
	printf("\n Enter Signed data:");
	gets(signed_input);

	/* ---------------------------------------------------------
	* READ signed file
	*  --------------------------------------------------------- */			
	/*	
	fp_in = fopen ("/home/agoston/openssltry/thesis_try/signed_files/signb.out", "rb");
	if( !fp_in ){
		perror("sign.out");
		return 1;
	}	

	fseek( fp_in , 0L , SEEK_END);
	lSize = ftell(fp_in);
	printf("%d \n",(int)lSize);
	rewind(fp_in);

	signed_input = (char*)malloc(lSize);
	if( 1!=fread(signed_input,lSize,1,fp_in)){
		printf("File_ERROR");
		return 1;
	}
	
	fclose(fp_in);
	
	printf("%d \n",(int)strlen(signed_input));
*/	
	result = verify_token(data,signed_input,pub_path);
	printf("\n Result of the verify: %d ",result);

}

int verify_token(char *input_data, char *signed_datab64, char *pubkey_path){
	
	BIO *inbio = NULL;
	BIO *b64 = NULL;
	BIO *bio = NULL;
	
	EVP_PKEY  *key = NULL;
	EC_KEY *eckey = NULL;

	char *signed_data;	

	int ret;

	size_t input_length = strlen(input_data);
	size_t signedb64_length = strlen(signed_datab64);
	size_t signed_length = base64DecodeLength(signed_datab64);	
	size_t read_length;	


	printf("\nLength:\n%d \n",(int)input_length);
	printf("%d \n",(int)signedb64_length);
	printf("%d \n",(int)signed_length);

	signed_data = (char*)malloc(signed_length + 1);
	signed_data[signed_length] = '\0';

	const char *c_pubkey_path= (const char*)pubkey_path;
	
	unsigned char hash [SHA_DIGEST_LENGTH];

	ERR_load_BIO_strings();
	SHA1(input_data,input_length,hash);
	
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new_mem_buf(signed_datab64, -1);	
	
	bio = BIO_push(b64,bio);	

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	printf("\nOriginal file: \n%s \n", signed_datab64);

	read_length = BIO_read(bio, signed_data, signedb64_length);
	
	printf("%d \n",(int)read_length);
	if(read_length != signed_length){
		printf( "%s \n", "READ_BIO_ERROR");
	}
	BIO_puts(b64,signed_datab64);		
	BIO_gets (b64,signed_data,200);	

	printf("\nNew file: \n%s \n", signed_data);	
	//signed_length = sizeof(signed_data);	

	inbio = BIO_new_file(c_pubkey_path, "r");
	key = PEM_read_bio_PUBKEY(inbio, NULL, pass_cb , "My public key");
	
	if (key == NULL){
		printf( "%s \n", "EVP_ERROR"); 
	}

	eckey = EVP_PKEY_get1_EC_KEY(key);
	if(eckey == NULL) {
		printf("%s \n", "EC_ERROR");
	}
	ret = ECDSA_verify(0, hash, strlen(hash), signed_data, read_length, eckey);

	return ret;

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

size_t base64DecodeLength(char *b64){

	size_t b64len = strlen(b64);
	size_t padding = 0;
	size_t db64len = 0;	

	if(b64[b64len-1] == '=' && b64[b64len-2] == '='){
		padding = 2;
	}
	else if (b64[b64len-1] == '='){
		padding = 1;
	}

	db64len = (b64len*3)/4 - padding;
	
	return db64len;
}
