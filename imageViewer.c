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

int main(int argc, char **argv){
    FILE *fp = fopen("dots.bmp","rb");

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8_t *raw = malloc(sizeof(uint8_t) * size);

    fread(raw, 1, size, fp);

    //Header section
    uint16_t fileSig = bitpack16(raw, HEADERSIGOFFSET);
    uint32_t fileSize = size;
    uint32_t dataOffset = bitpack32(raw, DATAOFFSET);

    if(fileSig != BMPSIG){
        printf("This file does not contain a valid BMP sig\n");
        return 1;
    }

    //infoHeader section
    uint32_t infoHeaderSize = bitpack32(raw, SIZEOFFSET);

    if(infoHeaderSize != BITMAPINFOHEADERSIZE){
        printf("infoHeader size of %d not supported\n", infoHeaderSize);
    }

    uint32_t width = bitpack32(raw, WIDTHOFFSET);
    uint32_t height = bitpack32(raw, HEIGHTOFFSET);

    SDL_Window *window = initDisplay(width, height);
    SDL_Renderer *renderer = initRender(window);
    uint32_t *pixelData = malloc(sizeof(uint32_t) * width * sizeof(uint32_t) * height);
    
    for(int i = 0; i < width * height; i++) {
        pixelData[i] = 0x0000FFFF;
    }
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_UpdateTexture(texture, NULL, pixelData, width * sizeof(uint32_t));

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_Event event;
    while(1){
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT){
            break;
        }
        SDL_RenderPresent(renderer);
    }
    printf("%d\n", width);

    return 0;
}
