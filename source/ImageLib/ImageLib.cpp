#define XMD_H

#include <windows.h>
#include "ImageLib.h"
#include "png\png.h"
#include <math.h>
#include <tchar.h>
#include "..\PakLib\PakInterface.h"

extern "C"
{
#include "jpeg\jpeglib.h"
#include "jpeg\jerror.h"
}

//#include "jpeg2000/jasper.h"

using namespace ImageLib;

Image::Image()
{
	mWidth = 0;
	mHeight = 0;
	mBits = NULL;
}

Image::~Image()
{
	delete mBits;
}

int	Image::GetWidth()
{
	return mWidth;
}

int	Image::GetHeight()
{
	return mHeight;
}

unsigned long* Image::GetBits()
{
	return mBits;
}

//////////////////////////////////////////////////////////////////////////
// PNG Pak Support

static void png_pak_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_size_t check;

	/* fread() returns 0 on error, so it is OK to store this in a png_size_t
	* instead of an int, which is what fread() actually returns.
	*/
	check = (png_size_t)p_fread(data, (png_size_t)1, length,
		(PFILE*)png_ptr->io_ptr);

	if (check != length)
	{
		png_error(png_ptr, "Read Error");
	}
}

Image* GetPNGImage(const std::string& theFileName)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	PFILE *fp;

	if ((fp = p_fopen(theFileName.c_str(), "rb")) == NULL)
		return NULL;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
	  NULL, NULL, NULL);
	png_set_read_fn(png_ptr, (png_voidp)fp, png_pak_read_data);

	if (png_ptr == NULL)
	{
		p_fclose(fp);
		return NULL;
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		p_fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return NULL;
	}

   /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */
	if (setjmp(png_ptr->jmpbuf))
	{
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		p_fclose(fp);
		/* If we get here, we had a problem reading the file */
		return NULL;
	}

	//png_init_io(png_ptr, fp);

	//png_ptr->io_ptr = (png_voidp)fp;

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
       &interlace_type, NULL, NULL);

	/* Add filler (or alpha) byte (before/after each RGB triplet) */
	png_set_expand(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	//png_set_gray_1_2_4_to_8(png_ptr);
	png_set_palette_to_rgb(png_ptr);
	png_set_gray_to_rgb(png_ptr);
	png_set_bgr(png_ptr);

//	int aNumBytes = png_get_rowbytes(png_ptr, info_ptr) * height / 4;
	unsigned long* aBits = new unsigned long[width*height];
	unsigned long* anAddr = aBits;
	for (int i = 0; i < height; i++)
	{
		png_read_rows(png_ptr, (png_bytepp) &anAddr, NULL, 1);
		anAddr += width;
	}

	/* read rest of file, and get additional chunks in info_ptr - REQUIRED */
	png_read_end(png_ptr, info_ptr);

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	/* close the file */
	p_fclose(fp);

	Image* anImage = new Image();
	anImage->mWidth = width;
	anImage->mHeight = height;
	anImage->mBits = aBits;

	return anImage;
}

Image* GetTGAImage(const std::string& theFileName)
{
	PFILE* aTGAFile = p_fopen(theFileName.c_str(), "rb");
	if (aTGAFile == NULL)
		return NULL;

	BYTE aHeaderIDLen;
	p_fread(&aHeaderIDLen, sizeof(BYTE), 1, aTGAFile);

	BYTE aColorMapType;
	p_fread(&aColorMapType, sizeof(BYTE), 1, aTGAFile);
	
	BYTE anImageType;
	p_fread(&anImageType, sizeof(BYTE), 1, aTGAFile);

	WORD aFirstEntryIdx;
	p_fread(&aFirstEntryIdx, sizeof(WORD), 1, aTGAFile);

	WORD aColorMapLen;
	p_fread(&aColorMapLen, sizeof(WORD), 1, aTGAFile);

	BYTE aColorMapEntrySize;
	p_fread(&aColorMapEntrySize, sizeof(BYTE), 1, aTGAFile);	

	WORD anXOrigin;
	p_fread(&anXOrigin, sizeof(WORD), 1, aTGAFile);

	WORD aYOrigin;
	p_fread(&aYOrigin, sizeof(WORD), 1, aTGAFile);

	WORD anImageWidth;
	p_fread(&anImageWidth, sizeof(WORD), 1, aTGAFile);	

	WORD anImageHeight;
	p_fread(&anImageHeight, sizeof(WORD), 1, aTGAFile);	

	BYTE aBitCount = 32;
	p_fread(&aBitCount, sizeof(BYTE), 1, aTGAFile);	

	BYTE anImageDescriptor = 8 | (1<<5);
	p_fread(&anImageDescriptor, sizeof(BYTE), 1, aTGAFile);

	if ((aBitCount != 32) ||
		(anImageDescriptor != (8 | (1<<5))))
	{
		p_fclose(aTGAFile);
		return NULL;
	}

	Image* anImage = new Image();

	anImage->mWidth = anImageWidth;
	anImage->mHeight = anImageHeight;
	anImage->mBits = new unsigned long[anImageWidth*anImageHeight];

	p_fread(anImage->mBits, 4, anImage->mWidth*anImage->mHeight, aTGAFile);

	p_fclose(aTGAFile);

	return anImage;
}

int ReadBlobBlock(PFILE* fp, char* data)
{
	unsigned char aCount = 0;
	p_fread(&aCount, sizeof(char), 1, fp);
	p_fread(data, sizeof(char), aCount, fp);
	return aCount;
}

