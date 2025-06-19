//https://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
//Format description

#define BITMAPINFOHEADERSIZE 40
#define BMPSIG 0x4d42

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "display.h"
#include "define.h"
#include "bitpack.h"

void displayLoop(uint32_t *pixelData, SDL_Window *window, SDL_Renderer *renderer);

uint16_t fileSig;
uint32_t fileSize;
uint32_t dataOffset;
uint32_t infoHeaderSize;
uint16_t bitsPerPixel;
uint32_t compression;
uint32_t width; //the number of pixels in a row
uint32_t height; //the number of pixels in a column
uint32_t imageSize;
uint32_t amountColorsUsed;

int main(int argc, char **argv){
    FILE *fp;
    if(argc > 1) {
        fp = fopen(argv[1],"rb");
    }
    else {
        fp = fopen("4bit.bmp","rb");
    }
    if(fp == NULL) return -69;

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8_t *raw = malloc(sizeof(uint8_t) * size);

    fread(raw, 1, size, fp);

    //Header initializations
    fileSig = bitpack16(raw, HEADERSIGOFFSET);
    fileSize = size;
    dataOffset = bitpack32(raw, DATAOFFSET);
    infoHeaderSize = bitpack32(raw, SIZEOFFSET);
    bitsPerPixel = bitpack16(raw, BITSPERPIXELOFFSET);
    compression = bitpack32(raw, COMPRESSIONOFFSET);
    width = bitpack32(raw, WIDTHOFFSET);
    height = bitpack32(raw, HEIGHTOFFSET);
    imageSize = bitpack32(raw, IMAGESIZEOFFSET);


    //header value validity verification
    if(fileSig != BMPSIG){
        printf("This file does not contain a valid BMP sig\n");
        return 1;
    }

    if(infoHeaderSize != BITMAPINFOHEADERSIZE){
        printf("infoHeader size of %d not supported\n", infoHeaderSize);
    }

    printf("%s: %d\n", "Compression type", compression);
    fflush(stdout);
    if(compression){
        printf("Compressed images are currently unsupported\n");
        return -1;
    }


    /* Initialize SDL renderer and display  */
    SDL_Window *window = initDisplay(width, height);
    SDL_Renderer *renderer = initRender(window);
    uint32_t *pixelData = malloc(width * sizeof(uint32_t) * height);

    amountColorsUsed = bitpack32(raw, COLORUSEDOFFSET);

    uint8_t *data = &raw[dataOffset];

    #if defined DEBUG
    printf("bitsPerPixel %d\n", bitsPerPixel);
    printf("imageSize %d\n", imageSize);
    printf("imageHeight %d\n", height);
    printf("width * height / 8 = %d\n", width * height / 8);
    #endif

    int rowSize = width * bitsPerPixel / 8; //unit:bytes
    //got this formula for paddedRowSize from wikipedia
    int paddedRowSize = (bitsPerPixel * width + 31) / 32 * 4; //unit:bytes
    printf("rowSize %d\n", rowSize);

    int pixelsPerByte = 8 / bitsPerPixel;
    int colorTableSize = pow(2.0f, (double)bitsPerPixel); 

    uint32_t colorTable[colorTableSize];

    switch(bitsPerPixel){
        case 1:{
            /*there will have to be a custom bit packer for the colorTable since the data is stored diffently*/
            colorTable[0] = colorTableBitpack(raw, COLORTABLEOFFSET);
            colorTable[1] = colorTableBitpack(raw, COLORTABLEOFFSET + sizeof(uint32_t));

            printf("colorTable[0]: %x\ncolorTable[1]: %x\n", colorTable[0], colorTable[1]);
            printf("height: %d, width: %d, paddedRowSize: %d, rowSize: %d", height, width, paddedRowSize, rowSize);
            for(int hIndex = height - 1 ; hIndex >= 0; hIndex--){
                for(int wIndex = 0; wIndex < paddedRowSize * pixelsPerByte; wIndex++){
                    if(wIndex / pixelsPerByte >= rowSize){//skip  the padded data (hopefully)
                        continue;
                    }
                    pixelData[hIndex * width + wIndex] = 
                    colorTable[(data[((height - hIndex) * (paddedRowSize * pixelsPerByte) + wIndex) / pixelsPerByte] >> (7 - wIndex % pixelsPerByte)) & 0b1];
                }
            }
            break;
        }
        case 4:
            for(int colorTableIndex = 0; colorTableIndex < colorTableSize; colorTableIndex++){
                colorTable[colorTableIndex] = colorTableBitpack(raw, COLORTABLEOFFSET + colorTableIndex * sizeof(uint32_t));
            }
            for(int hIndex = height - 1 ; hIndex >= 0; hIndex--){
                for(int wIndex = 0; wIndex < paddedRowSize * pixelsPerByte; wIndex++){
                    if(wIndex / pixelsPerByte >= rowSize){//skip  the padded data (hopefully)
                        continue;
                    }
                    pixelData[hIndex * width + wIndex] = 
                    colorTable[(data[((height - hIndex) * (paddedRowSize * pixelsPerByte) + wIndex) / pixelsPerByte] >> ((wIndex % pixelsPerByte) * 4)) & 0b1111];
                }
            }            
            break;

        case 8:
            for(int colorTableIndex = 0; colorTableIndex < colorTableSize; colorTableIndex++){
                colorTable[colorTableIndex] = colorTableBitpack(raw, COLORTABLEOFFSET + colorTableIndex * sizeof(uint32_t));
            }
            for(int hIndex = height - 1 ; hIndex >= 0; hIndex--){
                for(int wIndex = 0; wIndex < paddedRowSize * pixelsPerByte; wIndex++){
                    if(wIndex / pixelsPerByte >= rowSize){//skip  the padded data (hopefully)
                        continue;
                    }
                    pixelData[hIndex * width + wIndex] = 
                    colorTable[data[((height - hIndex) * (paddedRowSize * pixelsPerByte) + wIndex)]];
                }
            }
            break;

        case 16:
            
            break;

        case 24:
            break;
    }

    displayLoop(pixelData, window, renderer);

    /* End cleanup: print debug, free memory */

    printf("Window Width: %d\n", width);
    fflush(stdout);
    free(pixelData);
    return 0;
}

void displayLoop(uint32_t *pixelData, SDL_Window *window, SDL_Renderer *renderer){
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_UpdateTexture(texture, NULL, pixelData, width * sizeof(uint32_t));

    SDL_Event event;
    while(1){
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT){
            break;
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
}

