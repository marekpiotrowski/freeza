#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "instruction_parser.h"

#define EXIT "exit\n"
#define MAX_SIZE 1024

void set_com_full_name(int com_number, char * result) {
	sprintf(result, "COM%d", com_number);
}
 
int main()
{
	int com_port_number;
	char com[10];
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
	char bytes_to_send[100];
	int size;
	
	printf("Specify COM port of the machine please: COM");
	scanf("%d", &com_port_number);
	
	set_com_full_name(com_port_number, com);
         
    fprintf(stderr, "Opening %s...", com);
	
    hSerial = CreateFile(
                com, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				
    if (hSerial == INVALID_HANDLE_VALUE)
    {
            fprintf(stderr, "Error\n");
            return 1;
    }
    else fprintf(stderr, "OK\n");
    

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error getting device state\n");
        CloseHandle(hSerial);
        return 1;
    }
     
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
	
    if(SetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error setting device parameters\n");
        CloseHandle(hSerial);
        return 1;
    }
 
    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
	
    if(SetCommTimeouts(hSerial, &timeouts) == 0)
    {
        fprintf(stderr, "Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }
 
	printf("\n");
	
	fseek(stdin,0,SEEK_END);
	
	while(strcmp(bytes_to_send, EXIT) != 0) {
		printf(">");
		fgets(bytes_to_send, MAX_SIZE, stdin);
		short result = send_instruction(bytes_to_send, hSerial);
	}

    fprintf(stderr, "Closing serial port...");
    if (CloseHandle(hSerial) == 0)
    {
        fprintf(stderr, "Error\n");
        return 1;
    }
    fprintf(stderr, "OK\n");
 
    return 0;
}