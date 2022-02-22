#include "bmp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 


// calculate the number of bytes of memory needed to serialize the bitmap
// that is, to write a valid bmp file to memory
size_t bmp_calculate_size(bitmap_t *bmp) {

    // Compute the total size in bytes of the two headers and all the BGR pixel data
    return ( bmp->width * bmp->height * 3 ) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); 
}

// write the bmp file to memory at data, which must be at least
// bmp_calculate_size large.
void bmp_serialize(bitmap_t *bmp, uint8_t *data) {


    BITMAPFILEHEADER file_header = {0};
    file_header.bfType = 0x4d42; // 0x424d or 0x4d42 for 'B' (0x42) and 'M' (0x4d)
    file_header.bfSize = ( bmp->width * bmp->height * 3 ) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // the size, in bytes, of the bitmap file
    file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // combined size of both headers

    BITMAPINFOHEADER info_header = {0};
    info_header.biSize = sizeof(BITMAPINFOHEADER); // specifies the number of bytes required by the structure
    info_header.biWidth = bmp->width;
    info_header.biHeight = bmp->height;
    info_header.biPlanes = 1;
    info_header.biBitCount = 24; // 1 byte for ea RGB ( 3 bytes for ea pixel )
    info_header.biCompression = 0; // uncompressed
    info_header.biSizeImage =  0; // for uncompressed images
    info_header.biXPelsPerMeter = 2835;
    info_header.biYPelsPerMeter = 2835;
    info_header.biClrUsed = 0;
    info_header.biClrImportant = 0;

    // keep track of the next place we should write in the data buffer
    uint8_t *data_out = data;

    memcpy(data_out, &file_header, sizeof(file_header)); // write X number of bytes
    data_out += sizeof(file_header); // and then move data_out forward X bytes

    memcpy(data_out, &info_header, sizeof(info_header));
    data_out += sizeof(info_header);

    for ( int i = bmp->height - 1; i >= 0; i-- ) {
        memcpy( data_out, &bmp->data[ bmp->width * i ], ( bmp->width * 3) );
        data_out += ( bmp->width * 3 );
    } 

}

/* int main(void) {
     bitmap_t bmp = { 0 };

    // set bmp width and height
    bmp.width = 64;
    bmp.height = 48;

    // allocate bytes for bmp.data
    bmp.data = malloc( sizeof(color_bgr_t) * bmp.width * bmp.height * 3 );

    // set pixels in bmp.data - each pixel should be black ( value 0 )
    for ( int i = 0; i < ( bmp.width * bmp.height ); i++ ) {
        bmp.data[i].r = 0;
        bmp.data[i].g = 0;
        bmp.data[i].b = 0;
    }
 
    size_t bmp_size = bmp_calculate_size(&bmp);
    uint8_t *serialized_bmp = malloc(bmp_size);
    bmp_serialize(&bmp, serialized_bmp);
    // serialized_bmp now has the full bmp formatted image
 
    image_server_set_data(bmp_size, serialized_bmp);
    image_server_start("8000");

    sleep(1); 

 
    // write to a file so we can check if it is a valid image
    // note we ONLY do this for debugging this test case 0
    // do not keep this for code you submit
    FILE *f = fopen("my_image.bmp", "wb");
    fwrite(serialized_bmp, bmp_size, 1, f);
    fclose(f);  

     // debugging
     bmp_serialize(&bmp, serialized_bmp);
    for (int i = 0; i < 150; i++) {
        // %x is for hexadecimal
        // 02 tells it to print the leading zeros (0) of each byte (2 characters)
        // so "0e" inxxd -l 16 all_black_img.bmpstead of just "e"
        printf("%02x ", serialized_bmp[i]);
    }
    printf("\n");  
    
    //compare to actual 
    //xxd -l 16 all_black_img.bmp

    // free stuff we malloced
    free( serialized_bmp );
    serialized_bmp = NULL;
    free( bmp.data );
    bmp.data = NULL; 
    


    return 0;
}  
 */