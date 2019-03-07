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

std::string settingsfile = "Settings.txt";
bool UserCanNotEditSettings = false;
bool UserCanNotDisconnectStuff = false;
bool WriteEventsToFile = false;
bool WriteNMEAToFile = false;

bool SerialShouldBeConnected = false;
//SML HARD-CODED VALUES START
char SerialPort[] = "/dev/ttyUSB0";
//SML HARD-CODED VALUES END
int SerialSpeed = 9600;
int SerialDataBits = 8;
int SerialStopBits = 1;
int SerialParity = 0;
int SerialProtocol = 0;
std::string ReceiveBuffer;
//Dim WithEvents COMPort As New System.IO.Ports.SerialPort

int ReceiverType = 0; //0 is generic, 1 is NovAtel
int ReceiverMessageRate = 10; //Hz
std::string ReceiverCorrDataType = "RTCMV3";

bool NTRIPShouldBeConnected = false;
int NTRIPProtocol;
bool NTRIPUseManualGGA;
//Public Shared NTRIPResendGGAEvery10Seconds As Boolean = True
//static Decimal NTRIPManualLat;
//static Decimal NTRIPManualLon;
//SML HARD-CODED VALUES START
std::string NTRIPCaster = "gnssdata.or.kr";
int NTRIPPort = 2101;
std::string NTRIPUsername = "gnss";
std::string NTRIPPassword = "gnss";
std::string NTRIPMountPoint = "SOUL-RTCM32";
//SML HARD-CODED VALUES END
std::string PreferredMountPoint = "";
//Public NTRIPThread As Threading.Thread
bool NTRIPIsConnected = false;
int NTRIPConnectionAttempt = 1;
bool NTRIPStreamRequiresGGA = false;
int NTRIPByteCount = 0;
//Public NTRIPStreamArray(1, -1) As String
std::string MostRecentGGA = "$GPGGA,052158,4158.7333,N,09147.4277,W,2,08,3.1,260.4,M,-32.6,M,,*79";

int CheckForUpdateDays = 7;
//Dim LastCheckedForUpdates As Date = "1/1/2010"

int StartNTRIPThreadIn = 0;
std::string AudioFile = "";

//Dim lastLocation As Point = Location
//Dim lastSize As Size = Size

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

std::string ToBase64(unsigned char const* bytes_to_encode, unsigned int in_len) {
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
