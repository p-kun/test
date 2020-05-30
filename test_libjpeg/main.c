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

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define INP_FILENAME  "flower.bmp"
#define OUT_FILENAME  "flower.jpg"

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

/* -------------------------------------------------------------------------*/
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

/****************************************************************************
 * Public functions
 ****************************************************************************/

unsigned char *encode(unsigned char *inptr,
                      int            width,
                      int            height,
                      unsigned long *size)
{
  struct jpeg_compress_struct jpeg;
  struct jpeg_error_mgr       err;
  FILE                       *fp;
  JSAMPLE                    *addr;
  int                         comp = 3; /* RGB 24bit */
  unsigned char              *cp;

  /* Secure output destination memory */

  cp = malloc(*size);

  if (cp == NULL)
    {
      return NULL;
    }

  /* Initialize Jpeg encoding */

  jpeg.err = jpeg_std_error(&err);

  jpeg_create_compress(&jpeg);

  /* Setting the output memory */

  jpeg_mem_dest(&jpeg, &cp, size);

  /* Set encoded image information */

  jpeg.image_width      = width;
  jpeg.image_height     = height;
  jpeg.input_components = comp;
  jpeg.in_color_space   = JCS_RGB;

  jpeg_set_defaults(&jpeg);
  jpeg_set_quality(&jpeg, 40, TRUE);

  /* Start encode */

  jpeg_start_compress(&jpeg, TRUE);

  width *= comp;

  unsigned char *img = malloc(width);
  unsigned char *ptr;

  for (int j = height - 1; j >= 0; j-- )
    {
      ptr = inptr + j * width;

      for (int i = 0; i < width; i+= comp)
        {
          /* BGR to RGB */

          img[i + 0] = *(ptr + i + 2);
          img[i + 1] = *(ptr + i + 1);
          img[i + 2] = *(ptr + i + 0);
        }

      jpeg_write_scanlines(&jpeg, &img, 1);
    }

  free(img);

  /* Output encoded image */

  jpeg_finish_compress(&jpeg);

  /* Release Jpeg */

  jpeg_destroy_compress(&jpeg);

  return cp;
}

/* -------------------------------------------------------------------------*/
int main(void)
{
  int            res = -1;
  int            width;
  int            height;
  unsigned long  size;
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

  size /= 2;

  /* Start encoding */

  unsigned char *outptr = encode(inptr,
                                 width,
                                 height,
                                &size);
  free(inptr);

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

  return res;
}
