#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <pthread.h>
#include "serial.c"

struct Args {
	const char *server;
	const char *port;
	const char *user;
	const char *proxyhost;
	const char *proxyport;
	const char *password;
	const char *nmea;
	const char *data;
	int bitrate;
	int mode;

	int udpport;
	int initudp;
	enum SerialBaud baud;
	enum SerialDatabits databits;
	enum SerialStopbits stopbits;
	enum SerialParity parity;
	enum SerialProtocol protocol;
	const char *serdevice;
	const char *serlogfile;
};

//SML HARD-CODED VALUES START
const char* SerialPort = "/dev/ttyUSB0";
//SML HARD-CODED VALUES END
int SerialSpeed = 9600;
int SerialDataBits = 8;
int SerialStopBits = 1;
int SerialParity = 0;
int SerialProtocol = 0;
int Stream = 0;
std::string ReceiveBuffer;

//SML HARD-CODED VALUES START
std::string NTRIPCaster = "gnssdata.or.kr";
int NTRIPPort = 2101;
std::string NTRIPUsername = "gnss";
std::string NTRIPPassword = "gnss";
std::string NTRIPMountPoint = "SOUL-RTCM32";
//SML HARD-CODED VALUES END
std::string MostRecentGGA =
		"$GPGGA,052158,4158.7333,N,09147.4277,W,2,08,3.1,260.4,M,-32.6,M,,*79";
struct serial sx;
struct Args args;

void SerialInput();
int ProcessNMEAdata(std::string &x);
std::string CalculateChecksum(std::string &sentence);
std::string ToBase64(unsigned char const* bytes_to_encode, unsigned int in_len);

int main(int argc, char *argv[]) {

	struct sockaddr_in serv_addr;
	struct hostent *server;
	int lcount = 97;

	//Connect to server
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(0);
	}
	server = gethostbyname(NTRIPCaster.c_str());
	if (server == NULL) {
		fprintf(stderr, "ERROR, NTRIPCaster does not exist\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(NTRIPPort);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {
		perror("ERROR connecting to NTRIPCaster");
		exit(0);
	}

	//Build request message
	std::string msg = "GET /" + NTRIPMountPoint + " HTTP/1.0\r\n";
	msg += "User-Agent: NTRIP LefebureNTRIPClient/20131124\r\n";
	msg += "Accept: */*\r\n";
	msg += "Connection: close\r\n";
	if (NTRIPUsername.length() > 0) {
		// auth = "gnss:gnss", auth_encoded = "Z25zczpnbnNz"
		std::string auth = NTRIPUsername + ":" + NTRIPPassword;
		std::string auth_encoded = ToBase64(
				reinterpret_cast<const unsigned char*>(auth.c_str()),
				auth.length());
		msg += "Authorization: Basic " + auth_encoded + "\r\n"; //This line can be removed if no authorization is needed
	}
	msg += "\r\n";

	//Send request
	char *data = new char[msg.length() + 1];
	std::strcpy(data, msg.c_str());
	if (write(sockfd, data, strlen(data)) < 0) {
		perror("ERROR writing to socket");
		exit(0);
	}
	//Wait for response
	int DataNotReceivedFor = 0;
	std::string responseData;
	while (!strstr(responseData.c_str(), "ICY 200 OK")) {
		char InBytes[1000];
		int DataLength = read(sockfd, InBytes, strlen(InBytes));
		if (DataLength > 0) {
			responseData.append(InBytes);
			if (strstr(responseData.c_str(), "401 Unauthorized")) {
				perror("Invalid user name or password");
				close(sockfd);
				exit(0);
			}
		} else if (DataLength == 0) {
			DataNotReceivedFor += 1;
			if (DataNotReceivedFor > 300) {
				//Data not received for 30 seconds. Terminate the connection.
				perror("Wait for response - Connection timed out");
				close(sockfd);
				exit(0);
			}
		} else {
			perror("ERROR reading from socket");
			exit(0);
		}
	}

	printf("%s\n", responseData.c_str());

	//Initialize Serial Device Connection
	args.serdevice = "/dev/ttyUSB0";
	args.baud = SPABAUD_9600;
	args.stopbits = SPASTOPBITS_1;
	args.protocol = SPAPROTOCOL_NONE;
	args.parity = SPAPARITY_NONE;
	args.databits = SPADATABITS_8;
	const char *e = SerialInit(&sx, args.serdevice, args.baud, args.stopbits,
			args.protocol, args.parity, args.databits, 1);
	if (e) {
		fprintf(stderr, "%s\n", e);
		return 20;
	}

	//p_thread(());

	//ICY 200 OK, Waiting for data
	std::string serialData;
	DataNotReceivedFor = 0;
	bool KeepRunning = true;
	while (KeepRunning) {
		char InBytes[1000];
		int DataLength = recv(sockfd, InBytes, strlen(InBytes), 0);
		if (DataLength > 0) {
			DataNotReceivedFor = 0;
			printf("InBytes : %s\n" , InBytes);
			/*

			std::string serialData = InBytes;
			std::cout << serialData << std::endl;
			char * MyBytes = new char[serialData.length() + 1];
			std::strcpy(MyBytes, serialData.c_str());
			printf("%s\n", MyBytes);
			int j = write(Stream, MyBytes, strlen(MyBytes));
			if (j < 0) {
				fprintf(stderr, "Could not access serial device\n");
			}
			*/
		} else if (DataLength == 0) {
			/*
			DataNotReceivedFor += 1;
			if (DataNotReceivedFor > 300) {
				//Data not received for 30 seconds. Terminate the connection.
				KeepRunning = false;
				perror("Waiting for data - Connection timed out");
				close(sockfd);
				exit(0);
			}
			*/
		} else {
			perror("ERROR reading from socket");
			exit(0);
		}

		lcount += 1;
		if (lcount == 100) {
			SerialInput();
			std::string TheGGA = MostRecentGGA; //From GPS::ProcessNMEAdata()
			TheGGA += "\r\n";
			char *nmeadata = new char[TheGGA.length() + 1];
			std::strcpy(nmeadata, TheGGA.c_str());
			printf("nmeadata : %s", nmeadata);
			if (send(sockfd, nmeadata, strlen(nmeadata), 0) < 0) {
				perror("ERROR writing to socket");
				exit(0);
			}
			lcount = 0;
		}
	}

	SerialFree(&sx);
	return 0;
}