Image* GetGIFImage(const std::string& theFileName)
{
	#define BitSet(byte,bit)  (((byte) & (bit)) == (bit))
	#define LSBFirstOrder(x,y)  (((y) << 8) | (x))

	int
		opacity,
		status;

	register int i;

	register unsigned char *p;

	unsigned char
		background,
		c,
		flag,
		*global_colormap,
		header[1664],
		magick[12];

	unsigned int
		delay,
		dispose,
		global_colors,
		image_count,
		iterations;

	/*
	Open image file.
	*/

	PFILE *fp;

	if ((fp = p_fopen(theFileName.c_str(), "rb")) == NULL)
		return NULL;
	/*
	Determine if this is a GIF file.
	*/
	status=p_fread(magick, sizeof(char), 6, fp);

	if (((strncmp((char *) magick,"GIF87",5) != 0) &&
		(strncmp((char *) magick,"GIF89",5) != 0)))
		return NULL;

	global_colors=0;
	global_colormap=(unsigned char *) NULL;

	short pw;
	short ph;

	p_fread(&pw, sizeof(short), 1, fp);
	p_fread(&ph, sizeof(short), 1, fp);
	p_fread(&flag, sizeof(char), 1, fp);
	p_fread(&background, sizeof(char), 1, fp);
	p_fread(&c, sizeof(char), 1, fp);

	if (BitSet(flag,0x80))
	{
		/*
		opacity global colormap.
		*/
		global_colors=1 << ((flag & 0x07)+1);
		global_colormap=new unsigned char[3*global_colors];
		if (global_colormap == (unsigned char *) NULL)
		return NULL;

		p_fread(global_colormap, sizeof(char), 3*global_colors, fp);
	}

	delay=0;
	dispose=0;
	iterations=1;
	opacity=(-1);
	image_count=0;

	for ( ; ; )
	{
		if (p_fread(&c, sizeof(char), 1, fp) == 0)
		break;

		if (c == ';')
		break;  /* terminator */
		if (c == '!')
		{
			/*
			GIF Extension block.
			*/
			p_fread(&c, sizeof(char), 1, fp);

			switch (c)
			{
			case 0xf9:
				{
					/*
					Read Graphics Control extension.
					*/
					while (ReadBlobBlock(fp,(char *) header) > 0);

					dispose=header[0] >> 2;
					delay=(header[2] << 8) | header[1];
					if ((header[0] & 0x01) == 1)
					opacity=header[3];
					break;
				}
			case 0xfe:
				{
					char *comments;
					int length;

					/*
					Read Comment extension.
					*/
					comments=(char *) NULL;
					for ( ; ; )
					{
						length=ReadBlobBlock(fp,(char *) header);
						if (length <= 0)
						break;
						if (comments == NULL)
						{
							comments= new char[length+1];
							if (comments != (char *) NULL)
							*comments='\0';
						}

						header[length]='\0';
						strcat(comments,(char *) header);
					}
					if (comments == (char *) NULL)
						break;

					delete comments;
					break;
				}
			case 0xff:
				{
					int
					loop;

					/*
					Read Netscape Loop extension.
					*/
					loop=false;
					if (ReadBlobBlock(fp,(char *) header) > 0)
					loop=!strncmp((char *) header,"NETSCAPE2.0",11);
					while (ReadBlobBlock(fp,(char *) header) > 0)
					if (loop)
					iterations=(header[2] << 8) | header[1];
					break;
				}
			default:
				{
					while (ReadBlobBlock(fp,(char *) header) > 0);
					break;
				}
			}
		}

		if (c != ',')
			continue;

		if (image_count != 0)
		{
			/*
			Allocate next image structure.
			*/

			/*AllocateNextImage(image_info,image);
			if (image->next == (Image *) NULL)
			{
			DestroyImages(image);
			return((Image *) NULL);
			}
			image=image->next;
			MagickMonitor(LoadImagesText,TellBlob(image),image->filesize);*/
		}
		image_count++;

		short pagex;
		short pagey;
		short width;
		short height;
		int colors;
		bool interlaced;

		p_fread(&pagex, sizeof(short), 1, fp);
		p_fread(&pagey, sizeof(short), 1, fp);
		p_fread(&width, sizeof(short), 1, fp);
		p_fread(&height, sizeof(short), 1, fp);
		p_fread(&flag, sizeof(char), 1, fp);

		colors=!BitSet(flag,0x80) ? global_colors : 1 << ((flag & 0x07)+1);
		unsigned long* colortable = new unsigned long[colors];

		interlaced = BitSet(flag,0x40);

		delay=0;
		dispose=0;
		iterations=1;
		/*if (image_info->ping)
		{
		f (opacity >= 0)
		/image->matte=true;

		CloseBlob(image);
		return(image);
		}*/
		if ((width == 0) || (height == 0))
			return NULL;
		/*
		Inititialize colormap.
		*/
		/*if (!AllocateImageColormap(image,image->colors))
		ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
		image);*/
		if (!BitSet(flag,0x80))
		{
			/*
			Use global colormap.
			*/
			p=global_colormap;
			for (i=0; i < (int) colors; i++)
			{
				int r = *p++;
				int g = *p++;
				int b = *p++;

				colortable[i] = 0xFF000000 | (r << 16) | (g << 8) | (b);
			}

			//image->background_color=
			//image->colormap[Min(background,image->colors-1)];
		}
		else
		{
			unsigned char
			*colormap;

			/*
			Read local colormap.
			*/
			colormap=new unsigned char[3*colors];

			int pos = p_ftell(fp);
			p_fread(colormap, sizeof(char), 3*colors, fp);

			p=colormap;
			for (i=0; i < (int) colors; i++)
			{
				int r = *p++;
				int g = *p++;
				int b = *p++;

				colortable[i] = 0xFF000000 | (r << 16) | (g << 8) | (b);
			}
			delete colormap;
		}

		/*if (opacity >= (int) colors)
		{
		for (i=colors; i < (opacity+1); i++)
		{
		image->colormap[i].red=0;
		image->colormap[i].green=0;
		image->colormap[i].blue=0;
		}
		image->colors=opacity+1;
		}*/
		/*
		Decode image.
		*/
		//status=DecodeImage(image,opacity,exception);

		//if (global_colormap != (unsigned char *) NULL)
		// LiberateMemory((void **) &global_colormap);
		if (global_colormap != NULL)
		{
			delete [] global_colormap;
			global_colormap = NULL;
		}

		//while (image->previous != (Image *) NULL)
		//    image=image->previous;

		#define MaxStackSize  4096
		#define NullCode  (-1)

		int
			available,
			bits,
			code,
			clear,
			code_mask,
			code_size,
			count,
			end_of_information,
			in_code,
			offset,
			old_code,
			pass,
			y;

		register int
			x;

		register unsigned int
			datum;

		short
			*prefix;

		unsigned char
			data_size,
			first,
			*packet,
			*pixel_stack,
			*suffix,
			*top_stack;

		/*
		Allocate decoder tables.
		*/

		packet=new unsigned char[256];
		prefix=new short[MaxStackSize];
		suffix=new unsigned char [MaxStackSize];
		pixel_stack= new unsigned char[MaxStackSize+1];

		/*
		Initialize GIF data stream decoder.
		*/
		p_fread(&data_size, sizeof(char), 1, fp);
		clear=1 << data_size;
		end_of_information=clear+1;
		available=clear+2;
		old_code=NullCode;
		code_size=data_size+1;
		code_mask=(1 << code_size)-1;
		for (code=0; code < clear; code++)
		{
			prefix[code]=0;
			suffix[code]=(unsigned char) code;
		}
		/*
		Decode GIF pixel stream.
		*/
		datum=0;
		bits=0;
		c=0;
		count=0;
		first=0;
		offset=0;
		pass=0;
		top_stack=pixel_stack;

		unsigned long* aBits = new unsigned long[width*height];

		register unsigned char *c = NULL;

		for (y=0; y < (int) height; y++)
		{
			//q=SetImagePixels(image,0,offset,width,1);
			//if (q == (PixelPacket *) NULL)
			//break;
			//indexes=GetIndexes(image);

			unsigned long* q = aBits + offset*width;



			for (x=0; x < (int) width; )
			{
				if (top_stack == pixel_stack)
				{
					if (bits < code_size)
					{
						/*
						Load bytes until there is enough bits for a code.
						*/
						if (count == 0)
						{
							/*
							Read a new data block.
							*/
							int pos = p_ftell(fp);

							count=ReadBlobBlock(fp,(char *) packet);
							if (count <= 0)
							break;
							c=packet;
						}
						datum+=(*c) << bits;
						bits+=8;
						c++;
						count--;
						continue;
					}
					/*
					Get the next code.
					*/
					code=datum & code_mask;
					datum>>=code_size;
					bits-=code_size;
					/*
					Interpret the code
					*/
					if ((code > available) || (code == end_of_information))
					break;
					if (code == clear)
					{
						/*
						Reset decoder.
						*/
						code_size=data_size+1;
						code_mask=(1 << code_size)-1;
						available=clear+2;
						old_code=NullCode;
						continue;
					}
					if (old_code == NullCode)
					{
						*top_stack++=suffix[code];
						old_code=code;
						first=(unsigned char) code;
						continue;
					}
					in_code=code;
					if (code >= available)
					{
						*top_stack++=first;
						code=old_code;
					}
					while (code >= clear)
					{
						*top_stack++=suffix[code];
						code=prefix[code];
					}
					first=suffix[code];
					/*
					Add a new string to the string table,
					*/
					if (available >= MaxStackSize)
						break;
					*top_stack++=first;
					prefix[available]=old_code;
					suffix[available]=first;
					available++;
					if (((available & code_mask) == 0) && (available < MaxStackSize))
					{
						code_size++;
						code_mask+=available;
					}
					old_code=in_code;
				}
				/*
				Pop a pixel off the pixel stack.
				*/
				top_stack--;

				int index=(*top_stack);

				*q=colortable[index];

				if (index == opacity)
					*q = 0;

				x++;
				q++;
			}

			if (!interlaced)
				offset++;
			else
			{
				switch (pass)
				{
				case 0:
				default:
					{
						offset+=8;
						if (offset >= height)
						{
							pass++;
							offset=4;
						}
						break;
					}
				case 1:
					{
						offset+=8;
						if (offset >= height)
						{
							pass++;
							offset=2;
						}
						break;
					}
				case 2:
					{
						offset+=4;
						if (offset >= height)
						{
							pass++;
							offset=1;
						}
						break;
					}
				case 3:
					{
						offset+=2;
						break;
					}
				}
			}

			if (x < width)
				break;

			/*if (image->previous == (Image *) NULL)
			if (QuantumTick(y,image->rows))
			MagickMonitor(LoadImageText,y,image->rows);*/
		}
		delete pixel_stack;
		delete suffix;
		delete prefix;
		delete packet;

		delete colortable;

		//if (y < image->rows)
		//failed = true;

		Image* anImage = new Image();

		anImage->mWidth = width;
		anImage->mHeight = height;
		anImage->mBits = aBits;

		//TODO: Change for animation crap
		p_fclose(fp);
		return anImage;
	}

	p_fclose(fp);

	return NULL;
}

