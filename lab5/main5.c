// Main program file MAIN5.C
// Written by Daniel W. Lewis
// Revised Jan 2, 2015
//
// Purpose: Create double size and half size versions of an image.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "bmp3.h"

IMAGE	*HalfSize(IMAGE *image) ;
IMAGE	*DoubleSize(IMAGE *image) ;

typedef struct
	{
	char	*format ;
	IMAGE	*(*function)() ;
	double	param ;
	} OPTION ;

OPTION	*GetOption(int argc, char **argv, OPTION options[]) ;
void	DisplayImage(char *filespec, char *format, ...) ;

int main(int argc, char **argv)
	{
	char *src_filespec, *dst_filespec ;
	static OPTION options[] =
		{
		{"Half Size",			HalfSize,			  0.0},
		{"Double Size",			DoubleSize,			  0.0},
		{NULL,					NULL,				  0.0}
		} ;
	OPTION *option ;
	IMAGE *image ;

	if (argc != 3 && argc != 4)
		{
		fprintf(stderr, "Usage: %s src-file dst-file {option#}\n", argv[0]) ;
		exit(255) ;
		}

	src_filespec = argv[1] ;
	dst_filespec = argv[2] ;

	image = ReadBMP24(src_filespec) ;

	if ((option = GetOption(argc, argv, options))== NULL)
		{
		fprintf(stderr, "Usage: %s src-file dst-file {option#}\n", argv[0]) ;
		exit(255) ;
		}

	image = (option->function)(image, option->param) ;

	WriteBMP24(dst_filespec, image) ;
	FreeImage(image) ;
	DisplayImage(dst_filespec, option->format, option->param) ;

	return 0 ;
	}

IMAGE *DoubleSize(IMAGE *src)
    {
    int i, j, m = 0, n = 0;
    IMAGE *newimage;
    newimage = NewImage(src->rows*2, src->cols*2);
    for(i = 0; i < src->rows; i++)
        {
        for(j = 0; j < src->cols; j++)
            {
            newimage->pxlrow[m][n].red = newimage->pxlrow[m][n + 1].red = newimage->pxlrow[m + 1][n].red = newimage->pxlrow[m+1][n+1].red = src->pxlrow[i][j].red;
            newimage->pxlrow[m][n].blu = newimage->pxlrow[m][n + 1].blu = newimage->pxlrow[m + 1][n].blu = newimage->pxlrow[m+1][n+1].blu = src->pxlrow[i][j].blu;
            newimage->pxlrow[m][n].grn = newimage->pxlrow[m][n + 1].grn = newimage->pxlrow[m + 1][n].grn = newimage->pxlrow[m+1][n+1].grn = src->pxlrow[i][j].grn;
            n += 2;
            }
        n = 0;
        m += 2;
        }
    return newimage;
    }

IMAGE *HalfSize(IMAGE *src)
    {
    int i, j, m = 0, n = 0;
    IMAGE *newimage;
    newimage = NewImage((src->rows/2), (src->cols/2));
    for(i = 0; i < newimage->rows; i++)
        {
        for(j = 0; j < newimage->cols; j++)
            {
            newimage->pxlrow[i][j].red = (src->pxlrow[m][n].red + src->pxlrow[m][n + 1].red + src->pxlrow[m + 1][n].red + src->pxlrow[m+1][n+1].red)/4;
            newimage->pxlrow[i][j].blu = (src->pxlrow[m][n].blu + src->pxlrow[m][n + 1].blu + src->pxlrow[m + 1][n].blu + src->pxlrow[m+1][n+1].blu)/4;
            newimage->pxlrow[i][j].grn = (src->pxlrow[m][n].grn + src->pxlrow[m][n + 1].grn + src->pxlrow[m + 1][n].grn + src->pxlrow[m+1][n+1].grn)/4;
            n += 2;
            }
        n = 0;
        m += 2;
        }
    return newimage;
    }

OPTION *GetOption(int argc, char **argv, OPTION options[])
	{
	unsigned number, index = 0 ;

	// count number of options
	for (number = 0; options[number].function != NULL; number++) ;

	if (argc == 4)
		{
		sscanf(argv[3], "%u", &index) ;
		}
	else
		{
		printf("\nOptions:\n\n") ;
		for (index = 0; index < number; index++)
			{
			char title[100] ;
			sprintf(title, options[index].format, options[index].param) ;
			printf("%2d: %s\n", index + 1, title) ;
			}

		printf("\nOption? ") ;
		scanf("%u", &index) ;
		}

	return (1 <= index && index <= number) ? &options[index - 1] : NULL ;
	}

void DisplayImage(char *filespec, char *format, ...)
	{
	char progspec[200], command[1000], *program_files, title[100] ;
	struct stat filestat ;
	va_list args ;
	
	va_start(args, format) ;
	vsprintf(title, format, args) ;
	va_end(args) ;

#if defined(_WIN32)
	program_files = getenv("ProgramFiles(x86)") ;
	if (program_files == NULL) program_files = getenv("C:\\Program Files") ;
	if (program_files == NULL) program_files = "" ;

	sprintf(progspec, "%s\\IrfanView\\i_view32.exe", program_files) ;
	if (stat(progspec, &filestat) == 0 && (filestat.st_mode & S_IFREG) != 0)
		{
		sprintf(command, "\"%s\" %s /hide=7 /title=%s", progspec, filespec, title) ;
		}
	else sprintf(command, "mspaint \"%s\"\n", filespec) ;
#elif defined(__unix)
	sprintf(command, "qiv \"%s\"", filespec) ;
#elif defined(__APPLE__)
	sprintf(command, "open \"%s\"", filespec) ;
#endif 
	system(command) ;
	}

