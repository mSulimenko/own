#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
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
                ptr[0] = Color.r;ptr[1] = Color.g;ptr[2] = Color.b;ptr[3]=Color.a;
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
                ptr[0] = lnColor.r;ptr[1] = lnColor.g;ptr[2] = lnColor.b;ptr[3]=lnColor.a;
            }
            if((y >= y2 - w)&&(y <= y2 + w)&&(x<=x2+w)&&(x>=x1-w)){   //нижняя
                ptr[0] = lnColor.r;ptr[1] = lnColor.g;ptr[2] = lnColor.b;ptr[3]=lnColor.a;
            }
            if((x >= x1 - w)&&(x <= x1 + w)&&(y<=y2)&&(y>=y1)){   //левая
                ptr[0] = lnColor.r;ptr[1] = lnColor.g;ptr[2] = lnColor.b;ptr[3]=lnColor.a;
            }
            if((x >= x2 - w)&&(x <= x2 + w)&&(y<=y2)&&(y>=y1)){   //правая
                ptr[0] = lnColor.r;ptr[1] = lnColor.g;ptr[2] = lnColor.b;ptr[3]=lnColor.a;
            }

        }
    }
    if(interior == 1){
        inter(image, x1+w, y1+w, x2-w, y2-w, inColor);
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
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y2][(x2-i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
        }
        else{
            png_byte* ptr = &(image->row_pointers[y2+i][(x2) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y2-i][(x2) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
        }
    }
    while(x1 != x2 || y1 != y2)
    {
        for(int i = 0; i < w; i ++) {
            if(tg){png_byte* ptr = &(image->row_pointers[y1][(x1+i) * 4]);
                ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
                ptr = &(image->row_pointers[y1][(x1-i) * 4]);
                ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            }
            else{
                png_byte* ptr = &(image->row_pointers[y1+i][(x1) * 4]);
                ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
                ptr = &(image->row_pointers[y1-i][(x1) * 4]);
                ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
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
    int x1,y1,yk = 0;
    int sigma,delta,f;
    png_byte *ptr;

    x1 = 0;
    y1 = r;
    delta = 2*(1-r);
    do{
        for(int i = 0; i < w; i++){
            ptr = &(image->row_pointers[y+y1][(x+x1+i) * 4]);  //4 четверть
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y+y1][(x+x1-i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y+y1+i][(x+x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y+y1-i][(x+x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;

            ptr = &(image->row_pointers[y+y1][(x-x1+i) * 4]);  //3 четверть
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y+y1][(x-x1-i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y+y1+i][(x-x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y+y1-i][(x-x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;

            ptr = &(image->row_pointers[y-y1][(x+x1+i) * 4]);  //1 четверть
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y-y1][(x+x1-i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y-y1+i][(x+x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y-y1-i][(x+x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;

            ptr = &(image->row_pointers[y-y1][(x-x1+i) * 4]);  //2 четверть
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y-y1][(x-x1-i) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y-y1+i][(x-x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
            ptr = &(image->row_pointers[y-y1-i][(x-x1) * 4]);
            ptr[0] = color.r;ptr[1] = color.g;ptr[2] = color.b;ptr[3] = color.a;
        }

        f = 0;
        if (y1 < yk)
            break;
        if (delta < 0){
            sigma = 2*(delta+y1)-1;
            if (sigma <= 0){
                x1++;
                delta += 2*x1+1;
                f = 1;
            }
        }
        else if (delta > 0){
            sigma = 2*(delta-x1)-1;
            if (sigma > 0){
                y1--;
                delta += 1-2*y1;
                f = 1;
            }
        }
        if(!f){
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
int isSimilar(struct rgba f, struct rgba s){
    if((f.r == s.r) && (f.g == s.g) && (f.b == s.b) && (f.a == s.a)) return 1;
    return 0;
}

void flood(struct Png* image, int x, int y, struct rgba newColor, struct rgba outColor){
    png_byte* ptr = &(image->row_pointers[y][(x) * 4]);
    struct rgba cur = { ptr[0], ptr[1], ptr[2], ptr[3]};

    if( (ptr[0]!=newColor.r)||(ptr[1]!=newColor.g)||(ptr[2]!=newColor.b)||(ptr[3]!=newColor.a)){

        ptr[0]=newColor.r;ptr[1]=newColor.g;ptr[2]=newColor.b;ptr[3]=newColor.a;

        ptr = &(image->row_pointers[y][(x+1) * 4]);
        cur.r = ptr[0]; cur.g =ptr[1]; cur.b = ptr[2]; cur.a = ptr[3];
        if(!isSimilar(cur, outColor)   && x<image->width){
            flood(image, x+1, y, newColor, outColor);
        }

        ptr = &(image->row_pointers[y][(x-1) * 4]);
        cur.r = ptr[0]; cur.g =ptr[1]; cur.b = ptr[2]; cur.a = ptr[3];
        if(!isSimilar(cur, outColor)){
            flood(image, x-1, y, newColor, outColor);
        }

        ptr = &(image->row_pointers[y+1][(x) * 4]);
        cur.r = ptr[0]; cur.g =ptr[1]; cur.b = ptr[2]; cur.a = ptr[3];
        if(!isSimilar(cur, outColor)){
            flood(image, x, y+1, newColor, outColor);
        }

        ptr = &(image->row_pointers[y-1][(x) * 4]);
        cur.r = ptr[0]; cur.g =ptr[1]; cur.b = ptr[2]; cur.a = ptr[3];
        if(!isSimilar(cur, outColor)){
            flood(image, x, y-1, newColor, outColor);
        }
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


    drawLine(image, x1, y1, x6, y6, w, outColor);
    drawLine(image, x1, y1, x4, y4, w, outColor);
    drawLine(image, x3, y3, x4, y4, w, outColor);
    drawLine(image, x5, y5, x6, y6, w, outColor);
    drawLine(image, x2, y2, x5, y5, w, outColor);
    drawLine(image, x2, y2, x3, y3, w, outColor);

    if(interior == 1)
        flood(image, x, y, inColor, outColor);
}

int reflection(struct Png *image, int x1, int y1, int x2, int y2, int side){
    write_png_file_copy("NewTestTemporaryPngFile.png", image);
    struct Png image_copy;
    read_png_file("NewTestTemporaryPngFile.png", &image_copy);

    if((x1<0)||(x1>=image_copy.width)||(x2<0)||(x2>=image_copy.width)||(y1<0)||(y1>=image_copy.height)||(y2<0)||(y2>=image_copy.height)){
      printf("Error: x1 y1 x2 y2 should be less than resolution H & W\n");
      return 1;
    }

    if(side == 0){    //Горизонтально
        for(int y = 0; y <=y2-y1; y++){
            for(int x = 0; x <= x2 - x1; x++){
                png_byte* ptr = &(image->row_pointers[y+y1][(x+x1) * 4]);
                png_byte* ptr_copy = &(image_copy.row_pointers[y2-y][(x+x1) * 4]);
                ptr[0] = ptr_copy[0];ptr[1] = ptr_copy[1];ptr[2] = ptr_copy[2];ptr[3] = ptr_copy[3];
            }
        }
    }

    if(side == 1){    //Вертикально
        for(int y = 0; y <=y2-y1; y++){
            for(int x = 0; x <= x2 - x1; x++){
                png_byte* ptr = &(image->row_pointers[y+y1][(x+x1) * 4]);
                png_byte* ptr_copy = &(image_copy.row_pointers[y+y1][(x2-x) * 4]);
                ptr[0] = ptr_copy[0];ptr[1] = ptr_copy[1];ptr[2] = ptr_copy[2];ptr[3] = ptr_copy[3];
            }
        }
    }
    write_png_file("NewTestTemporaryPngFile.png", &image_copy);

    if (remove("NewTestTemporaryPngFile.png") ==-1)
        printf("Error during deleting temporary file\n");
    return 0;
}


struct Configs{
    int x1, y1, x2, y2;
    int  r, g, b, a, r2, g2, b2, a2;
    int i, horizon, t;
    int ctrX, ctrY, rad;
    int rectangle, circle, pentagram, hexagon;
};

void printHelp(){
    printf("getopt example\n");
    printf("-r --reflection <value> -s --start x1,y1 -e --end x2,y2 <file_in> <file_out>- Отражение заданной области. В пункт value впишите 1, если отражение относительно горизонтальной оси, 0 - относительно вертикальной)\n");
    printf("-p --pentagram -c --circle <ctrX,ctrY,rad> -s --start x1,y1 -e --end x2 -t --thick <value> -g --rgb <r,g,b,a> <file_in> <file_out>- нарисовать пентаграму. Введите либо координаты ее центра и радиусом окружности(-с)\n");
    printf("либо координаты левого верхнего и правого нижнего угла квадрата, в который вписана окружность пентаграммы(-s -e). Но нужно выбрать что-то одно. Значение по умолчанию - окружность.\n");
    printf("-l --rectangle -s --start x1,y1 -e --end x2,y2 -t --thick <value> -g --rgb <r,g,b,a> -i --interior <r2,g2,b2,a> <file_in> <file_out> - Рисование прямоугольника. Если хотите, чтобы он был залит, введите компоненты r2,g2,b2,a2\n");
    printf("-x --hexagon -c --circle <ctrX,ctrY,rad> -s --start x1,y1 -e --end x2 -t --thick <value> -g --rgb <r,g,b,a> -i --interior <r2,g2,b2,a> <file_in> <file_out> - нарисовать пентаграму. Введите либо координаты ее центра и радиусом окружности(-с)\n");
    printf("либо координаты левого верхнего и правого нижнего угла квадрата, в который вписана окружность пентаграммы(-s -e). Но нужно выбрать что-то одно. Значение по умолчанию - окружность.Если хотите, чтобы он был залит, введите компоненты r2,g2,b2,a2\n");

}

int main(int argc, char **argv) {
    struct Configs config = {0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1, 1, 0, 0, 0, 0, 0, 0, 0};
    char *opts = "r:g:i:s:e:pc:li:t:xh?";
    struct option longOpts[] = {
            {"reflection", required_argument, NULL, 'r'},
            {"rgb",        required_argument, NULL, 'g'},
            {"interior",   required_argument, NULL, 'i'},
            {"start",      required_argument, NULL, 's'},
            {"end",        required_argument, NULL, 'e'},
            {"pentagram",  no_argument,       NULL, 'p'},
            {"circle",     required_argument, NULL, 'c'},
            {"rectangle",  no_argument,       NULL, 'l'},
            {"interior",   required_argument, NULL, 'i'},
            {"thick",      no_argument,       NULL, 't'},
            {"hexagon",    no_argument,       NULL, 'h'},
            {"help",       no_argument,       NULL, 'h'},
            {NULL,         no_argument,       NULL, 0}
    };
    int opt;
    int longIndex;
    int count;
    char *colours;
    char *numbers;
    opt = getopt_long(argc, argv, opts, longOpts, &longIndex);

    while (opt != -1) {
        switch (opt) {
            case 'r':
                config.horizon = atoi(optarg);
                if ((config.horizon != 1) && (config.horizon != 0)) {
                    printf("Неверные параметры ориентации\n");
                    printHelp();
                }
                break;
            case 'x':
                config.hexagon = 1;
                break;
            case 'l':
                config.rectangle = 1;
                break;
            case 'p':
                config.pentagram = 1;
                break;
            case 't':
                config.t = atoi(optarg);
                break;

            case 'g':
                count = 0;
                colours = strtok(optarg, ",");
                while (colours != NULL) {
                    if (count == 0)
                        config.r = atoi(colours);
                    if (count == 1)
                        config.g = atoi(colours);
                    if (count == 2)
                        config.b = atoi(colours);
                    if (count == 3)
                        config.a = atoi(colours);
                    colours = strtok(NULL, ",");
                    count++;
                }
                if (count != 4) {
                    printHelp();
                    return 0;
                }
                break;
            case 'i':
                count = 0;
                config.i = 1;
                colours = strtok(optarg, ",");
                while (colours != NULL) {
                    if (count == 0)
                        config.r2 = atoi(colours);
                    if (count == 1)
                        config.g2 = atoi(colours);
                    if (count == 2)
                        config.b2 = atoi(colours);
                    if (count == 3)
                        config.a2 = atoi(colours);
                    colours = strtok(NULL, ",");
                    count++;
                }
                if (count != 4) {
                    printHelp();
                    return 0;
                }
                break;
            case 'c':
                count = 0;
                numbers = strtok(optarg, ",");
                while (numbers != NULL) {
                    if (count == 0)
                        config.ctrX = atoi(numbers);
                    if (count == 1)
                        config.ctrY = atoi(numbers);
                    if (count == 2)
                        config.rad = atoi(numbers);
                    numbers = strtok(NULL, ",");
                    count++;
                }
                if (count != 3) {
                    printHelp();
                    return 0;
                }
                break;
            case 's':
                count = 0;
                numbers = strtok(optarg, ",");
                while (numbers != NULL) {
                    if (count == 0)
                        config.x1 = atoi(numbers);
                    if (count == 1)
                        config.y1 = atoi(numbers);
                    numbers = strtok(NULL, ",");
                    count++;
                }
                if (count != 2) {
                    printHelp();
                    return 0;
                }
                break;
            case 'e':
                count = 0;
                numbers = strtok(optarg, ",");
                while (numbers != NULL) {
                    if (count == 0)
                        config.x2 = atoi(numbers);
                    if (count == 1)
                        config.y2 = atoi(numbers);
                    numbers = strtok(NULL, ",");
                    count++;
                }
                if (count != 2) {
                    printHelp();
                    return 0;
                }
                break;
            case 'h':
            case '?':
                printHelp();
                return 0;
            case 0:
                printf("->%s\n", longOpts[longIndex].name);
        }
        opt = getopt_long(argc, argv, opts, longOpts, &longIndex);
    }
    argc -= optind;
    argv += optind;
    if (argc != 2) {
        printHelp();
        return 0;
    }
    struct Png image;
    int checker = read_png_file(argv[0], &image);
    if (checker)
        return 0;

    if(((config.horizon == 0) || (config.horizon == 1)) + (config.pentagram == 1) + (config.rectangle == 1) + (config.hexagon == 1) > 1){
        printf("Error: You can choose only 1 function");
        printHelp();
        return 0;
    }
    if ((config.horizon == 0) || (config.horizon == 1)) {
        if (config.x1 < 0 || config.x2 < 0 || config.y1 < 0 || config.y2 < 0) {
            printf("Error: x1 y1 x2 y2 should be greater than zero\n");
            return 0;
        }
        if (config.x1 > image.width || config.x2 > image.width || config.y1 > image.height ||
            config.y2 > image.height) {
            printf("Error: x1 y1 x2 y2 should be less than resolution H & W\n");
            return 0;
        }
        if(reflection(&image, config.x1, config.y1, config.x2, config.y2, config.horizon) == 1){
            return 0;
        }
    }


    if (config.pentagram == 1) {
        if (config.r < 0 || config.g < 0 || config.b < 0 || config.a < 0) {
            printf("Error: rgb should be greater than or equal to 0\n");
            return 0;
        }
        if (config.r > 255 || config.g > 255 || config.b > 255 || config.a > 255) {
            printf("Error: rgb should be less than or equal to 255\n");
            return 0;
        }
        if (config.t < 1) {
            printf("Error: size should be greater than 0\n");
            return 0;
        }

        if (config.t > 20) {
            printf("Error: the size is too large\n");
            return 0;
        }
            if (config.x1 && config.y1 && config.x2 && config.y2) {
                if (config.x1 >= config.x2 || config.y1 >= config.y2) {
                    printf("Error: corners of square are set incorrectly\n");
                    return 0;
                }
                if ((config.x1 - config.x2) != (config.y1 - config.y2)) {
                    printf("Error: corners of square are set incorrectly\n");
                    return 0;
                }
                if (config.x1 < 0 || config.x2 < 0 || config.y1 < 0 || config.y2 < 0) {
                    printf("Error: x1 y1 x2 y2 should be greater than 0\n");
                    return 0;
                }
                if (config.x1 > image.width || config.x2 > image.width || config.y1 > image.height ||
                    config.y2 > image.height) {
                    printf("Error: x1 y1 x2 y2 should be less than resolution H & W\n");
                    return 0;
                }
                if (config.t  + config.x2 > image.width ||
                    -config.t  + config.y1 > image.height ||
                    config.x1 - config.t  < 0 ||
                    config.y2 + config.t  < 0) {
                    printf("Error: going beyond the border of picture\n");
                    return 0;
                }
                int r = (config.x2 - config.x1) / 2;
                int x = config.x1 + r;
                int y = config.y1 + r;
                struct rgba color = {config.r, config.g, config.b, config.a};


                pentax(&image, x, y, r, config.t, color);
            }
            else {
                int x1 = config.ctrX - config.rad;
                int x2 = config.ctrX + config.rad;
                int y1 = config.ctrY + config.rad;
                int y2 = config.ctrY - config.rad;

                if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) {
                    printf("Error: going beyond the border of picture\n");
                    return 0;
                }
                if (x1 > image.width || x2 > image.width || y1 > image.height || y2 > image.height) {
                    printf("Error: going beyond the border of picture\n");
                    return 0;
                }

                if (config.t / 2 + x2 > image.width ||
                    config.t / 2 + y1 > image.height ||
                    x1 - config.t / 2 < 0 ||
                    y2 - config.t / 2 < 0) {
                    printf("Error: going beyond the border of picture\n");
                    return 0;
                }
                struct rgba color = {config.r, config.g, config.b, config.a};
                pentax(&image, config.ctrX, config.ctrY, config.rad, config.t, color);
            }
        }

    if (config.rectangle == 1) {
        if (config.r < 0 || config.g < 0 || config.b < 0 || config.a < 0 ) {  //|| config.r2 < 0 || config.g2 < 0 || config.b2 < 0 || config.a2 < 0
            printf("Error: rgb should be greater than or equal to 0\n");
            return 0;
        }
        if (config.r < 0 || config.g < 0 || config.b < 0 || config.a < 0) {
            printf("Error: rgb should be less than or equal to 255\n");
            return 0;
        }
        if (config.t < 1) {
            printf("Error: size should be greater than 0\n");
            return 0;
        }

        if (config.t > 20) {
            printf("Error: the size is too large\n");
            return 0;
        }
        if (config.x1 >= config.x2 || config.y1 >= config.y2) {
            printf("Error: corners of rectangle are set incorrectly\n");
            return 0;
        }

        if (config.x1 < 0 || config.x2 < 0 || config.y1 < 0 || config.y2 < 0) {
            printf("Error: x1 y1 x2 y2 should be greater than 0\n");
            return 0;
        }
        if (config.x1 > image.width || config.x2 > image.width || config.y1 > image.height ||
            config.y2 > image.height) {
            printf("Error: x1 y1 x2 y2 should be less than resolution H & W\n");
            return 0;
        }
        if (config.t  + config.x2 > image.width ||
            -config.t  + config.y1 > image.height ||
            config.x1 - config.t  < 0 ||
            config.y2 + config.t < 0) {
            printf("Error: going beyond the border of picture\n");
            return 0;
        }

        struct rgba lnColor = {config.r, config.g, config.b, config.a};
        if(config.i == 0){
            drawRectangle(&image, config.x1, config.y1, config.x2, config.y2, config.t, lnColor, 0, lnColor);
        }
        else{
            if (config.r2 < 0 || config.g2 < 0 || config.b2 < 0 || config.a2 < 0 ) {
                printf("Error: rgb should be greater than or equal to 0\n");
                return 0;
            }
            if (config.r2 < 0 || config.g2 < 0 || config.b2 < 0 || config.a2 < 0) {
                printf("Error: rgb should be less than or equal to 255\n");
                return 0;
            }
            struct rgba inColor = {config.r2, config.g2, config.b2, config.a2};
            drawRectangle(&image, config.x1, config.y1, config.x2, config.y2, config.t, lnColor, 1, inColor);
        }
    }

    if (config.hexagon == 1) {
        if (config.r < 0 || config.g < 0 || config.b < 0 || config.a < 0) {
            printf("Error: rgb should be greater than or equal to 0\n");
            return 0;
        }
        if (config.r > 255 || config.g > 255 || config.b > 255 || config.a > 255) {
            printf("Error: rgb should be less than or equal to 255\n");
            return 0;
        }
        if (config.t < 1) {
            printf("Error: size should be greater than 0\n");
            return 0;
        }

        if (config.t > 20) {
            printf("Error: the size is too large\n");
            return 0;
        }

        if (config.x1 && config.y1 && config.x2 && config.y2) {
            if (config.x1 >= config.x2 || config.y1 >= config.y2) {
                printf("Error: corners of square are set incorrectly\n");
                return 0;
            }
            if (config.x1 < 0 || config.x2 < 0 || config.y1 < 0 || config.y2 < 0) {
                printf("Error: x1 y1 x2 y2 should be greater than 0\n");
                return 0;
            }
            if (config.x1 > image.width || config.x2 > image.width || config.y1 > image.height ||
                config.y2 > image.height) {
                printf("Error: x1 y1 x2 y2 should be less than resolution H & W\n");
                return 0;
            }

            if (config.t / 2 + config.x2 > image.width ||
                config.t / 2 + config.y1 > image.height ||
                config.x1 - config.t / 2 < 0 ||
                config.y2 - config.t / 2 < 0) {
                printf("Error: going beyond the border of picture\n");
                return 0;
            }
            int r = (config.x2 - config.x1) / 2;
            int x = config.x1 + r;
            int y = config.y1 + r;
            struct rgba lnColor = {config.r, config.g, config.b, config.a};


            if(config.i == 0){
                hexagon(&image, x, y, r, config.t, lnColor, 0, lnColor);
            }
            else{
                if (config.r2 < 0 || config.g2 < 0 || config.b2 < 0 || config.a2 < 0 ) {
                    printf("Error: rgb should be greater than or equal to 0\n");
                    return 0;
                }
                if (config.r2 < 0 || config.g2 < 0 || config.b2 < 0 || config.a2 < 0) {
                    printf("Error: rgb should be less than or equal to 255\n");
                    return 0;
                }
                struct rgba inColor = {config.r2, config.g2, config.b2, config.a2};
                    hexagon(&image, x, y, r, config.t, lnColor, 1, inColor);
            }
        }                   //Окружность
        else {
            int x1 = config.ctrX - config.rad;
            int x2 = config.ctrX + config.rad;
            int y1 = config.ctrY + config.rad;
            int y2 = config.ctrY - config.rad;

            if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) {
                printf("Error: going beyond the border of picture\n");
                return 0;
            }
            if (x1 > image.width || x2 > image.width || y1 > image.height || y2 > image.height) {
                printf("Error: going beyond the border of picture\n");
                return 0;
            }

            if (config.t / 2 + x2 > image.width ||
                config.t / 2 + y1 > image.height ||
                x1 - config.t / 2 < 0 ||
                y2 - config.t / 2 < 0) {
                printf("Error: going beyond the border of picture\n");
                return 0;
            }
            struct rgba lnColor = {config.r, config.g, config.b, config.a};
            if(config.i == 0){
                hexagon(&image, config.ctrX, config.ctrY, config.rad, config.t, lnColor, 0, lnColor);
            }
            else{
                if (config.r2 < 0 || config.g2 < 0 || config.b2 < 0 || config.a2 < 0 ) {
                    printf("Error: rgb should be greater than or equal to 0\n");
                    return 0;
                }
                if (config.r2 < 0 || config.g2 < 0 || config.b2 < 0 || config.a2 < 0) {
                    printf("Error: rgb should be less than or equal to 255\n");
                    return 0;
                }
                struct rgba inColor = {config.r2, config.g2, config.b2, config.a2};
                hexagon(&image, config.ctrX, config.ctrY, config.rad, config.t, lnColor, 1, inColor);
            }
        }
    }
    write_png_file(argv[1], &image);

    return 0;
}