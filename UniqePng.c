#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>


#define PNG_DEBUG 3
#include <png.h>

struct Png{
    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep *row_pointers;
};

struct rgba{
    int r;
    int g;
    int b;
    int a;
};

int read_png_file(char *file_name, struct Png *image) {
    int x,y;
    char header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp){
        printf("File could not be opened\n");
        return 1;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)){
        printf("File is not recognized as a PNG\n");
        return 1;
    }

    /* initialize stuff */
    image->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!image->png_ptr){
        printf("Png_create_read_struct failed\n");
        return 1;
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);

    if (!image->info_ptr){
        printf("Png_create_info_struct failed\n");
        return 1;
    }

    if (setjmp(png_jmpbuf(image->png_ptr))){
        printf("Error during init_io\n");
        return 1;
    }

    png_init_io(image->png_ptr, fp);
    png_set_sig_bytes(image->png_ptr, 8);

    png_read_info(image->png_ptr, image->info_ptr);

    image->width = png_get_image_width(image->png_ptr, image->info_ptr);
    image->height = png_get_image_height(image->png_ptr, image->info_ptr);
    image->color_type = png_get_color_type(image->png_ptr, image->info_ptr);
    image->bit_depth = png_get_bit_depth(image->png_ptr, image->info_ptr);

    image->number_of_passes = png_set_interlace_handling(image->png_ptr);
    png_read_update_info(image->png_ptr, image->info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        printf("Error during read_image\n");
        return 1;
    }

    image->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * image->height);
    for (y = 0; y < image->height; y++)
        image->row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));

    png_read_image(image->png_ptr, image->row_pointers);

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB){
        printf("Input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA\n");
        return 1;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA){
        printf("Color_type of input file must be PNG_COLOR_TYPE_RGBA\n");
        return 1;
    }

    fclose(fp);
    return 0;
}


void write_png_file(char *file_name, struct Png *image) {
    int x,y;
    /* create file */
    FILE *fp = fopen(file_name, "wb");
    if (!fp){
        printf("File could not be opened\n");
        return;
    }

    /* initialize stuff */
    image->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!image->png_ptr){
        printf("Png_create_write_struct failed\n");
        return;
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr){
        printf("Png_create_info_struct failed\n");
        return;
    }

    if (setjmp(png_jmpbuf(image->png_ptr))){
        printf("Error during init_io\n");
        return;
    }

    png_init_io(image->png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        printf("Error during writing header\n");
        return;
    }

    png_set_IHDR(image->png_ptr, image->info_ptr, image->width, image->height,
                 image->bit_depth, image->color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(image->png_ptr, image->info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        printf("Error during writing bytes\n");
        return;
    }

    png_write_image(image->png_ptr, image->row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        printf("Error during end of write\n");
        return;
    }

    png_write_end(image->png_ptr, NULL);

    /* cleanup heap allocation */
    for (y = 0; y < image->height; y++)
        free(image->row_pointers[y]);
    free(image->row_pointers);

    fclose(fp);
}

void write_png_file_copy(char *file_name, struct Png *image) {
    int x,y;
    /* create file */
    FILE *fp = fopen(file_name, "wb");
    if (!fp){
        printf("File could not be opened\n");
        return;
    }

    /* initialize stuff */
    image->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!image->png_ptr){
        printf("Png_create_write_struct failed\n");
        return;
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr){
        printf("Png_create_info_struct failed\n");
        return;
    }

    if (setjmp(png_jmpbuf(image->png_ptr))){
        printf("Error during init_io\n");
        return;
    }

    png_init_io(image->png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        printf("Error during writing header\n");
        return;
    }

    png_set_IHDR(image->png_ptr, image->info_ptr, image->width, image->height,
                 image->bit_depth, image->color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(image->png_ptr, image->info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        printf("Error during writing bytes\n");
        return;
    }

    png_write_image(image->png_ptr, image->row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        printf("Error during end of write\n");
        return;
    }
    png_write_end(image->png_ptr, NULL);

    fclose(fp);
}

