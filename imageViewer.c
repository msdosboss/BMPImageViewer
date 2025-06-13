//https://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
//Format description

#define BITMAPINFOHEADERSIZE 40
#define BMPSIG 0x4d42

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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
uint32_t width;
uint32_t height;
uint32_t imageSize;
uint32_t amountColorsUsed;

int main(int argc, char **argv){
    FILE *fp = fopen("BW_1015.bmp","rb");

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

    printf("bitsPerPixel %d\n", bitsPerPixel);
    printf("imageSize %d\n", imageSize);
    printf("imageHeight %d\n", height);
    printf("width * hieght / 8 = %d\n", width * height / 8);

    int rowSize = width * bitsPerPixel / 8;
    //got this formula for paddedRowSize from wikipedia
    int paddedRowSize = (bitsPerPixel * width + 31) / 32 * 4;
    printf("rowSize %d\n", rowSize);

    switch(bitsPerPixel){
        case 1:{
            printf("case 1 reached\n");
            uint32_t colorTable[2];
            /*there will have to be a custom bit packer for the colorTable since the data is stored diffently*/
            colorTable[1] = colorTableBitpack(raw, COLORTABLEOFFSET);
            colorTable[0] = colorTableBitpack(raw, COLORTABLEOFFSET + sizeof(uint32_t));

            printf("colorTable[0]: %x\ncolorTable[1]: %x\n", colorTable[0], colorTable[1]);

            int ii = 0;
            for(int i = 0; i < imageSize; i++){
                if(i % paddedRowSize >= rowSize){
                    continue;
                }
                for(int j = 0; j < 8; j++){
                    pixelData[ii * 8 + j] = colorTable[(data[i] >> (7 - j)) & 0b1];
                }
                ii++;
            }
            break;
        }
        case 4:
            break;

        case 8:
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