typedef struct my_error_mgr * my_error_ptr;

struct my_error_mgr
{
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);

}

// Original 0x49C9F0 (name unknown; LTCG passes the quality in EAX and the table set in ECX). Modelled on
// jpeg_add_quant_table, but blends two of the twelve preset tables instead of scaling one, and always
// clamps to 1..255. Only WriteJPEGImage calls it (quality 80 = table 9 exactly). For quality > 50 the
// upper table is raised to at least 11, so quality 100 would read a 13th table (weight 0) -- unreachable.
static void SetPresetQuantTable(j_compress_ptr cinfo, int which_tbl, const unsigned char theTables[][64], int theQuality)
{
	if (theQuality < 0)
		theQuality = 0;
	else if (theQuality > 100)
		theQuality = 100;

	/* Safety check to ensure start_compress not called yet. */
	if (cinfo->global_state != 100 /* CSTATE_START */)
		ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);

	if (cinfo->quant_tbl_ptrs[which_tbl] == NULL)
		cinfo->quant_tbl_ptrs[which_tbl] = jpeg_alloc_quant_table((j_common_ptr) cinfo);

	int aLowTable = theQuality / 10;
	int aHighWeight = (theQuality % 10) * 10;
	int aHighTable;
	if (theQuality > 50)
	{
		aLowTable++;
		aHighTable = aLowTable + 1;
		if (aHighTable <= 11)
			aHighTable = 11;
	}
	else
		aHighTable = aLowTable + 1;

	int aLowWeight = 100 - aHighWeight;
	for (int i = 0; i < DCTSIZE2; i++)
	{
		int aVal = (aHighWeight * theTables[aHighTable][i] + aLowWeight * theTables[aLowTable][i]) / 100;
		if (aVal <= 0)
			aVal = 1;
		else if (aVal > 255)
			aVal = 255;
		cinfo->quant_tbl_ptrs[which_tbl]->quantval[i] = (UINT16) aVal;
	}

	/* Initialize sent_table FALSE so table will be written to JPEG file. */
	cinfo->quant_tbl_ptrs[which_tbl]->sent_table = FALSE;
}