void inter(struct Png *image, int x1, int y1, int x2, int y2, struct rgba Color){
    int x,y;
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB){
        // Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA){
        // Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
    }
    for (y = 0; y < image->height; y++) {
        png_byte *row = image->row_pointers[y];
        for (x = 0; x < image->width; x++) {
            png_byte *ptr = &(row[x * 4]);
            if((x>=x1)&&(x<=x2)&&(y>=y1)&&(y<=y2)){
                ptr[0] = Color.r;
                ptr[1] = Color.g;
                ptr[2] = Color.b;
            }
        }
    }
}
void drawRectangle(struct Png *image,int x1,int y1, int x2, int y2, int w, struct rgba lnColor, int interior, struct rgba inColor) {
    int x,y;
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB){
        // Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA){
        // Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
    }

    for (y = 0; y < image->height; y++) {
        png_byte *row = image->row_pointers[y];
        for (x = 0; x < image->width; x++) {
            png_byte *ptr = &(row[x * 4]);
            if((y >= y1 - w)&&(y <= y1 + w)&&(x <= x2 + w)&&(x >= x1 - w)){   //верхняя
                ptr[0] = lnColor.r;
                ptr[1] = lnColor.g;
                ptr[2] = lnColor.b;
            }
            if((y >= y2 - w)&&(y <= y2 + w)&&(x<=x2+w)&&(x>=x1-w)){   //нижняя
                ptr[0] = lnColor.r;
                ptr[1] = lnColor.g;
                ptr[2] = lnColor.b;
            }
            if((x >= x1 - w)&&(x <= x1 + w)&&(y<=y2)&&(y>=y1)){   //левая
                ptr[0] = lnColor.r;
                ptr[1] = lnColor.g;
                ptr[2] = lnColor.b;
            }
            if((x >= x2 - w)&&(x <= x2 + w)&&(y<=y2)&&(y>=y1)){   //правая
                ptr[0] = lnColor.r;
                ptr[1] = lnColor.g;
                ptr[2] = lnColor.b;
            }

        }
    }
    if(interior == 1){
        inter(image, x1+w, y1+w, x2-w, y2-w, inColor);
    }
}
void clear_file(struct Png *image) {
    int x,y;
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB){
        // Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA){
        // Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
    }

    int w = 5;
    for (y = 0; y < image->height; y++) {
        png_byte *row = image->row_pointers[y];
        for (x = 0; x < image->width; x++) {
            png_byte *ptr = &(row[x * 4]);
            ptr[0] = 255;
            ptr[1] = 255;
            ptr[2] = 255;
            ptr[3] = 255;
        }
    }
}


