#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// each pixel is 24 bits = 3 bytes = 1 each of RGB values
struct pixel {
	unsigned char b;
	unsigned char g;
	unsigned char r;
};

void blur(int num, int row, int col, struct pixel array[row][col]);
void grey(int row, int col, struct pixel array[row][col]);
void rgb(char *cmd, int row, int col, struct pixel array[row][col]);

int main(int argc, char *argv[])
{
	int row, col;
	FILE *in, *out;
	int Num_Rows, Num_Cols, Header;

	if (argc < 4) {
		printf("usage: %s y|u# <imagefile> <outfile> opt: <hdr> <rows> <cols>\n", argv[0]);
		exit(0);
	}

	char *cmd = argv[1];

	if ((in = fopen(argv[2], "r")) == (FILE *)NULL) {
		printf("cannot open %s for reading\n", argv[2]);
		exit(0);
	}
	if ((out = fopen(argv[3], "w")) == (FILE *)NULL) {
		printf("cannot open %s for writing\n", argv[3]);
		exit(0);
	}

	if (argc >= 7) {
		Header = atol(argv[4]);
		Num_Rows = atol(argv[5]);
		Num_Cols = atol(argv[6]);
	} else {
		Header = 54;	// default values
		Num_Rows = 400;	// default values
		Num_Cols = 640;	// default values
	}

	
  struct pixel img[Num_Rows][Num_Cols];

	// skip over the header
	fseek(in, Header, SEEK_SET);
	// read in the file data
	for (row=0; row<Num_Rows; row++) {
		fread(&img[row][0], Num_Cols, sizeof(struct pixel), in);
	}

	// look at the input command to see what to do
	if (strchr(cmd, 'u') != (char *)NULL) {			// if 'u' do a blur
		int num = atol(cmd+1);
		blur(num, Num_Rows, Num_Cols, img);
	} else if (strchr(cmd, 'y') != (char *)NULL) {	// if 'y' do greyscale
		grey(Num_Rows, Num_Cols, img);
	} else if (	strchr(cmd, 'r') != (char *)NULL ||
				strchr(cmd, 'g') != (char *)NULL ||
				strchr(cmd, 'b') != (char *)NULL) {	// if r or g or b, do that
		rgb(cmd, Num_Rows, Num_Cols, img);
	} else {
		printf("No valid command in [%s]\n", cmd);
		exit(0);
	}

	// write out the modified file
	char buf[128];
	// first, go to the beginning of the input file 
	fseek(in, 0, SEEK_SET);
	// then, read the header out of the input file
	fread(buf, Header, 1, in);
	// then write that to the output file
	fwrite(buf, Header, 1, out);
	// and then write all the other image data out to the file
	for (row=0; row<Num_Rows; row++) {
		fwrite(&img[row][0], Num_Cols, sizeof(struct pixel), out);
	}

	fclose(in);
	fclose(out);

}

void rgb(char *cmd, int Num_Rows, int Num_Cols, struct pixel img[Num_Rows][Num_Cols]) {
	int row, col;
	for (row=0; row<Num_Rows; row++) {
		for (col=0; col<Num_Cols; col++) {
			if (strchr(cmd, 'r') != (char *)NULL) {
				img[row][col].r += 30;
			}
			if (strchr(cmd, 'g') != (char *)NULL) {
				img[row][col].g += 30;
			}
			if (strchr(cmd, 'b') != (char *)NULL) {
				img[row][col].b += 30;
			}
		}
	}
}

// convert the image to greyscale
void grey(int Num_Rows, int Num_Cols, struct pixel img[Num_Rows][Num_Cols]) {
	int row, col;
	for (row=0; row<Num_Rows; row++) {
		for (col=0; col<Num_Cols; col++) {
			int px =	img[row][col].r +
						    img[row][col].g +
						    img[row][col].b;
      img[row][col].r = px/3;
      img[row][col].g = px/3;
      img[row][col].b = px/3;
		}
	}
}

// blur the image (each pixel gets the average of it and surrounding 8 pixels)
void blur(int num, int Num_Rows, int Num_Cols, struct pixel img[Num_Rows][Num_Cols]) {
	int row, col;
    for(int i=0; i<num; i++) {
  	for (row=0; row<Num_Rows; row++) {
  		for (col=0; col<Num_Cols; col++) {
  			int r=0, g=0, b=0, ct=0;
  			if (row-1 >= 0 && col-1 >= 0) {
  				r += img[row-1][col-1].r;
  				g += img[row-1][col-1].g;
  				b += img[row-1][col-1].b;
  				ct ++;
  			}
  			if (row-1 >= 0) {
  				r += img[row-1][col].r;
  				g += img[row-1][col].g;
  				b += img[row-1][col].b;
  				ct ++;
  			}
  			if (row-1 >= 0 && col+1 < Num_Cols) {
  				r += img[row-1][col+1].r;
  				g += img[row-1][col+1].g;
  				b += img[row-1][col+1].b;
  				ct ++;
  			}
  
  			if (col-1 >= 0) {
  				r += img[row][col-1].r;
  				g += img[row][col-1].g;
  				b += img[row][col-1].b;
  				ct ++;
  			}
  			if (1) {
  				r += img[row][col].r;
  				g += img[row][col].g;
  				b += img[row][col].b;
  				ct ++;
  			}
  			if (col+1 < Num_Cols) {
  				r += img[row][col+1].r;
  				g += img[row][col+1].g;
  				b += img[row][col+1].b;
  				ct ++;
  			}
  
  			if (row+1 < Num_Rows && col-1 >= 0) {
  				r += img[row+1][col-1].r;
  				g += img[row+1][col-1].g;
  				b += img[row+1][col-1].b;
  				ct ++;
  			}
  			if (row+1 < Num_Rows) {
  				r += img[row+1][col].r;
  				g += img[row+1][col].g;
  				b += img[row+1][col].b;
  				ct ++;
  			}
  			if (row+1 < Num_Rows && col+1 < Num_Cols) {
  				r += img[row+1][col+1].r;
  				g += img[row+1][col+1].g;
  				b += img[row+1][col+1].b;
  				ct ++;
  			}
  
  			if (ct < 0) ct = 1;
  
  			img[row][col].r = (unsigned char)((r / ct) & 0xFF);
  			img[row][col].g = (unsigned char)((g / ct) & 0xFF);
  			img[row][col].b = (unsigned char)((b / ct) & 0xFF);
  		}
  	}
  }
}