bool ImageLib::WriteJPEGImage(const std::string& theFileName, Image* theImage)
{
	int aQuality = 80;

	// Preset quantisation tables (natural coefficient order, 12 steps each). The original builds both sets as
	// locals on the stack on every call (0x49CC30-0x49F326: ebp-0x608 luminance, then ebp-0x308 chrominance),
	// before opening the file; the values are copied verbatim from those stores, including their irregular
	// entries (luminance 5/6 end in 0, 9 has a 1).
	unsigned char aLuminanceTables[12][64] =
	{
		{	// 0
			 20,  16,  25,  39,  50,  46,  62,  68,
			 16,  18,  23,  38,  38,  53,  65,  68,
			 25,  23,  31,  38,  53,  65,  68,  68,
			 39,  38,  38,  53,  65,  68,  68,  68,
			 50,  38,  53,  65,  68,  68,  68,  68,
			 46,  53,  65,  68,  68,  68,  68,  68,
			 62,  65,  68,  68,  68,  68,  68,  68,
			 68,  68,  68,  68,  68,  68,  68,  68,
		},
		{	// 1
			 20,  16,  25,  39,  50,  46,  62,  68,
			 16,  18,  23,  38,  38,  53,  65,  68,
			 25,  23,  31,  38,  53,  65,  68,  68,
			 39,  38,  38,  53,  65,  68,  68,  68,
			 50,  38,  53,  65,  68,  68,  68,  68,
			 46,  53,  65,  68,  68,  68,  68,  68,
			 62,  65,  68,  68,  68,  68,  68,  68,
			 68,  68,  68,  68,  68,  68,  68,  68,
		},
		{	// 2
			 18,  14,  14,  21,  30,  35,  34,  39,
			 14,  16,  16,  19,  26,  24,  30,  39,
			 14,  16,  17,  21,  24,  34,  46,  62,
			 21,  19,  21,  26,  33,  48,  62,  65,
			 30,  26,  24,  33,  51,  65,  65,  65,
			 35,  24,  34,  48,  65,  65,  65,  65,
			 34,  30,  46,  62,  65,  65,  65,  65,
			 39,  39,  62,  65,  65,  65,  65,  65,
		},
		{	// 3
			 16,  11,  11,  16,  23,  27,  31,  30,
			 11,  12,  12,  15,  20,  23,  23,  30,
			 11,  12,  13,  16,  23,  26,  35,  47,
			 16,  15,  16,  23,  26,  37,  47,  64,
			 23,  20,  23,  26,  39,  51,  64,  64,
			 27,  23,  26,  37,  51,  64,  64,  64,
			 31,  23,  35,  47,  64,  64,  64,  64,
			 30,  30,  47,  64,  64,  64,  64,  64,
		},
		{	// 4
			 12,   8,   8,  12,  17,  21,  24,  23,
			  8,   9,   9,  11,  15,  19,  18,  23,
			  8,   9,  10,  12,  19,  20,  27,  36,
			 12,  11,  12,  21,  20,  28,  36,  53,
			 17,  15,  19,  20,  30,  39,  51,  59,
			 21,  19,  20,  28,  39,  51,  59,  59,
			 24,  18,  27,  36,  51,  59,  59,  59,
			 23,  23,  36,  53,  59,  59,  59,  59,
		},
		{	// 5
			  8,   6,   6,   8,  12,  14,  16,  17,
			  6,   6,   6,   8,  10,  13,  12,  15,
			  6,   6,   7,   8,  13,  14,  18,  24,
			  8,   8,   8,  14,  13,  19,  24,  35,
			 12,  10,  13,  13,  20,  26,  34,  14,
			 13,  14,  19,  26,  34,  39,  39,  16,
			 12,  18,  24,  34,  39,  39,  39,  17,
			 15,  24,  35,  39,  39,  39,  39,   0,
		},
		{	// 6
			  8,   6,   6,   8,  12,  14,  16,  17,
			  6,   6,   6,   8,  10,  13,  12,  15,
			  6,   6,   7,   8,  13,  14,  18,  24,
			  8,   8,   8,  14,  13,  19,  24,  35,
			 12,  10,  13,  13,  20,  26,  34,  14,
			 13,  14,  19,  26,  34,  39,  39,  16,
			 12,  18,  24,  34,  39,  39,  39,  17,
			 15,  24,  35,  39,  39,  39,  39,   0,
		},
		{	// 7
			  6,   4,   4,   6,   9,  11,  12,  16,
			  4,   5,   5,   6,   8,  10,  12,  12,
			  4,   5,   5,   6,  10,  12,  14,  19,
			  6,   6,   6,  11,  12,  15,  19,  28,
			  9,   8,  10,  12,  16,  20,  27,  31,
			 11,  10,  12,  15,  20,  27,  31,  31,
			 12,  12,  14,  19,  27,  31,  31,  31,
			 16,  12,  19,  28,  31,  31,  31,  31,
		},
		{	// 8
			  4,   3,   3,   4,   6,   7,   8,  10,
			  3,   3,   3,   4,   5,   6,   8,  10,
			  3,   3,   3,   4,   6,   9,  12,  12,
			  4,   4,   4,   7,   9,  12,  12,  17,
			  6,   5,   6,   9,  12,  13,  17,  20,
			  7,   6,   9,  12,  13,  17,  20,  20,
			  8,   8,  12,  12,  17,  20,  20,  20,
			 10,  10,  12,  17,  20,  20,  20,  20,
		},
		{	// 9
			  2,   2,   2,   2,   3,   4,   5,   6,
			  2,   2,   2,   2,   3,   4,   5,   6,
			  2,   2,   2,   2,   4,   5,   7,   9,
			  2,   2,   2,   4,   5,   7,   9,  12,
			  3,   3,   4,   5,   8,  10,  12,   1,
			  4,   4,   5,   7,  10,  12,  12,  12,
			  5,   5,   7,   9,  12,  12,  12,  12,
			  6,   6,   9,  12,  12,  12,  12,  12,
		},
		{	// 10
			  1,   1,   1,   1,   2,   2,   2,   3,
			  1,   1,   1,   1,   2,   2,   2,   3,
			  1,   1,   1,   1,   2,   3,   4,   5,
			  1,   1,   1,   2,   3,   4,   5,   7,
			  2,   2,   2,   3,   4,   5,   7,   8,
			  2,   2,   3,   4,   5,   7,   8,   8,
			  2,   2,   4,   5,   7,   8,   8,   8,
			  3,   3,   5,   7,   8,   8,   8,   8,
		},
		{	// 11
			  1,   1,   1,   1,   1,   1,   1,   1,
			  1,   1,   1,   1,   1,   1,   1,   1,
			  1,   1,   1,   1,   1,   1,   1,   1,
			  1,   1,   1,   1,   1,   1,   2,   2,
			  1,   1,   1,   1,   1,   2,   2,   3,
			  1,   1,   1,   1,   2,   2,   3,   3,
			  1,   1,   1,   2,   2,   3,   3,   3,
			  1,   1,   2,   2,   3,   3,   3,   3,
		},
	};

	unsigned char aChrominanceTables[12][64] =
	{
		{	// 0
			 21,  25,  32,  38,  54,  68,  68,  68,
			 25,  28,  24,  38,  54,  68,  68,  68,
			 32,  24,  32,  43,  66,  68,  68,  68,
			 38,  38,  43,  53,  68,  68,  68,  68,
			 54,  54,  66,  68,  68,  68,  68,  68,
			 68,  68,  68,  68,  68,  68,  68,  68,
			 68,  68,  68,  68,  68,  68,  68,  68,
			 68,  68,  68,  68,  68,  68,  68,  68,
		},
		{	// 1
			 21,  25,  32,  38,  54,  68,  68,  68,
			 25,  28,  24,  38,  54,  68,  68,  68,
			 32,  24,  32,  43,  66,  68,  68,  68,
			 38,  38,  43,  53,  68,  68,  68,  68,
			 54,  54,  66,  68,  68,  68,  68,  68,
			 68,  68,  68,  68,  68,  68,  68,  68,
			 68,  68,  68,  68,  68,  68,  68,  68,
			 68,  68,  68,  68,  68,  68,  68,  68,
		},
		{	// 2
			 20,  19,  22,  27,  26,  33,  49,  62,
			 19,  25,  23,  22,  26,  33,  45,  56,
			 22,  23,  26,  29,  33,  39,  59,  65,
			 27,  22,  29,  36,  39,  51,  65,  65,
			 26,  26,  33,  39,  51,  62,  65,  65,
			 33,  33,  39,  51,  62,  65,  65,  65,
			 49,  45,  59,  65,  65,  65,  65,  65,
			 62,  56,  65,  65,  65,  65,  65,  65,
		},
		{	// 3
			 17,  15,  17,  21,  20,  26,  38,  48,
			 15,  19,  18,  17,  20,  26,  35,  43,
			 17,  18,  20,  22,  26,  30,  46,  53,
			 21,  17,  22,  28,  30,  39,  53,  64,
			 20,  20,  26,  30,  39,  48,  64,  64,
			 26,  26,  30,  39,  48,  63,  64,  64,
			 38,  35,  46,  53,  64,  64,  64,  64,
			 48,  43,  53,  64,  64,  64,  64,  64,
		},
		{	// 4
			 13,  11,  13,  16,  20,  20,  29,  37,
			 11,  14,  14,  14,  16,  20,  26,  32,
			 13,  14,  15,  17,  20,  23,  35,  40,
			 16,  14,  17,  21,  23,  30,  40,  50,
			 20,  16,  20,  23,  30,  37,  50,  59,
			 20,  20,  23,  30,  37,  48,  59,  59,
			 29,  26,  35,  40,  50,  59,  59,  59,
			 37,  32,  40,  50,  59,  59,  59,  59,
		},
		{	// 5
			  9,   8,   9,  11,  14,  17,  19,  24,
			  8,  10,   9,  11,  14,  13,  17,  22,
			  9,   9,  13,  14,  13,  15,  23,  26,
			 11,  11,  14,  14,  15,  20,  26,  33,
			 14,  14,  13,  15,  20,  24,  33,  39,
			 17,  13,  15,  20,  24,  32,  39,  39,
			 19,  17,  23,  26,  33,  39,  39,  39,
			 24,  22,  26,  33,  39,  39,  39,  39,
		},
		{	// 6
			  8,   9,  16,  29,  32,  38,  38,  38,
			  9,  14,  20,  26,  38,  38,  38,  38,
			 16,  20,  21,  38,  38,  38,  38,  38,
			 29,  26,  38,  38,  38,  38,  38,  38,
			 32,  38,  38,  38,  38,  38,  38,  38,
			 38,  38,  38,  38,  38,  38,  38,  38,
			 38,  38,  38,  38,  38,  38,  38,  38,
			 38,  38,  38,  38,  38,  38,  38,  38,
		},
		{	// 7
			  7,   7,  13,  24,  26,  31,  31,  31,
			  7,  12,  16,  21,  31,  31,  31,  31,
			 13,  16,  17,  31,  31,  31,  31,  31,
			 24,  21,  31,  31,  31,  31,  31,  31,
			 26,  31,  31,  31,  31,  31,  31,  31,
			 31,  31,  31,  31,  31,  31,  31,  31,
			 31,  31,  31,  31,  31,  31,  31,  31,
			 31,  31,  31,  31,  31,  31,  31,  31,
		},
		{	// 8
			  4,   5,   8,  15,  20,  20,  20,  20,
			  5,   7,  10,  14,  20,  20,  20,  20,
			  8,  10,  14,  20,  20,  20,  20,  20,
			 15,  14,  20,  20,  20,  20,  20,  20,
			 20,  20,  20,  20,  20,  20,  20,  20,
			 20,  20,  20,  20,  20,  20,  20,  20,
			 20,  20,  20,  20,  20,  20,  20,  20,
			 20,  20,  20,  20,  20,  20,  20,  20,
		},
		{	// 9
			  3,   3,   5,   9,  13,  15,  15,  15,
			  3,   4,   6,  11,  14,  12,  12,  12,
			  5,   6,   9,  14,  12,  12,  12,  12,
			  9,  11,  14,  12,  12,  12,  12,  12,
			 13,  14,  12,  12,  12,  12,  12,  12,
			 15,  12,  12,  12,  12,  12,  12,  12,
			 15,  12,  12,  12,  12,  12,  12,  12,
			 15,  12,  12,  12,  12,  12,  12,  12,
		},
		{	// 10
			  1,   1,   2,   5,   7,   8,   8,   8,
			  1,   2,   3,   5,   8,   8,   8,   8,
			  2,   3,   4,   8,   8,   8,   8,   8,
			  5,   5,   8,   8,   8,   8,   8,   8,
			  7,   8,   8,   8,   8,   8,   8,   8,
			  8,   8,   8,   8,   8,   8,   8,   8,
			  8,   8,   8,   8,   8,   8,   8,   8,
			  8,   8,   8,   8,   8,   8,   8,   8,
		},
		{	// 11
			  1,   1,   1,   2,   2,   3,   3,   3,
			  1,   1,   1,   2,   3,   3,   3,   3,
			  1,   1,   1,   3,   3,   3,   3,   3,
			  2,   2,   3,   3,   3,   3,   3,   3,
			  2,   3,   3,   3,   3,   3,   3,   3,
			  3,   3,   3,   3,   3,   3,   3,   3,
			  3,   3,   3,   3,   3,   3,   3,   3,
			  3,   3,   3,   3,   3,   3,   3,   3,
		},
	};

	FILE *fp;

	if ((fp = fopen(theFileName.c_str(), "wb")) == NULL)
		return false;

	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_compress(&cinfo);
		fclose(fp);
		return false;
	}

	jpeg_create_compress(&cinfo);

	cinfo.image_width = theImage->mWidth;
	cinfo.image_height = theImage->mHeight;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	// Original (0x49F3E0-0x49F449): optimized Huffman tables set after jpeg_set_defaults (which would
	// reset it), the preset quantisation tables instead of jpeg_set_quality, and no chroma subsampling
	jpeg_set_defaults(&cinfo);
	cinfo.optimize_coding = TRUE;
	jpeg_set_colorspace(&cinfo, JCS_YCbCr);
	SetPresetQuantTable(&cinfo, 0, aLuminanceTables, aQuality);
	SetPresetQuantTable(&cinfo, 1, aChrominanceTables, aQuality);
	if (aQuality > 50)
	{
		cinfo.comp_info[0].h_samp_factor = 1;
		cinfo.comp_info[0].v_samp_factor = 1;
	}

	jpeg_stdio_dest(&cinfo, fp);

	jpeg_start_compress(&cinfo, true);

	int row_stride = theImage->GetWidth() * 3;

	unsigned char* aTempBuffer = new unsigned char[row_stride];

	unsigned long* aSrcPtr = theImage->mBits;

	for (int aRow = 0; aRow < theImage->mHeight; aRow++)
	{
		unsigned char* aDest = aTempBuffer;

		for (int aCol = 0; aCol < theImage->mWidth; aCol++)
		{
			unsigned long src = *(aSrcPtr++);

			*aDest++ = (src >> 16) & 0xFF;
			*aDest++ = (src >>  8) & 0xFF;
			*aDest++ = (src      ) & 0xFF;
		}

		jpeg_write_scanlines(&cinfo, &aTempBuffer, 1);
	}

	delete [] aTempBuffer;

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	fclose(fp);

	return true;
}

