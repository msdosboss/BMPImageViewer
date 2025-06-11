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

int main(int argc, char **argv){
    FILE *fp = fopen("dots.bmp","rb");

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8_t *raw = malloc(sizeof(uint8_t) * size);

    fread(raw, 1, size, fp);

    //Header section
    fileSig = bitpack16(raw, HEADERSIGOFFSET);
    fileSize = size;
    dataOffset = bitpack32(raw, DATAOFFSET);

    if(fileSig != BMPSIG){
        printf("This file does not contain a valid BMP sig\n");
        return 1;
    }

    //infoHeader section
    infoHeaderSize = bitpack32(raw, SIZEOFFSET);

    if(infoHeaderSize != BITMAPINFOHEADERSIZE){
        printf("infoHeader size of %d not supported\n", infoHeaderSize);
    }

    bitsPerPixel = bitpack16(raw, BITSPERPIXELOFFSET);
    compression = bitpack32(raw, COMPRESSIONOFFSET);

    printf("%s: %d\n", "Compression type", compression);
    fflush(stdout);
    /* Initialize SDL renderer and display  */

    width = bitpack32(raw, WIDTHOFFSET);
    height = bitpack32(raw, HEIGHTOFFSET);

    SDL_Window *window = initDisplay(width, height);
    SDL_Renderer *renderer = initRender(window);
    uint32_t *pixelData = malloc(sizeof(uint32_t) * width * sizeof(uint32_t) * height);

    displayLoop(pixelData, window, renderer);

    /* End cleanup: print debug, free memory */

    printf("Window Width: %d\n", width);
    fflush(stdout);
    free(pixelData);
    return 0;
}

void displayLoop(uint32_t *pixelData, SDL_Window *window, SDL_Renderer *renderer){
    for(int i = 0; i < width * height; i++) {
        pixelData[i] = 0x0000FFFF;
    }
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