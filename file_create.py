#!/usr/bin/python
import sys
import json 
input_array = {'user':sys.argv[1],'rights':sys.argv[2],'validate_time':sys.argv[3]}
json_format=json.dumps(input_array)
print 'Argument List:', str(input_array)
print 'Argument List:', str(json_format)
f_write = open("/home/agoston/openssltry/elliptic/input_files/input.json","w").write(str(json_format))

