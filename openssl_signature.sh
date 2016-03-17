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
BOLD='\033[1m' # BOLD
enable=0
HOME_DIR=/home/agoston/openssltry/elliptic
SIGN_HOME=${HOME_DIR}/signed_files
id
printf "${BOLD}SWITCHING to openssl app home!${NC}\n"
cd $HOME_DIR
pwd
printf "${BOLD}Choose a key:${NC}\n"
while [ $enable -lt 1 ]
do	
	read comm
	if [ ${comm} = "list" ]; then	
		printf "${RED}"
		keys=$(find -path "./*openssl_keys*" -prune | sed "s/openssl_keys_//;s/\.//;s/\///")
		printf "$keys${NC}\n"
	else
		if [ $(echo $keys | grep -wc $comm) = "1" ]; then
			key=$comm			
			KEY_HOME=${HOME_DIR}/openssl_keys_$key			
			printf "Valid key choosed: ${BLUE}$key${NC}\n"
			enable=1
		else
			printf "${RED}Not a valid key!${NC}\n"
			exit 1
		fi
	fi
done
printf "\n${BOLD}Generate an input file:${NC}\n"
printf "User:\n"
read user
printf "Right:\n"
read right
printf "Validate time (h):\n"
read valid
./file_create.py "${user}" "${right}" "${valid}"
check_command "Input file generation"
cat input_files/input.json | python -mjson.tool
printf "\n${BOLD}Sign the input file${NC}\n"
openssl dgst -ecdsa-with-SHA1 -sign ${KEY_HOME}/private.pem -out ${SIGN_HOME}/sign.out input.txt
check_command "Input files sign"



