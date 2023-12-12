#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include "rc522.h"

int show_menu(){
	int menu_choice;
	
printf("\n###############################################\n");
printf("##################################################\n");
printf("(1) lue RFID\n");
printf("(2) kirjoita RFID\n");
printf("----------------------------------------- \n");
printf("(0) Ohjelman lopetus\n");
printf("----------------------------------------- \n");
printf("Valitse toiminto numerolla (0-2) :");

  scanf("%d", &menu_choice);
  return menu_choice;
}
//unsigned char SN[4]; //card number
void print_info(unsigned char *p,int cnt);
int read_card();
int card_passworld(uint8_t auth_mode,uint8_t addr,uint8_t *Src_Key,uint8_t *New_Key,uint8_t *pSnr);
uint8_t write_card_data(uint8_t *data);
uint8_t read_card_data();
void MFRC522_HAL_Delay(unsigned int ms);
/*

gcc -g -o rfid_ohjelma rfid_main.c rfids.c mfrc522_hal_linux.c rc522.c

RST -> 22

(SDA)NSS -> 24

MOSI -> 19

MISO -> 21

SCK -> 23

GND -> 25

VCC -> 17

IRQ -> tyhja

*/

int main() {

	uint8_t data[16]={0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,255,255,255,255};
	uint8_t status=1;
    InitRc522();		 //RC522
	memset(data,0,16);
	
        int valinta = 0;
        int rc = 1;
		
        while(valinta >= 0 && rc !=0){
			valinta=show_menu();
			switch (valinta) {
	
				case 1:     
					printf("Reading...Please place the card...\r\n");
					    while (1){
							status=read_card_data(data);
							if(status==MI_OK){
							break;
							}
						}
					break;
				case 2: 
					printf("Please enter user_id to be written:\r\n");
					scanf("%s", data);
					printf("Reading...Please place the card...\r\n");
					while (1){
							status=write_card_data(data);
							if(status==MI_OK){
							break;
							}
					}
					break;
				case 0:
					rc = 0;
					break;
				default:
					//printf("Out of range");
					break;
			}
			 if(rc==0)
			   valinta=-1;
		}//while

        return 0;
}

