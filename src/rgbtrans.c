/*
 *  Filename : rgbtrans.c
 *   Version : 1.0
 *
 *    Author : Martin Reddy, <M.Reddy@ed.ac.uk>
 *      Date : 3 Nov 1996.
 *
 *   Purpose : take any RGB image (BW/RGB/RGBA) and output an RGBA image.
 *             a list of colours can be provided and these are used to
 *             define which colours within the RGBA image will be transparent.
 *             (Uses Paul Haeberli's image library.)
 *
 *   Compile : "cc rgbtrans.c -o rgbtrans -limage"
 *
 */
#include "FixFVers.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <gl/image.h>
//#include "C:\FGCVS\grass51\lib\image\image.h"
#include "image2.h"

#define TRUE      1
#define FALSE     0
#define RGBA_EXT  ".rgba"
#define TEMP_EXT  ".tmp"
#define IncPtr(x,y) ( ((int)(x)) + ((int)(y)) )

/* a structure to hold an RGB colour definition, and a list of these */

typedef struct { 
  short r, g, b;
} colour_t;

colour_t *colour;
int      noOfColours;

/* Program usage information */

void usage( void )
{
  puts( "rgbtrans v1.0. Martin Reddy, 1996." );
  puts( "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" );
  puts( "  usage : rgbtrans <rgbfile> <colour1> [<colour2> [<colour3> ...]]" );
  puts( "purpose : takes any RGB image and generates an RGBA image in which" );
  puts( "          all of the specified colours are transparent." );
  puts( " output : the output file is <rgbfile> with a .rgba extension" );
  puts( "          (if input file has .rgba ext, it will be overwritten)");
  puts( " params : format for <colour> = 0xRRGGBB (hexidecimal notation)" );
  puts( "example : rgbtrans image.rgb 0x000000" );
  exit( 1 );
}

/* add a specified extension to a filename, replacing one if already there */
#ifdef   ADD_RGB_SUPPORT2

void addExt( char* filename, char* ext )
{
    char* dot;
    if ( (dot = (char *) strrchr( filename, '.' )) != NULL ) *dot = '\0';
    strcat( filename, ext );
}

/* return the decimal value of the first two hexidec. chars of a string */

short convertHex( char *hexstring )
{
  char hexbyte[3];
  hexbyte[0] = hexstring[0];
  hexbyte[1] = hexstring[1];
  hexbyte[2] = '\0';
  return (short) strtoul( hexbyte, NULL, 16 );
}

/* check that the command line parameters and build the colour array */

void checkParams( int argc, char *argv[] )
{
  FILE *handle;
//  char *ext;
  int  i, j, valid;

  /* provide program usage information if not enough parameters */

  if ( argc < 3 ) usage();

  /* check to see if the specified filename actually exists */

  if ( ( handle = fopen( argv[1], "r" ) ) == NULL ) {
    fprintf( stderr, "%s: file \"%s\" does not exist.\n", argv[0], argv[1] );
    exit( 1 );
  } else
    fclose( handle );

  /* create a table to hold all of the colour specifications */

  colour = (colour_t *) malloc( argc * sizeof(colour_t) );
  noOfColours = 0;

  /* check the format of each of the colour specifications */

  for ( i = 2; i < argc; ++i ) {
    valid = TRUE;

    if ( strlen( argv[i] ) != 8 ) valid = FALSE;
    if ( argv[i][0] != '0' && toupper(argv[i][1]) != 'X' ) valid = FALSE;

    for ( j = 2; j <= 7 && valid == TRUE; ++j ) {
      argv[i][j] = toupper( argv[i][j] );
      if ( ( argv[i][j] < '0' || argv[i][j] > '9' ) &&
	   ( argv[i][j] < 'A' || argv[i][j] > 'F' ) ) 
	valid = FALSE;
    }

    if ( valid == FALSE ) {
      fprintf( stderr, "%s: \"%s\" not in 0xRRGGBB notation.\n",
	       argv[0], argv[i] );
      exit( 1 );
    }

    colour[noOfColours].r = convertHex( (char *) IncPtr( argv[i], 2 ) );
    colour[noOfColours].g = convertHex( (char *) IncPtr( argv[i], 4 ) );
    colour[noOfColours].b = convertHex( (char *) IncPtr( argv[i], 6 ) );

    noOfColours++;
  }
}

/* the main program: this is the biggy where is all happens! */

int rgbtrans_main( int argc, char *argv[] )
{
  IMAGE *in_image, *out_image;
  char  in_filename[100], out_filename[100];
  int   x, y, w, h, a, c;
  short *rbuf, *gbuf, *bbuf,*abuf;

  /* check that we have enough parameters and that they are all valid */

  checkParams( argc, argv );

  /* work out the name of the input RGB image and the output RGBA image */

  strcpy( in_filename, argv[1] );
  strcpy( out_filename, argv[1] );
  addExt( out_filename, TEMP_EXT );

  /* open the input and output image files */

  if ( ( in_image = iopen( in_filename, "r" ) ) == NULL ) {
    fprintf( stderr, "%s: \"%s\" is not a valid RGB image\n",
	     argv[0], in_filename );
    exit(1);
  }

  w = in_image->xsize;
  h = in_image->ysize;

  if ( (out_image = iopen( out_filename, "w", RLE(1), 3, w, h, 4 )) == NULL) { 
    fprintf( stderr, "%s: cannot write to file %s\n", argv[0], out_filename );
    exit(1);
  }

  /* allocate some buffers for the image data */

  rbuf = (short *) malloc( w * sizeof(short) );
  gbuf = (short *) malloc( w * sizeof(short) );
  bbuf = (short *) malloc( w * sizeof(short) );
  abuf = (short *) malloc( w * sizeof(short) );

  /* generate the new RGBA file.... */

  for( y = 0; y < h; ++y ) {

    /* read in the RGB data from the input file */

    getrow( in_image, rbuf, y, 0 );
    getrow( in_image, gbuf, y, 1 );
    getrow( in_image, bbuf, y, 2 );

    /* generate the alpha channel from the RGB data and colour specs */

    for ( x = 0; x < w; ++x ) {
      a = 255;

      for ( c = 0; c < noOfColours && a > 0; ++c )
	if ( rbuf[x] == colour[c].r && 
	     gbuf[x] == colour[c].g &&
	     bbuf[x] == colour[c].b )
	  a = 0;
      
      abuf[x] = a;
    }

    /* output the new image data to the RGBA file */

    putrow( out_image, rbuf, y, 0 );
    putrow( out_image, gbuf, y, 1 );
    putrow( out_image, bbuf, y, 2 );
    putrow( out_image, abuf, y, 3 );
  }

  /* close all open files, move the temp file to the final file and exit */

  iclose( in_image );
  iclose( out_image );

  addExt( in_filename, TEMP_EXT );
  addExt( out_filename, RGBA_EXT );
  rename( in_filename, out_filename );

  printf( "Transparent (RGBA) image written to %s.\n", out_filename );

  return 0;
}

#endif // #ifdef   ADD_RGB_SUPPORT2

/*** EOF: rgbtrans.c ***/

