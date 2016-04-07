#define OPENSSL_VERIFY "\ 
	#!/bin/bash \n\	
	openssl dgst -ecdsa-with-SHA1 -verify public.pem -signature sign.out input.txt \n\
	"
