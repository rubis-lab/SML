#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

//SML HARD-CODED VALUES START
char SerialPort[] = "/dev/ttyUSB0";
//SML HARD-CODED VALUES END
int SerialSpeed = 9600;
int SerialDataBits = 8;
int SerialStopBits = 1;
int SerialParity = 0;
int SerialProtocol = 0;
std::string ReceiveBuffer;

//SML HARD-CODED VALUES START
std::string NTRIPCaster = "gnssdata.or.kr";
int NTRIPPort = 2101;
std::string NTRIPUsername = "gnss";
std::string NTRIPPassword = "gnss";
std::string NTRIPMountPoint = "SOUL-RTCM32";
//SML HARD-CODED VALUES END
std::string MostRecentGGA = "$GPGGA,052158,4158.7333,N,09147.4277,W,2,08,3.1,260.4,M,-32.6,M,,*79";

void SerialInput();
void ProcessNMEAdata(std::string &x);
std::string CalculateChecksum(std::string &sentence);
std::string ToBase64(unsigned char const* bytes_to_encode, unsigned int in_len);

int main(int argc, char *argv[]) {
	struct termios Termios;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int lcount = 97;
	int Stream=0;

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
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
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
		std::string auth_encoded = ToBase64(reinterpret_cast<const unsigned char*>(auth.c_str()), auth.length());
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
			if (strstr(responseData.c_str(), "401 Unauthorized")){
				perror("Invalid user name or password");
				close(sockfd);
				exit(0);
			}
		} else if (DataLength == 0){
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
	if ((Stream = open(SerialPort, O_RDWR | O_NOCTTY | O_NONBLOCK)) <= 0) {
		perror("ERROR opening serial port");
		exit(0);
	}

	tcgetattr(Stream, &Termios);

	memset(&Termios, 0, sizeof(struct termios));
	Termios.c_cflag = SerialSpeed | SerialStopBits | SerialParity
			| SerialDataBits | CLOCAL | CREAD;

	Termios.c_cflag |= SerialProtocol;
	Termios.c_cc[VMIN] = 1;
	tcflush(Stream, TCIOFLUSH);
	tcsetattr(Stream, TCSANOW, &Termios);
	tcflush(Stream, TCIOFLUSH);
	fcntl(Stream, F_SETFL, O_NONBLOCK);

	//ICY 200 OK, Waiting for data
	std::string serialData;
	DataNotReceivedFor = 0;
	bool KeepRunning = true;
	while (KeepRunning) {
		char InBytes[1000];
		int DataLength = read(sockfd, InBytes, strlen(InBytes));
		if (DataLength > 0) {
			DataNotReceivedFor = 0;
			/*
			serialData.append(InBytes);
			char *MyBytes = new char[serialData.length() + 1];
			std::strcpy(MyBytes, serialData.c_str());
			printf("%s\n", serialData.c_str());
			*/
			//SerialWrite(MyBytes);
			////////////////////////////         SerialWrite(InBytes) Start     ///////////////////////////////
			std::string serialData = InBytes;
			char * MyBytes = new char[serialData.length() + 1];
			std::strcpy(MyBytes, serialData.c_str());
			printf("%s\n", MyBytes);
			int j = write(Stream, MyBytes, strlen(MyBytes));
			if (j < 0) {
				fprintf(stderr, "Could not access serial device\n");
			}
			///////////////////////////          Serial Write End               //////////////////////////////////
		} else if (DataLength == 0) {
			DataNotReceivedFor += 1;
			if (DataNotReceivedFor > 300) {
				//Data not received for 30 seconds. Terminate the connection.
				KeepRunning = false;
				perror("Waiting for data - Connection timed out");
				close(sockfd);
				exit(0);
			}
		} else {
			perror("ERROR reading from socket");
			exit(0);
		}

		lcount += 1;
		if (lcount == 100) {
			std::string TheGGA = MostRecentGGA; //From GPS::ProcessNMEAdata()
			TheGGA += "\r\n";
			char *nmeadata = new char[TheGGA.length() + 1];
			std::strcpy(nmeadata, TheGGA.c_str());
			if (write(sockfd, nmeadata, strlen(nmeadata)) < 0) {
				perror("ERROR writing to socket");
				exit(0);
			}
			lcount = 0;
		}
	}

	close(sockfd);
	return 0;
}

void SerialInput(){
	//ReceiveBuffer += SerialRead();
	if(ReceiveBuffer.find("\r\n")){
		//Contains at least one carridge return
		char *lines = strtok(reinterpret_cast<char*>(ReceiveBuffer.c_str()),"\r\n");
		while(lines != NULL){
			if(strlen(lines)>5){
				printf("lines: %s\n", lines);
				//TODO: IMPLEMENT TRIM (if needed)
			}
			lines = strtok(NULL,"\r\n");
		}
		ReceiveBuffer = lines;
	}
	else{
		//Data doesn't contain any line breaks
		if(ReceiveBuffer.length()>4000){
			ReceiveBuffer = "";
			//No line breaks found in data stream
		}
	}
}

void ProcessNMEAdata(std::string &x) {
	int charlocation = (int) x.rfind("$"); // Find location of last $
	if (charlocation == -1 || charlocation + 1 > x.length() - 5) {
		//no $ found or not enough data left
		return;
	}

	//drop characters before the $
	//TODO: x = Mid(x, charlocation + 1)

	charlocation = (int) x.find("*"); //Find location of first *
	if (charlocation == -1) {
		//no * found
		return;
	}
	if (x.length() < charlocation + 3) {
		//there aren't 2 characters after the *
		return;
	} else if (x.length() > charlocation + 3) { //there is extra data after the *
		//remove the extra data after 2 chars after the *
		//TODO: x = Mid(x, 1, charlocation + 3)
		return;
	}
	if (x.length() < 8) {
		//not enough data left
		return;
	}
	char *aryNMEALine = strtok(reinterpret_cast<char*>(x.c_str()),"*");
	//lets see if the checksum matches the stuff before the astrix

	if (CalculateChecksum(replace(aryNMEALine[0], "$", "")) == aryNMEALine[1]) {
		//Checksum matches, send it to the stuff before the astrix
		if (aryNMEALine[0].substr(0, 6) == "$GPGGA" || aryNMEALine[0].substr(0, 6) == "$GNGGA") {
			MostRecentGGA = x;
			printf("MostRecentGGA: %s\n", x);
		}
	}
}