bool ImageLib::WritePNGImage(const std::string& theFileName, Image* theImage)
{
	png_structp png_ptr;
	png_infop info_ptr;

	FILE *fp;

	if ((fp = fopen(theFileName.c_str(), "wb")) == NULL)
		return false;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	  NULL, NULL, NULL);

	if (png_ptr == NULL)
	{
		fclose(fp);
		return false;
	}

	// Allocate/initialize the memory for image information.  REQUIRED.
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return false;
	}

   // Set error handling if you are using the setjmp/longjmp method (this is
   // the normal method of doing things with libpng).  REQUIRED unless you
   // set up your own error handlers in the png_create_write_struct() earlier.

	if (setjmp(png_ptr->jmpbuf))
	{
		// Free all of the memory associated with the png_ptr and info_ptr
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		// If we get here, we had a problem writeing the file
		return NULL;
	}

	png_init_io(png_ptr, fp);

	png_color_8 sig_bit;
	sig_bit.red = 8;
	sig_bit.green = 8;
	sig_bit.blue = 8;
	/* if the image has an alpha channel then */
	sig_bit.alpha = 8;
	png_set_sBIT(png_ptr, info_ptr, &sig_bit);
	png_set_bgr(png_ptr);

	png_set_IHDR(png_ptr, info_ptr, theImage->mWidth, theImage->mHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA,
       PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Add filler (or alpha) byte (before/after each RGB triplet)
	//png_set_expand(png_ptr);
	//png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	//png_set_gray_1_2_4_to_8(png_ptr);
	//png_set_palette_to_rgb(png_ptr);
	//png_set_gray_to_rgb(png_ptr);

	png_write_info(png_ptr, info_ptr);

	for (int i = 0; i < theImage->mHeight; i++)
	{
		png_bytep aRowPtr = (png_bytep) (theImage->mBits + i*theImage->mWidth);
		png_write_rows(png_ptr, &aRowPtr, 1);
	}

	// write rest of file, and get additional chunks in info_ptr - REQUIRED
	png_write_end(png_ptr, info_ptr);

	// clean up after the write, and free any memory allocated - REQUIRED
	png_destroy_write_struct(&png_ptr, &info_ptr);

	// close the file
	fclose(fp);

	return true;
}

