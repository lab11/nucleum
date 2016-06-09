#!/usr/bin/python

import sys

startAddress = "00004000";

# open file
inFile = open(sys.argv[1], "r")
outFile = open(sys.argv[2], "w")

def toVoltage(adcVoltage):
	return (adcVoltage/1024.0)*1.2*3;

def toCurrent(adcCurrent):
	return (((adcCurrent/1024.0)*1.2)*(1.0/(2.0/3.0)))/1.17;

def strToInt(s):
	i = int(s,16)
	if (i >= 2**15):
		i -= 2**16
	return i


#read to the start of memory we want
line = inFile.readline()
while(line[0:8] != startAddress):
	#print("read " + line[0:8])
	line = inFile.readline()

#we got to where we want - process to csv
outFile.write("Time,Open Voltage,Short Current\n")

#first line
day = 0;
hour = 0; 
sec = 0;
min = 0;

voc = 0;
isc = 0;

line = inFile.readline()
while(line):
	words = line.split()
	if(not words):
		line = inFile.readline()
		continue
	if(not words[0]):
		line = inFile.readline()
		continue
	if(words[0][0] != '0'):
		line = inFile.readline()
		continue
	for i in range(2,6):
		if(words[i] == "FFFFFFFF"):
			pass
			#outFile.write("na,na\n")
		else:
			if(not(i % 2)):
				#this must be time
				day = strToInt(words[i][0:2]);
				hour = strToInt(words[i][2:4]);
				min = strToInt(words[i][4:6]);
				sec = strToInt(words[i][6:8]);
				outFile.write(str(day) + ":" + 
					  	str(hour) + ":" + 
						str(min) + ":" + 
						str(sec) + ","); 
			else:
				#this must be measurements
				voc = strToInt(words[i][0:4]);
				isc = strToInt(words[i][4:8]);
				outFile.write("{:.3}".format(toVoltage(voc)) + "," + 
							 	"{:.3}".format(toCurrent(isc))+"\n");


	line = inFile.readline()

inFile.close()
outFile.close()
