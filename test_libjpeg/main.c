/*
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
 * Copyright(C) 2019 ****, Inc.
 *
 *  File Name: 
 *  Created  : 12-01-19 09:46:44
 *
 *  Function
 *    libjpeg test
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  $Revision: 1.1.1.1 $
 *  $Date: 2008/07/27 10:32:48 $
 *  $Author: poko $
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
 */

/****************************************************************************
 * Included files
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "jpeglib.h"
#include "jerror.h"
#include "png.h"

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define INP_FILENAME  "../flower.bmp"
//#define INP_FILENAME  "../zeno.bmp"
//#define INP_FILENAME  "../pic.bmp"
#define OUT_FILENAME  "flower.jpg"
#define PNG_FILENAME  "../hanko.png"

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define OUTPUT_BUF_MIN_SIZE  4096
#define OUTPUT_BUF_MAX_SIZE  60000

/****************************************************************************
 * Private types
 ****************************************************************************/

#pragma pack (2)

struct bmp_t
{
  unsigned short  bfType;
  unsigned long   bfSize;
  unsigned short  bfReserved1;
  unsigned short  bfReserved2;
  unsigned long   bfOffBits;
  unsigned long   biSize;
  unsigned long   biWidth;
  unsigned long   biHeight;
  unsigned short  biPlanes;
  unsigned short  biBitCount;
  unsigned long   biCompression;
  unsigned long   biSizeImage;
  unsigned long   biXPixPerMeter;
  unsigned long   biYPixPerMeter;
  unsigned long   biClrUsed;
  unsigned long   biCirImportant;
};

/****************************************************************************
 * Private functions
 ****************************************************************************/

static unsigned char *read_bmp_file(const char    *filename,
                                    int           *width,
                                    int           *height,
                                    unsigned long *size)
{
  FILE          *fp;
  unsigned char *cp = NULL;
  struct bmp_t   bmp;

  fp = fopen(filename, "rb");

  if (fp)
    {
      /* Get header of bitmap file */

      fread(&bmp, 1, sizeof(bmp), fp);

      *width  = bmp.biWidth;
      *height = bmp.biHeight;
      *size   = bmp.bfSize;

      if (bmp.bfType == 0x4D42)
        {
          cp = malloc(bmp.bfSize);

          if (cp)
            {
              fread(cp, 1, bmp.bfSize, fp);
            }
        }

      fclose(fp);
    }

  return cp;
}

/*--------------------------------------------------------------------------*/
static int write_file(const char    *filename,
                      unsigned char *bp,
                      unsigned long  size)
{
  FILE  *fp;
  int    res = -1;

  fp = fopen(filename, "wb");

  if (fp)
    {
      fwrite(bp, 1, size, fp);

      fclose(fp);

      /* Successful */

      res = 0;
    }

  return res;
}

/* Expanded data destination object for memory output */

typedef struct {
  struct jpeg_destination_mgr pub;        /* public fields */
  boolean                     succeed;
}
my_mem_destination_mgr;

typedef my_mem_destination_mgr *my_mem_dest_ptr;
typedef my_mem_destination_mgr  my_mem_dest;

/*-------------------------------------------------------------------------*/
METHODDEF(void) mem_destination (j_compress_ptr cinfo){}

/*-------------------------------------------------------------------------*/
METHODDEF(boolean) empty_mem_output_buffer (j_compress_ptr cinfo)
{
  /* empty_mem_output_buffer */

  my_mem_dest_ptr dest = (my_mem_dest_ptr) cinfo->dest;

  dest->pub.next_output_byte = dest->pub.next_output_byte - OUTPUT_BUF_MIN_SIZE;
  dest->pub.free_in_buffer   = OUTPUT_BUF_MIN_SIZE;
  dest->succeed              = FALSE;

  return TRUE;
}

/****************************************************************************
 * Public functions
 ****************************************************************************/

