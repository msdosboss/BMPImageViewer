//https://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
//Format description

#define BITMAPINFOHEADERSIZE 40
#define BMPSIG 0x4d42

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(){
    FILE *fp = fopen("dots.bmp","rb");

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unsigned char *raw = malloc(sizeof(unsigned char) * size);

    fread(raw, 1, size, fp);

    //Header section
    uint16_t fileSig = raw[0x0] | raw[0x1] << 8;
    uint32_t fileSize = size;
    uint32_t dataOffset = raw[0x10] | raw[0x11] << 8 | raw[0x12] << 16 | raw[0x13] << 24;

    if(fileSig != BMPSIG){
        printf("This file does not contain a valid BMP sig\n");
        return 1;
    }

    //infoHeader section
    uint32_t infoHeaderSize = raw[0xe] | raw[0xf] << 8 | raw[0x10] << 16 | raw[0x11] << 24;

    if(infoHeaderSize != BITMAPINFOHEADERSIZE){
        printf("infoHeader size of %d not supported\n", infoHeaderSize);
    }

    return 0;
}
