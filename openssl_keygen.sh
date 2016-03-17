#!/bin/bash
function check_command(){
	if [ $? -eq 0 ]; then
		printf "${BLUE}$1 was succesful!${NC}\n"	
		return 0	
	else
		printf "${RED}$1 was failed!${NC}\n"
		exit 1	
	fi
}
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color
current_date=$(date +"%F")$(date +"%H")$(date +"%M")
current_date=$(echo ${current_date} | sed "s/-//g;")
printf " ==== Begining of the openssl key generation ====\n"
printf "\nFirst step: making a directory for the keys\n"
directory_name=$"openssl_keys_"${current_date}
mkdir $directory_name
check_command "Directory creation"
cd $directory_name
printf "Current directory:$(pwd)\n"
printf "\nSecond step: creating private key\n"
openssl ecparam -genkey -name secp384r1 -noout -out private.pem
check_command "Private key creation"
printf "\nThird step: creating public key\n"
openssl ec -in private.pem -pubout -out public.pem
check_command "Public key creation"