unsigned char *encode(unsigned char *inptr,
                      int            width,
                      int            height,
                      int            quality,
                      unsigned char *outptr,
                      unsigned long *size)
{
  struct jpeg_compress_struct jpeg;
  struct jpeg_error_mgr       err;
  FILE                       *fp;
  JSAMPLE                    *addr;
  int                         comp = 3; /* RGB 24bit */
  my_mem_dest                 dest;

  /* Initialize Jpeg encoding */

  jpeg.err = jpeg_std_error(&err);

  jpeg_create_compress(&jpeg);

  /* Setting the output memory */

  jpeg.dest = (struct jpeg_destination_mgr *)&dest;

  dest.pub.init_destination    = mem_destination;
  dest.pub.empty_output_buffer = empty_mem_output_buffer;
  dest.pub.term_destination    = mem_destination;
  dest.pub.next_output_byte    = outptr;
  dest.pub.free_in_buffer      = *size;
  dest.succeed                 = TRUE;

  /* Set encoded image information */

  jpeg.image_width      = width;
  jpeg.image_height     = height;
  jpeg.input_components = comp;
  jpeg.in_color_space   = JCS_RGB;

  jpeg_set_defaults(&jpeg);
  jpeg_set_quality(&jpeg, quality, TRUE);

  /* Start encode */

  jpeg_start_compress(&jpeg, TRUE);

  width *= comp;

  unsigned char *img = malloc(width);
  unsigned char *ptr;

  for (int j = height - 1; j >= 0; j-- )
    {
      ptr = inptr + j * ((width + 3) & ~3);

      for (int i = 0; i < width; i+= comp)
        {
          /* BGR to RGB */
#if 1
          img[i + 0] = *(ptr + i + 2);
          img[i + 1] = *(ptr + i + 1);
          img[i + 2] = *(ptr + i + 0);
#else
          img[i + 0] = rand() & 0xFF;
          img[i + 1] = rand() & 0xFF;
          img[i + 2] = rand() & 0xFF;
#endif
        }

      jpeg_write_scanlines(&jpeg, &img, 1);
    }

  free(img);

  /* Output encoded image */

  jpeg_finish_compress(&jpeg);

  /* Release Jpeg */

  jpeg_destroy_compress(&jpeg);

  /* Set encode size */

  *size -= dest.pub.free_in_buffer;

  if (dest.succeed == FALSE)
    {
      return NULL;
    }

  return outptr;
}

/*--------------------------------------------------------------------------*/
unsigned char *decode(char *file_name)
{
  FILE  *fp = fopen(file_name, "rb");
  char   head[8];

  if (fp == NULL)
    {
      printf("File not found.\n");

      return NULL;
    }

  if (png_sig_cmp(head, 0, 8))
    {
      printf("PNG\n");
    }
  else
    {
      printf("NG\n");

      return NULL;
    }

  fclose(fp);

  return NULL;
}

/*-------------------------------------------------------------------------*/
int main(void)
{
  int            res = -1;
  int            width;
  int            height;
  int            quality = 95;
  unsigned long  size;
  unsigned char *outptr;
  unsigned char *buffer;
  unsigned char *inptr = read_bmp_file(INP_FILENAME,
                                      &width,
                                      &height,
                                      &size);

  if (inptr == NULL)
    {
      printf("File not found or memory allocation failed.\n");

      return res;
    }

  printf("Width  = %d\n", width);
  printf("Height = %d\n", height);
  printf("Size   = %d\n", size);

  /* Suppose the output memory is about half of the input */

  size = OUTPUT_BUF_MAX_SIZE;

  printf("size1 = %d\n", size);

  buffer = malloc(size);

  /* Start encoding */

  for (int i = quality; i > 1; i /= 2)
    {
      printf("%d\n", i);
      outptr = encode(inptr, width, height, i, buffer, &size);

      if (outptr)
        {
          break;
        }
    }

  

  free(inptr);

  printf("size2 = %d\n", size);

  if (outptr == NULL)
    {
      printf("Encoding failed.\n");

      return res;
    }

  /* Encoding succeed, Write to a file */

  if (write_file(OUT_FILENAME, outptr, size) == 0)
    {
      /* Successful */

      printf("Successful!\n");

      res = 0;
    }
  else
    {
      printf("File creation failed.\n");
    }

  free(outptr);

  decode(PNG_FILENAME);

  return res;
}
