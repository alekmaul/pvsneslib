/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2021
		Alekmaul 

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

    Convert binary to text file

---------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BIN2TXTVERSION __BUILD_VERSION
#define BIN2TXTDATE __BUILD_DATE


//// M A I N   V A R I A B L E S ////////////////////////////////////////////////
int	convformat=1;			//convertion formation (1=c , 2 = asm)
int quietmode=0;			// 0 = not quiet, 1 = i can't say anything :P
FILE *fpi,*fpo;				// input and output file handler
unsigned int filesize;		// input file size
char filebase[256]="";		// input filename
char filename[256];			// output filename

//////////////////////////////////////////////////////////////////////////////
void PrintOptions(char *str)
{
	printf("\n\nUsage : bin2txt [options] filename ...");
	printf("\n  where filename is a binary file");
	
	if(str[0]!=0)
		printf("\n\nbin2txt: error 'The [%s] parameter is not recognized'",str);
	
	printf("\n\nConvert options:");
	printf("\n-cc               Output in c style format");
	printf("\n-ca               Output in assembly style format");
	printf("\n\nMisc options:");
    printf("\n-h                Display this information");
	printf("\n-q                Quiet mode");
    printf("\n-v                Display constify version information");
	printf("\n");
	
} //end of PrintOptions()

//////////////////////////////////////////////////////////////////////////////
void PrintVersion(void)
{
	printf("\n\nbin2txt.exe ("BIN2TXTDATE") version "BIN2TXTVERSION"");
    printf("\nCopyright (c) 2012-2021 Alekmaul\n");
}

/*
#ifndef HAVE_STRUPR
char* strupr(char* s)
{
	char* tmp = s;

	for (;*tmp;++tmp) {
		*tmp = toupper((unsigned char) *tmp);
	}

	return s;
}
#endif
*/

/// M A I N ////////////////////////////////////////////////////////////

#include <math.h>

int main(int argc, char **argv)
{
	int i;
	unsigned char bytei;

	
	//parse the arguments
	for(i=1;i<argc;i++)
	{
		if(argv[i][0]=='-')
		{
			if(argv[i][1]=='v') // show version
            {
                PrintVersion();
                exit( 0 );
            }
			else if(argv[i][1]=='h') // show help
            {
                PrintOptions((char *) "");
                exit( 0 );
            }
			else if(argv[i][1]=='c') // convert options
			{
				if( strcmp(&argv[i][1],"cc") == 0)	//packed pixels
				{
					convformat=1;
				}
				else if( strcmp(&argv[i][1],"ca") == 0)	// lzss compressed pixels
				{
					convformat=2;
				}
				else
				{
					PrintOptions(argv[i]);
					exit( 1 );
				}
			}
			else if(argv[i][1]=='q') //quiet mode
			{
				quietmode=1;
			}
			else //invalid option
			{
				PrintOptions(argv[i]);
				exit( 1 );
			}
		}
		else
		{
			//its not an option flag, so it must be the filebase
			if(filebase[0]!=0) // if already defined... there's a problem
			{
				PrintOptions(argv[i]);
				exit( 1 );
			}
			else
				strcpy(filebase,argv[i]);
		}
	}

	//make sure options are valid
	if( filebase[0] == 0 )
	{
		printf("\nbin2txt: error 'You must specify a base filename'");
		PrintOptions("");
		exit( 1 );
	}

	// open the file
	fpi = fopen(filebase,"rb");
	if(fpi==NULL)
	{
		printf("\nbin2txt: error 'Can't open file [%s]'",filebase);
		exit( 1 );
	}
	if (filebase[strlen(filebase)-4] == '.') {
		filebase[strlen(filebase)-4] = '\0';
	}

	//Print what the user has selected
	if (quietmode == 0) {
		if(convformat == 1)
			printf("\nbin2txt: 'C Style format=ON'\n");
		else
			printf("\nbin2txt: 'ASM style format=ON'\n");
	}

	// get filesize
	fseek(fpi, 0, SEEK_END);
	filesize = ftell(fpi);
	fseek(fpi, 0, SEEK_SET);

	// open output file for writing
	if (convformat == 1)
		sprintf(filename,"%s.h",filebase);
	else if (convformat == 2)
		sprintf(filename,"%s.asm",filebase);

	if (quietmode == 0)
		printf("bin2txt: 'Saving text file: [%s]\n",filename);
	fpo = fopen(filename,"wb");
	if(fpo==NULL)
	{	
		printf("bin2txt: error 'Can't open file [%s] for writing'\n",filename);
		fclose(fpi);
		return 1;
	}

	// write header & content & footer
	if (convformat == 1) {
		// write asm header
		fprintf(fpo, "//----------------------------------------------------------------------\n");
		fprintf(fpo, "// bin2txt converted binary data\n");
		fprintf(fpo, "// binary file size : %d bytes\n",filesize);
		fprintf(fpo, "//----------------------------------------------------------------------\n\n");
		strupr(filebase);
		fprintf(fpo, "#ifndef %s_INC_\n", filebase);
		fprintf(fpo, "#define %s_INC_\n\n", filebase);
		fprintf(fpo, "#define %s_SIZE 0x%x\n\n", filebase,filesize);
		fprintf(fpo, "unsigned char %s[%d]={\n", filebase,filesize);
		
		// write const
		for(i = 0; i < filesize; i++) {
			if(i) {
				if((i & 31) == 0)
					fprintf(fpo, ",\n");
				else
					fprintf(fpo, ", ");
			}
			fread(&bytei, 1, 1, fpi);
			fprintf(fpo, "0x%02X", bytei);
		}
	
		// write footer
		fprintf(fpo, "\n};\n\n");
		fprintf(fpo, "#endif\n");
	}
	else if (convformat == 2) {
		// write asm header
		fprintf(fpo, ";----------------------------------------------------------------------\n");
		fprintf(fpo, "; bin2txt converted binary data\n");
		fprintf(fpo, "; binary file size : %d bytes\n",filesize);
		fprintf(fpo, ";----------------------------------------------------------------------\n\n");
		fprintf(fpo, ".SECTION \".%s\" SUPERFREE\n\n", filebase);
		strupr(filebase);
		fprintf(fpo, ".define %s_SIZE %xh\n\n", filebase,filesize);
		fprintf(fpo, "%s:\n", filebase);
		
		// write const
		for(i = 0; i < filesize; i++) {
			if(i) {
				if((i & 31) == 0)
					fprintf(fpo, "\n\t.byte ");
				else
					fprintf(fpo, ", ");
			}
			else
				fprintf(fpo, "\t.byte ");
			fread(&bytei, 1, 1, fpi);
			fprintf(fpo, "$%02X", bytei);
		}
	
		// write footer
		fprintf(fpo, "\n\n%s_end:\n\n", filebase);
		fprintf(fpo, ".ENDS\n");
	}
		
	// close the input & output file
	fclose(fpi);
	fclose(fpo);

	if (quietmode == 0)
		printf("bin2txt: Done 'File converted'\n");

	return 0;
}