bool ImageLib::WriteTGAImage(const std::string& theFileName, Image* theImage)
{
	FILE* aTGAFile = fopen(theFileName.c_str(), "wb");
	if (aTGAFile == NULL)
		return false;

	BYTE aHeaderIDLen = 0;
	fwrite(&aHeaderIDLen, sizeof(BYTE), 1, aTGAFile);

	BYTE aColorMapType = 0;
	fwrite(&aColorMapType, sizeof(BYTE), 1, aTGAFile);
	
	BYTE anImageType = 2;
	fwrite(&anImageType, sizeof(BYTE), 1, aTGAFile);

	WORD aFirstEntryIdx = 0;
	fwrite(&aFirstEntryIdx, sizeof(WORD), 1, aTGAFile);

	WORD aColorMapLen = 0;
	fwrite(&aColorMapLen, sizeof(WORD), 1, aTGAFile);

	BYTE aColorMapEntrySize = 0;
	fwrite(&aColorMapEntrySize, sizeof(BYTE), 1, aTGAFile);	

	WORD anXOrigin = 0;
	fwrite(&anXOrigin, sizeof(WORD), 1, aTGAFile);

	WORD aYOrigin = 0;
	fwrite(&aYOrigin, sizeof(WORD), 1, aTGAFile);

	WORD anImageWidth = theImage->mWidth;
	fwrite(&anImageWidth, sizeof(WORD), 1, aTGAFile);	

	WORD anImageHeight = theImage->mHeight;
	fwrite(&anImageHeight, sizeof(WORD), 1, aTGAFile);	

	BYTE aBitCount = 32;
	fwrite(&aBitCount, sizeof(BYTE), 1, aTGAFile);	

	BYTE anImageDescriptor = 8 | (1<<5);
	fwrite(&anImageDescriptor, sizeof(BYTE), 1, aTGAFile);

	fwrite(theImage->mBits, 4, theImage->mWidth*theImage->mHeight, aTGAFile);

	fclose(aTGAFile);

	return true;
}

bool ImageLib::WriteBMPImage(const std::string& theFileName, Image* theImage)
{
	FILE* aFile = fopen(theFileName.c_str(), "wb");
	if (aFile == NULL)
		return false;

	BITMAPFILEHEADER aFileHeader;
	BITMAPINFOHEADER aHeader;

	memset(&aFileHeader,0,sizeof(aFileHeader));
	memset(&aHeader,0,sizeof(aHeader));

	int aNumBytes = theImage->mWidth*theImage->mHeight*4;

	aFileHeader.bfType = ('M'<<8) | 'B';
	aFileHeader.bfSize = sizeof(aFileHeader) + sizeof(aHeader) + aNumBytes;
	aFileHeader.bfOffBits = sizeof(aHeader); 

	aHeader.biSize = sizeof(aHeader);
	aHeader.biWidth = theImage->mWidth;
	aHeader.biHeight = theImage->mHeight;
	aHeader.biPlanes = 1;
	aHeader.biBitCount = 32;
	aHeader.biCompression = BI_RGB;

	fwrite(&aFileHeader,sizeof(aFileHeader),1,aFile);
	fwrite(&aHeader,sizeof(aHeader),1,aFile);
	DWORD *aRow = theImage->mBits + (theImage->mHeight-1)*theImage->mWidth;
	int aRowSize = theImage->mWidth*4;
	for (int i=0; i<theImage->mHeight; i++, aRow-=theImage->mWidth)
		fwrite(aRow,4,theImage->mWidth,aFile);

	fclose(aFile);
	return true;
}

////////////////////////////////////////////////////////////////////////// 
// JPEG Pak Reader

typedef struct {
	struct jpeg_source_mgr pub;	/* public fields */

	PFILE * infile;		/* source stream */
	JOCTET * buffer;		/* start of buffer */
	boolean start_of_file;	/* have we gotten any data yet? */
} pak_source_mgr;

typedef pak_source_mgr * pak_src_ptr;

#define INPUT_BUF_SIZE 4096

METHODDEF(void) init_source (j_decompress_ptr cinfo)
{
	pak_src_ptr src = (pak_src_ptr) cinfo->src;
	src->start_of_file = TRUE;
}

METHODDEF(boolean) fill_input_buffer (j_decompress_ptr cinfo)
{
	pak_src_ptr src = (pak_src_ptr) cinfo->src;
	size_t nbytes;

	nbytes = p_fread(src->buffer, 1, INPUT_BUF_SIZE, src->infile);
	//((size_t) fread((void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))

	if (nbytes <= 0) {
		if (src->start_of_file)	/* Treat empty input file as fatal error */
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

METHODDEF(void) skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	pak_src_ptr src = (pak_src_ptr) cinfo->src;

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			(void) fill_input_buffer(cinfo);
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

METHODDEF(void) term_source (j_decompress_ptr cinfo)
{
	/* no work necessary here */
}

void jpeg_pak_src (j_decompress_ptr cinfo, PFILE* infile)
{
	pak_src_ptr src;

	/* The source object and input buffer are made permanent so that a series
	* of JPEG images can be read from the same file by calling jpeg_stdio_src
	* only before the first one.  (If we discarded the buffer at the end of
	* one image, we'd likely lose the start of the next one.)
	* This makes it unsafe to use this manager and a different source
	* manager serially with the same JPEG object.  Caveat programmer.
	*/
	if (cinfo->src == NULL) {	/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			sizeof(pak_source_mgr));
		src = (pak_src_ptr) cinfo->src;
		src->buffer = (JOCTET *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			INPUT_BUF_SIZE * sizeof(JOCTET));
	}

	src = (pak_src_ptr) cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term_source;
	src->infile = infile;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}


Image* GetJPEGImage(const std::string& theFileName)
{
	PFILE *fp;

	if ((fp = p_fopen(theFileName.c_str(), "rb")) == NULL)
		return NULL;

	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
		p_fclose(fp);
		return 0;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_pak_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);
	int row_stride = cinfo.output_width * cinfo.output_components;

	unsigned char** buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	unsigned long* aBits = new unsigned long[cinfo.output_width*cinfo.output_height];
	unsigned long* q = aBits;

	if (cinfo.output_components==1)
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);

			unsigned char* p = *buffer;
			for (int i = 0; i < cinfo.output_width; i++)
			{
				int r = *p++;
				*q++ = 0xFF000000 | (r << 16) | (r << 8) | (r);
			}
		}
	}
	else
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);

			unsigned char* p = *buffer;
			for (int i = 0; i < cinfo.output_width; i++)
			{
				int r = *p++;
				int g = *p++;
				int b = *p++;

				*q++ = 0xFF000000 | (r << 16) | (g << 8) | (b);
			}
		}
	}

	Image* anImage = new Image();
	anImage->mWidth = cinfo.output_width;
	anImage->mHeight = cinfo.output_height;
	anImage->mBits = aBits;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	p_fclose(fp);

	return anImage;
}