void drawLine(struct Png *image, int x1, int y1, int x2, int y2, int w, struct rgba color) {
    int x,y;
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB){
        // Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA
    }
    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA){
        // Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
    }

    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;
    int tg;
    if(deltaY/deltaX<=1) tg = 0; else tg = 1;
    int error = deltaX - deltaY;

    for(int i = 0; i < w; i ++){
        if(tg){
            png_byte* ptr = &(image->row_pointers[y2][(x2+i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y2][(x2-i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
        }
        else{
            png_byte* ptr = &(image->row_pointers[y2+i][(x2) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y2-i][(x2) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
        }
    }
    while(x1 != x2 || y1 != y2)
    {
        for(int i = 0; i < w; i ++) {
            if(tg){png_byte* ptr = &(image->row_pointers[y1][(x1+i) * 4]);
                ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
                ptr = &(image->row_pointers[y1][(x1-i) * 4]);
                ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            }
            else{
                png_byte* ptr = &(image->row_pointers[y1+i][(x1) * 4]);
                ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
                ptr = &(image->row_pointers[y1-i][(x1) * 4]);
                ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            }
        }
        int error2 = error * 2;
        if(error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }
        if(error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
    }
}


void drawCircle(struct Png* image, int x, int y, int r, int w, struct rgba color)
{
    int x1,y1,yk = 0;                       //проверить: 1. радиус больше толщины
    int sigma,delta,f;
    png_byte *ptr;

    x1 = 0;
    y1 = r;
    delta = 2*(1-r);

    do
    {
        for(int i = 0; i < w; i++){
            ptr = &(image->row_pointers[y+y1][(x+x1+i) * 4]);  //4 четверть
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y+y1][(x+x1-i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y+y1+i][(x+x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y+y1-i][(x+x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;

            ptr = &(image->row_pointers[y+y1][(x-x1+i) * 4]);  //3 четверть
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y+y1][(x-x1-i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y+y1+i][(x-x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y+y1-i][(x-x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;

            ptr = &(image->row_pointers[y-y1][(x+x1+i) * 4]);  //1 четверть
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y-y1][(x+x1-i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y-y1+i][(x+x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y-y1-i][(x+x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;

            ptr = &(image->row_pointers[y-y1][(x-x1+i) * 4]);  //2 четверть
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y-y1][(x-x1-i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y-y1+i][(x-x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;
            ptr = &(image->row_pointers[y-y1-i][(x-x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;

        }

        f = 0;
        if (y1 < yk)
            break;
        if (delta < 0)
        {
            sigma = 2*(delta+y1)-1;
            if (sigma <= 0)
            {
                x1++;
                delta += 2*x1+1;
                f = 1;
            }
        }
        else
        if (delta > 0)
        {
            sigma = 2*(delta-x1)-1;
            if (sigma > 0)
            {
                y1--;
                delta += 1-2*y1;
                f = 1;
            }
        }
        if(!f)
        {
            x1++;
            y1--;
            delta += 2*(x1-y1-1);
        }
    }
    while(1);
}

void pentax(struct Png* image, int x, int y, int r, int w, struct rgba color){
    drawCircle(image, x, y, r, w, color);

    int y1 = y - (int)(sin(72)*r);
    int y4 = y1;
    int x1 = x + (int)(cos(72)*r);
    int x4 = x - (int)(cos(72)*r);

    int y2 = y - (int)(sin(18)*r);
    int y3 = y2;
    int x2 = x - (int)(cos(18)*r);
    int x3 = x + (int)(cos(18)*r);

    int x0 = x;
    int y0 = y-r;

    drawLine(image, x2, y2, x0, y0, w, color);
    drawLine(image, x3, y3, x0, y0, w, color);
    drawLine(image, x1, y1, x3, y3, w, color);
    drawLine(image, x1, y1, x4, y4, w, color);
    drawLine(image, x2, y2, x4, y4, w, color);

}

void flood(struct Png* image, int x, int y, struct rgba newColor, struct rgba outColor){
    png_byte* ptr = &(image->row_pointers[y][(x) * 4]);

    if( (ptr[0]!=outColor.r)||(ptr[1]!=outColor.g)||(ptr[2]!=outColor.b)||(ptr[3]!=outColor.a)){

        ptr[0]=newColor.r;ptr[1]=newColor.g;ptr[2]=newColor.b;ptr[3]=newColor.a;

        ptr = &(image->row_pointers[y][(x+1) * 4]);
        if((ptr[0]!=newColor.r)||(ptr[1]!=newColor.g)||(ptr[2]!=newColor.b)||(ptr[3]!=newColor.a)) flood(image, x+1, y, newColor, outColor);

        ptr = &(image->row_pointers[y][(x-1) * 4]);
        if((ptr[0]!=newColor.r)||(ptr[1]!=newColor.g)||(ptr[2]!=newColor.b)||(ptr[3]!=newColor.a)) flood(image, x-1, y, newColor, outColor);

        ptr = &(image->row_pointers[y+1][(x) * 4]);
        if((ptr[0]!=newColor.r)||(ptr[1]!=newColor.g)||(ptr[2]!=newColor.b)||(ptr[3]!=newColor.a)) flood(image, x, y+1, newColor, outColor);

        ptr = &(image->row_pointers[y-1][(x) * 4]);
        if((ptr[0]!=newColor.r)||(ptr[1]!=newColor.g)||(ptr[2]!=newColor.b)||(ptr[3]!=newColor.a)) flood(image, x, y-1, newColor, outColor);

    }
}

void hexagon(struct Png* image, int x, int y, int r, int w, struct rgba outColor, int interior, struct rgba inColor){

    int x1 = x - r;
    int y1 = y;

    int x2 = x+r;
    int y2 = y;

    int x3 = x + (int)(cos(60*M_PI/180)*r);
    int y3 = y + (int)(sin(60*M_PI/180)*r);

    int x4 = x + (x-x3);
    int y4 = y3;

    int x5 = x + (int)(cos(60*M_PI/180)*r);
    int y5 = y - (int)(sin(60*M_PI/180)*r);

    int x6 = x + (x-x5);
    int y6 = y5;
//


    drawLine(image, x1, y1, x6, y6, w, outColor);  //blue
    drawLine(image, x1, y1, x4, y4, w, outColor);  //blue
    drawLine(image, x3, y3, x4, y4, w, outColor);  //green
    drawLine(image, x5, y5, x6, y6, w, outColor);  //red
    drawLine(image, x2, y2, x5, y5, w, outColor);  //red
    drawLine(image, x2, y2, x3, y3, w, outColor);  //red
    if(interior == 1)
        flood(image, x, y, inColor, outColor);
}

void reflection(struct Png *image, int x1, int y1, int x2, int y2, int side){
    write_png_file_copy("NewTestTemporaryPngFile.png", image);
    struct Png image_copy;
    read_png_file("NewTestTemporaryPngFile.png", &image_copy);

    int diff_y = y2 - y1;
    int diff_x = x2 - x1;

    for(int y = 0; y <=y2-y1; y++){
        for(int x = 0; x <= x2 - x1; x++){
            png_byte* ptr = &(image->row_pointers[y+y1][(x+x1) * 4]);
            png_byte* ptr_copy = &(image_copy.row_pointers[y2-y][(x+x1) * 4]);
            ptr[0] = ptr_copy[0];ptr[1] = ptr_copy[1];ptr[2] = ptr_copy[2];ptr[3] = ptr_copy[3];
        }
    }
    write_png_file("NewTestTemporaryPngFile.png", &image_copy);
}

int main(int argc, char **argv) {
    if (argc != 3){
        fprintf(stderr,"Usage: program_name <file_in> <file_out>\n");
        return 0;
    }
    int x1 = 10;
    int y1 = 10;
    int x2 = 300;
    int y2 = 300;
    int w = 5;   //тут надо от полученной ширины отнять 1
    struct rgba lnColor={100, 250 ,235, 255};
    int interior = 1;
    struct rgba inColor = {150, 150, 135, 255};
    struct Png image;
    read_png_file(argv[1], &image);
    clear_file(&image);
//    drawRectangle(&image, x1, y1, x2, y2, w, lnColor, interior, inColor);
//    drawLine(&image,10, 100, 250, 10, w, inColor);
//  drawCircle(&image, 200, 200, 150, 3, lnColor);

    //Если дан квадрат, то:
    int r = (x2-x1)/2;
    int x = x1+r;
    int y = y1+r;

    //printf("%d %d %d\n", x, y, r);
    //pentax(&image, x, y, r, 5, lnColor);
    //hexagon(&image, 200, 200, 100, 2, inColor, 1, lnColor);

    //reflection(&image, 50, 50, 250, 250, w);

    write_png_file(argv[2], &image);

    return 0;
}