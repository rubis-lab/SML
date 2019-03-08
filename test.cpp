#include "serial.c"
#include <string>
#include <cstring>

struct Args
{
  const char *server;
  const char *port;
  const char *user;
  const char *proxyhost;
  const char *proxyport;
  const char *password;
  const char *nmea;
  const char *data;
  int         bitrate;
  int         mode;

  int         udpport;
  int         initudp;
  enum SerialBaud baud;
  enum SerialDatabits databits;
  enum SerialStopbits stopbits;
  enum SerialParity parity;
  enum SerialProtocol protocol;
  const char *serdevice;
  const char *serlogfile;
};

int main(int argc, char **argv) {
	struct serial sx;
	struct Args args;
	static char buf[1000];

	args.serdevice = "/dev/ttyUSB0";
	args.baud = SPABAUD_9600;
	args.stopbits = SPASTOPBITS_1;
	args.protocol = SPAPROTOCOL_NONE;
	args.parity = SPAPARITY_NONE;
	args.databits = SPADATABITS_8;

    const char *e = SerialInit(&sx, args.serdevice, args.baud,
    		args.stopbits, args.protocol, args.parity, args.databits, 1);
	if(e)
	{
		fprintf(stderr, "%s\n", e);
		return 20;
	}

	int i = SerialRead(&sx, buf, 200);
	if(i < 0){
		fprintf(stderr, "Could not access serial device\n");
	}
	fprintf(stdout, buf, 200);
	int iteration = 0;
	while(iteration <100){
		std::string data = "TEST";
		char * msg = new char[data.length()+1];
		std::strcpy(msg, data.c_str());
		int j = SerialWrite(&sx, msg, strlen(msg));
		if(j < 0){
			fprintf(stderr, "Could not access serial device\n");
		}
		iteration ++;
	}
	return 0;
}