#if 0
Image* GetJPEG2000Image(const std::string& theFileName)
{
	DWORD aTick = GetTickCount();

	static bool inited = false;
	if (!inited)
	{
		inited = true;
		jas_init();
	}

	jas_stream_t* aStream = jas_stream_fopen(theFileName.c_str(),"rb");
	if (!aStream)
		return NULL;

	// Read Image Header
	int aFormat = jas_image_getfmt(aStream);
	if (aFormat < 0)
	{
		jas_stream_close(aStream);
		return NULL;
	}

	jas_image_t* aJasImage = jas_image_decode(aStream,aFormat,NULL);
	if (!aJasImage)
	{
		jas_stream_close(aStream);
		return NULL;
	}

	DWORD aDecodeTime = GetTickCount() - aTick;

	int aNumComponents = jas_image_numcmpts(aJasImage);
	if (aNumComponents<1 || aNumComponents>4)
	{
		jas_stream_close(aStream);
		jas_image_destroy(aJasImage);
		return NULL;
	}

	int i;
	int aMaxWidth = 0;
	int aMaxHeight = 0;
	for (i=0; i<aNumComponents; i++)
	{
		int hstep = jas_image_cmpthstep(aJasImage,i);
		int numHSteps = jas_image_cmptwidth(aJasImage,i);

		int vstep = jas_image_cmptvstep(aJasImage,i);
		int numVSteps = jas_image_cmptheight(aJasImage,i);

		int aWidth = jas_image_cmpttlx(aJasImage,i) + hstep*numHSteps;
		int aHeight = jas_image_cmpttly(aJasImage,i) + vstep*numVSteps;

		if (aWidth > aMaxWidth)
			aMaxWidth = aWidth;

		if (aHeight > aMaxHeight)
			aMaxHeight = aHeight;
	}

	// Read Image Data
	Image *anImage = new Image;
	anImage->mWidth = aMaxWidth;
	anImage->mHeight = aMaxHeight;
	anImage->mBits = new unsigned long[aMaxWidth * aMaxHeight];
	memset(anImage->mBits,0,aMaxWidth * aMaxHeight*4);

	int aColorModel = jas_image_clrspc(aJasImage);

	for (i=0; i<aNumComponents; i++)
	{
		int hstep = jas_image_cmpthstep(aJasImage,i);
		int vstep = jas_image_cmptvstep(aJasImage,i);
		int numHSteps = jas_image_cmptwidth(aJasImage,i);
		int numVSteps = jas_image_cmptheight(aJasImage,i);
		int xorig = jas_image_cmpttlx(aJasImage,i);
		int yorig = jas_image_cmpttly(aJasImage,i);
		bool sign = jas_image_cmptsgnd(aJasImage,i)?true:false;

		jas_matrix_t* aMatrix = jas_matrix_create(1,numHSteps);
		if (!aMatrix)
		{
			delete anImage;
			jas_image_destroy(aJasImage);
			jas_stream_close(aStream);

			return NULL;
		}

		int aShift = 8 - jas_image_cmptprec(aJasImage,i);
		if (aShift<0)
		{
			delete anImage;
			jas_matrix_destroy(aMatrix);
			jas_image_destroy(aJasImage);
			jas_stream_close(aStream);

			return NULL;
		}

		unsigned long* destRow = anImage->mBits + yorig*numVSteps*anImage->GetWidth() + xorig*numHSteps;

		// color model
		int aComponentType = jas_image_cmpttype(aJasImage,i);
		int aColorType = JAS_IMAGE_CT_COLOR(aComponentType);

		switch (aColorType)
		{
			case JAS_IMAGE_CT_RGB_R: aShift += 16; break;
			case JAS_IMAGE_CT_RGB_G: aShift += 8; break;
			case JAS_IMAGE_CT_RGB_B: break;
			default: aShift += 24; break; 
		}

		for (int y=0; y<numVSteps; y++)
		{
			if (jas_image_readcmpt(aJasImage,i,0,y,numHSteps,1,aMatrix) )
			{
				delete anImage;
				jas_matrix_destroy(aMatrix);
				jas_image_destroy(aJasImage);
				jas_stream_close(aStream);

				return NULL;			
			}

			unsigned long* dest = destRow;
			for (int x=0; x<numHSteps; x++)
			{
				int aVal = jas_matrix_getv(aMatrix,x);
				if (sign)
					aVal = (unsigned char)(aVal + 128);

				aVal <<= aShift;

				unsigned long *destRowWriter = dest;
				for (int j=0; j<vstep; j++)
				{
					unsigned long *destWriter = destRowWriter;
					for (int k=0; k<hstep; k++)
						*destWriter++ |= aVal;

					destRowWriter += anImage->GetWidth();
				}

				dest += hstep;
			}
			destRow += vstep*anImage->GetWidth();
		}

		// release decoding matrix
		jas_matrix_destroy(aMatrix);
	}

	DWORD aReadTime = GetTickCount() - aTick;
	char aBuf[512];
	sprintf(aBuf,"%d %d\n",aDecodeTime,aReadTime);
	OutputDebugString(aBuf);


	if (aNumComponents < 4) // add 255 alpha
	{
		int aSize = anImage->GetWidth()*anImage->GetHeight();
		unsigned long *dest = anImage->mBits;
		for (i=0; i<aSize; i++)
			*dest++ |= 0xff000000;
	}

	jas_image_destroy(aJasImage);
	jas_stream_close(aStream);

	return anImage;
}
#else

#include "j2k-codec\j2k-codec.h"

HMODULE gJ2KCodec = NULL;
std::string gJ2KCodecKey = "Your registration here";

void ImageLib::InitJPEG2000()
{
	gJ2KCodec = ::LoadLibrary(_T("j2k-codec.dll"));
}

void ImageLib::CloseJPEG2000()
{
	if (gJ2KCodec != NULL)
	{
		::FreeLibrary(gJ2KCodec);
		gJ2KCodec = NULL;
	}
}

void ImageLib::SetJ2KCodecKey(const std::string& theKey)
{
	gJ2KCodecKey = theKey;
}

int __stdcall Pak_seek(void *data_source, int offset)
{
	return p_fseek((PFILE*) data_source, offset, SEEK_SET);
}

int __stdcall Pak_read(void *ptr, int size, void *data_source)
{
	return p_fread(ptr, 1, size, (PFILE*) data_source);
}

void __stdcall Pak_close(void *data_source)
{	
}

