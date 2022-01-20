#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <sys/types.h>


int main(int argc, char** argv)
{
	int sockid;
	struct sockaddr_in servaddr, cli;
	sockid = socket(AF_INET, SOCK_STREAM, 0);
	if (sockid == -1)
	{
		printf("socket creation failed\n");
		exit(0);
	}
	else
	{
		printf("socket creation successful\n");
	}
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(50000);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int bind_val = bind(sockid, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if (bind_val != 0)
	{
		printf("socket bind failed\n");
		exit(0);
	}
	else
	{
		printf("socket bind successful\n");
	}
    int status = listen(sockid, 5);
	if (status != 0)
	{
		printf("listening failed\n");
		exit(0);
	}
	else
	{
		printf("listening successful\n");
	}

	char filename[] = "data-acquisition-task/";

	if (argc > 1)
	{
		strcat(filename,argv[1]);
	}

	FILE* fptr = fopen(filename, "rb");

    if (fptr == NULL)
    {
    	printf("failed to open file\n");	
    	exit(0);
    }

    
    char buffheader[16];
    char bufferevent[4];
    char XYbytes[2];
    short XY;
    char bitmask = 127;
    char bitmask2 = 63;
    char bitone = 1;
    char X, Y, newX, newY;
    uint32_t pktype, length, eventbytes = 0, readbytes, numevents;

    int len = sizeof(cli);
	int s = accept(sockid, (struct sockaddr *) &cli, &len);

	while (s < 0)
	{

		printf("accept failed\n");
		s = accept(sockid, (struct sockaddr *) &cli, &len);

	}
	
	printf("server accepts the client\n");
	
    const int packetlen = 255 + 7;  //START: numeventsXYXYXY...

	char buff[packetlen];

	buff[0] = 'S';
	buff[1] = 'T';
	buff[2] = 'A';
	buff[3] = 'R';
	buff[4] = 'T';
	buff[5] = ':';
	buff[6] = ' ';

	for (int i = 0; i < packetlen; i++)
	{
		buff[i+7] = 0;
	}
    
    int j = 0; 

    while (feof(fptr) == 0)
    {
    readbytes = fread(&buffheader, 16, 1, fptr);

    if (readbytes < 1 && feof(fptr))
    {
    	printf("end of file reached\n");
    	exit(0);
    }
    else if (readbytes == 1)
    {
    	length = buffheader[3] << 24 | buffheader[2] << 16 | buffheader[1] << 8 | buffheader[0];
    	pktype = buffheader[7] << 24 | buffheader[6] << 16 | buffheader[5] << 8 | buffheader[4];
    	eventbytes = length - 16;
        numevents = eventbytes/4;
        char* eventptr = (char *) malloc(eventbytes);
        if ((pktype != 4096 && eventbytes > 0) || (eventptr == NULL && eventbytes > 0))
    	{
    		fseek(fptr, eventbytes, SEEK_CUR);
    	}
        if (eventptr != NULL && pktype == 4096 && eventbytes > 0)
    	{
    		readbytes = fread(&eventptr[0], 1, eventbytes, fptr);
    		if (readbytes > 0)
    		{
    			int eventstosend = packetlen/2 < numevents ? packetlen/2 : numevents;
    			buff[7 + 0] =  (char) eventstosend;
    			printf("\nj: %d\n", j);
    			printf("eventbytes: %d, numevents: %d, eventstosend: %d\n", eventbytes, numevents, eventstosend);
    			for (int i = 0; i < eventstosend; i++)
    			{
    				XYbytes[0] = eventptr[i*4 + 2];
    				XYbytes[1] = eventptr[i*4 + 3]; 
    				X = (char) ((XYbytes[1] >> 1) & bitmask);
    				Y = (char) (((XYbytes[0] >> 2) & bitmask2) | ((XYbytes[1] & bitone) << 6));
    				buff[7 + 1 + 2*i] = X;
    				buff[7 + 2*i + 2] = Y; 
    			}
    			printf ("data being sent \n");
    			int count = send(s, buff, sizeof(buff), 0);
    			j += 1;
    			printf("bytes sent: %d\n", count);
    		}
    		else if (readbytes <= 0 && feof(fptr))
    		{
    			printf("end of file reached\n");
    			exit(0);
    		}
    		if (readbytes < numevents * 4 && feof(fptr))
    		{
    			printf("end of file reached\n");
    			exit(0);
    		}
    	}
    }

    }

	close(sockid);	
}