void SerialInput() {
	//I got error message that reinterpret_cast can't cast from const char* to char*
	//ReceiveBuffer.c_str() is const char*, So i used method casting ReceiveBuffer type to char*

	////////////////////////////////      Serial Read[Raw Data] Start     //////////////////////////////////////////
	char readSerial[1000];
	int i = 0;
	while ((i = SerialRead(&sx, readSerial, 300)) <= 0) {
		sleep(1);
	}
	ReceiveBuffer += readSerial;

	std::vector<char> writable(ReceiveBuffer.begin(), ReceiveBuffer.end());
	writable.push_back('\0');
	char* ptr = &writable[0];

	////////////////////////////////           Serial Read End            //////////////////////////////////////////
	if (ReceiveBuffer.find("\r\n")) {
		int count = 0;
		//Contains at least one carridge return
		char *lines = strtok(reinterpret_cast<char*>(ptr), "\n");
		std::vector<std::string> lineVector;
		while(lines != NULL){
			if (strlen(lines) > 5){
				lineVector.push_back(lines);
			}
			lines = strtok(NULL, "\n");
		}

		for(int i=0; i<lineVector.size(); i++){
			//printf("%s", lineVector[i].c_str());
			if (ProcessNMEAdata(lineVector[i]) == 1)
			{
				//printf("Congrat!!");
				break;
			}
		}

		//ReceiveBuffer.assign(last_line);
	} else {
		//Data doesn't contain any line breaks
		if (ReceiveBuffer.length() > 4000) {
			ReceiveBuffer = "";
			//No line breaks found in data stream
		}
	}
	//ProcessNMEAdata(ReceiveBuffer);
}

int ProcessNMEAdata(std::string &x){
	//printf("        %s\n", x.c_str());
	int charlocation = (int) x.rfind("$"); // Find location of last $
	if (charlocation == -1 || charlocation + 1 > x.length() - 5) {
		//no $ found or not enough data left
		return 0;
	}
	//drop characters before the $
	x = x.substr(charlocation);

	charlocation = (int) x.find("*"); //Find location of first *

	if (charlocation == -1) {
		//no * found
		return 0;
	}
	if (x.length() < charlocation + 3) {
		//there aren't 2 characters after the *
		return 0;
	} else if (x.length() > charlocation + 3) { //there is extra data after the *
		//remove the extra data after 2 chars after the *
		x = x.substr(0, charlocation + 3);

	}
	if (x.length() < 8) {
		//not enough data left
		return 0;
	}
 	//printf("%s\n", x.c_str());

	std::vector<char> v(x.begin(), x.end());

	v.push_back('\0');

	char* ptr = &v[0];


	char *aryNMEALine = strtok(reinterpret_cast<char*>(ptr),"*");

	std::string aryNMEALine1 = aryNMEALine;
	aryNMEALine = strtok(NULL,"*");

	std::string aryNMEALine2 = aryNMEALine;
	aryNMEALine1.erase(0,1);

	//printf("1:%s\n", aryNMEALine1.c_str());
	//printf("2:%s\n",aryNMEALine2.c_str());

	//lets see if the checksum matches the stuff before the astrix

	if (CalculateChecksum(aryNMEALine1) == aryNMEALine2) {
		//Checksum matches, send it to the stuff before the astrix
		if (strstr(aryNMEALine1.c_str(), "GPGGA") || strstr(aryNMEALine1.c_str(), "GNGGA")) {
			MostRecentGGA = x;
			//printf("MostRecentGGA: %s\n", MostRecentGGA.c_str());
			return 1;
		}
	}
	return 0;
}


//Calculate the checksum for a sentence
std::string CalculateChecksum(std::string &sentence) {
	int Checksum = 0;
	//Loop through all chars to get checksum
	std::string::iterator it = sentence.begin();
	//Stop processing before the asterisk
	while(it != sentence.end()){
		if (*it == '$'){
			//Ignore the dollar sign
			continue;
		}
		if (*it == '*'){
			//Stop processing before the asterisk
			break;
		}
		//Is this the first value for the checksum?
		else if (Checksum == 0){
			//Yes. Set the checksum to the value
			Checksum = *it;
		}
		else{

			//No. XOR the checksum with this character's value
			Checksum = Checksum ^ (*it);
		}
		++it;
	}

	std::stringstream sstream;
	sstream << std::hex << Checksum;
	std::string result = sstream.str();
	//printf("Checksum: %s\n", result.c_str());

	return result;

}

std::string ToBase64(unsigned char const* bytes_to_encode,
		unsigned int in_len) {
	std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4)
					+ ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2)
					+ ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;
			for (i = 0; (i < 4); i++) {
				ret += base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++) {
			char_array_3[j] = '\0';
		}
		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4)
				+ ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2)
				+ ((char_array_3[2] & 0xc0) >> 6);
		for (j = 0; (j < i + 1); j++) {
			ret += base64_chars[char_array_4[j]];
		}
		while ((i++ < 3)) {
			ret += '=';
		}
	}
	return ret;
}

void serialReadTread(){

}