Image* GetJPEG2000Image(const std::string& theFileName)
{
	if (gJ2KCodec != NULL)
	{
		PFILE* aFP = p_fopen(theFileName.c_str(), "rb");
		if (aFP == NULL)
			return NULL;

		static int (__stdcall *fJ2K_getVersion)() = NULL;
		static void (__stdcall *fJ2K_Unlock)(const char*) = NULL;
		static void* (__stdcall *fJ2K_OpenCustom)(void*, J2K_Callbacks*) = NULL;
		static void* (__stdcall *fJ2K_OpenFile)(const char*) = NULL;
		static void (__stdcall *fJ2K_Close)(void*) = NULL;
		static int (__stdcall *fJ2K_GetInfo)(void*, int*, int*, int*) = NULL;
		static int (__stdcall *fJ2K_GetResolutionDimensions)(void*, int, int*, int*) = NULL;
		static int (__stdcall *fJ2K_Decode)(void*, unsigned char**, int*, char*, int*) = NULL;
		static int (__stdcall *fJ2K_getLastError)() = NULL;
		static const char* (__stdcall *fJ2K_getErrorStr)(int) = NULL;
		static bool loadFuncs = true;

		if (loadFuncs)
		{
			loadFuncs = false;
			*((void**)&fJ2K_getVersion) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_getVersion@0");
			*((void**)&fJ2K_Unlock) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_Unlock@4");
			*((void**)&fJ2K_OpenCustom) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_OpenCustom@8");
			*((void**)&fJ2K_OpenFile) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_OpenFile@4");
			*((void**)&fJ2K_Close) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_Close@4");
			*((void**)&fJ2K_GetInfo) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_GetInfo@16");
			*((void**)&fJ2K_GetResolutionDimensions) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_GetResolutionDimensions@16");
			*((void**)&fJ2K_Decode) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_Decode@20");
			*((void**)&fJ2K_getLastError) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_getLastError@0");
			*((void**)&fJ2K_getErrorStr) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_getErrorStr@4");

			// j2k guys didn't use declare the export names. yay! now we have to update these mangled names any time the DLL changes.

			if (!(fJ2K_getVersion != NULL && 
				  fJ2K_Unlock != NULL && 
				  fJ2K_OpenCustom != NULL && 
				  fJ2K_OpenFile != NULL && 
				  fJ2K_Close != NULL && 
				  fJ2K_GetInfo != NULL && 
				  fJ2K_GetResolutionDimensions != NULL && 
				  fJ2K_Decode != NULL &&
				  fJ2K_getLastError != NULL &&
				  fJ2K_getErrorStr != NULL))
			{
				CloseJPEG2000();
				return NULL;
			}

			int aJ2kVer = (*fJ2K_getVersion)();
			if (aJ2kVer < 0x120000) 
			{
				CloseJPEG2000();
				return NULL;
			}

			(*fJ2K_Unlock)(gJ2KCodecKey.c_str()); 
		}

		J2K_Callbacks aCallbacks;
		aCallbacks.read = Pak_read;
		aCallbacks.seek = Pak_seek;
		aCallbacks.close = Pak_close;

		//theFileName.c_str()
		void* aJ2KImage = (*fJ2K_OpenCustom)(aFP, &aCallbacks);
		if (aJ2KImage == NULL)
		{
			int anErrNum = (*fJ2K_getLastError)();
			std::string anErrorMessage = (*fJ2K_getErrorStr)(anErrNum);
			return NULL;
		}

		int aWidth, aHeight, aComponents;
		if ((*fJ2K_GetInfo)(aJ2KImage, &aWidth, &aHeight, &aComponents) != J2KERR_SUCCESS) 
		{
			(*fJ2K_Close)(aJ2KImage);
			return NULL;
		}

		(*fJ2K_GetResolutionDimensions)(aJ2KImage, 0, &aWidth, &aHeight);
		
		unsigned long* aBuffer = new unsigned long[aWidth*aHeight];
		if (aBuffer == NULL)
		{
			(*fJ2K_Close)(aJ2KImage);
			return NULL;
		}

		char anOptsBuffer[32];
		strcpy(anOptsBuffer, "bpp=4,rl=0");

		int aSize = aWidth*aHeight*4;
		int aPitch = aWidth*4;
		if ((*fJ2K_Decode)(aJ2KImage, (unsigned char**)&aBuffer, &aSize, anOptsBuffer, &aPitch) != J2KERR_SUCCESS)
		{
			(*fJ2K_Close)(aJ2KImage);
			delete[] aBuffer;
			return NULL;
		}
		(*fJ2K_Close)(aJ2KImage);

		ImageLib::Image* anImage = new ImageLib::Image;
		anImage->mBits = aBuffer;
		anImage->mWidth = aWidth;
		anImage->mHeight = aHeight;

		// The original (0x49FE72-0x49FE9B) forces the alpha to opaque unconditionally; it never reads
		// gIgnoreJPEG2000Alpha (VS2005 LTCG does not fold never-written globals, cf. gMerylSpeechFadeTime)
		DWORD *aPtr = anImage->mBits;
		DWORD *anEnd = aPtr+anImage->mWidth*anImage->mHeight;
		for (; aPtr!=anEnd; ++aPtr)
			*aPtr |= 0xFF000000;

		p_fclose(aFP);

		return anImage;
	}
	return NULL;
}

#endif


int ImageLib::gAlphaComposeColor = 0xFFFFFF;
// Kept for ImageLib.h; the original's GetImage and GetJPEG2000Image never read these two (see there)
bool ImageLib::gAutoLoadAlpha = true;
bool ImageLib::gIgnoreJPEG2000Alpha = true;

Image* ImageLib::GetImage(const std::string& theFilename, bool lookForAlphaImage)
{
	// The original (0x49FED0) has no gAutoLoadAlpha test: it starts with the rfind('.') call
	int aLastDotPos = theFilename.rfind('.');
	int aLastSlashPos = max((int)theFilename.rfind('\\'), (int)theFilename.rfind('/'));

	std::string anExt;
	std::string aFilename;

	if (aLastDotPos > aLastSlashPos)
	{
		anExt = theFilename.substr(aLastDotPos, theFilename.length() - aLastDotPos);
		aFilename = theFilename.substr(0, aLastDotPos);
	}
	else
		aFilename = theFilename;

	Image* anImage = NULL;

	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".tga") == 0) || (anExt.length() == 0)))
		 anImage = GetTGAImage(aFilename + ".tga");

	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".jpg") == 0) || (anExt.length() == 0)))
		 anImage = GetJPEGImage(aFilename + ".jpg");

	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".png") == 0) || (anExt.length() == 0)))
		 anImage = GetPNGImage(aFilename + ".png");

	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".gif") == 0) || (anExt.length() == 0)))
		 anImage = GetGIFImage(aFilename + ".gif");

	if (anImage == NULL && (stricmp(anExt.c_str(), ".j2k") == 0 || anExt.length() == 0))
		anImage = GetJPEG2000Image(aFilename + ".j2k");
	if (anImage == NULL && (stricmp(anExt.c_str(), ".jp2") == 0 || anExt.length() == 0))
		anImage = GetJPEG2000Image(aFilename + ".jp2");


	// Check for alpha images
	Image* anAlphaImage = NULL;
	if(lookForAlphaImage)
	{
		// Check _ImageName
		anAlphaImage = GetImage(theFilename.substr(0, aLastSlashPos+1) + "_" +
			theFilename.substr(aLastSlashPos+1, theFilename.length() - aLastSlashPos - 1), false);

		// Check ImageName_ (original 0x4A05AC: built from the extension-less name)
		if(anAlphaImage==NULL)
			anAlphaImage = GetImage(aFilename + "_", false);
	}



	// Compose alpha channel with image
	if (anAlphaImage != NULL) 
	{
		if (anImage != NULL)
		{
			if ((anImage->mWidth == anAlphaImage->mWidth) &&
				(anImage->mHeight == anAlphaImage->mHeight))
			{
				unsigned long* aBits1 = anImage->mBits;
				unsigned long* aBits2 = anAlphaImage->mBits;
				int aSize = anImage->mWidth*anImage->mHeight;

				for (int i = 0; i < aSize; i++)
				{
					*aBits1 = (*aBits1 & 0x00FFFFFF) | ((*aBits2 & 0xFF) << 24);
					++aBits1;
					++aBits2;
				}
			}

			delete anAlphaImage;
		}
		else if (gAlphaComposeColor==0xFFFFFF)
		{
			anImage = anAlphaImage;

			unsigned long* aBits1 = anImage->mBits;

			int aSize = anImage->mWidth*anImage->mHeight;
			for (int i = 0; i < aSize; i++)
			{
				*aBits1 = (0x00FFFFFF) | ((*aBits1 & 0xFF) << 24);
				++aBits1;
			}
		}
		else
		{
			const int aColor = gAlphaComposeColor;
			anImage = anAlphaImage;

			unsigned long* aBits1 = anImage->mBits;

			int aSize = anImage->mWidth*anImage->mHeight;
			for (int i = 0; i < aSize; i++)
			{
				*aBits1 = aColor | ((*aBits1 & 0xFF) << 24);
				++aBits1;
			}
		}
	}

	return anImage;
}
