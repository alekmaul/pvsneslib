/******************************************************************************
 * snesmod converter
 * (C) 2009 Mukunda Johnson
 ******************************************************************************/
 
#include <string>
#include <stdio.h>
#include "inputdata.h"
#include "itloader.h"
#include "it2spc.h"

//DEFINES
#define SMCONVVERSION __BUILD_VERSION
#define SMCONVDATE __BUILD_DATE

const char USAGE[] = {
	"\n==========================================================="
	"\n---                  smconv v"SMCONVVERSION" "SMCONVDATE"             ---"
	"\n-----------------------------------------------------------"
	"\n             (c) 2012 Mukunda Johnson & Alekmaul "
	"\nBased on SNESMOD (C) 2009 Mukunda Johnson (www.mukunda.com)"
	"\n===========================================================\n"
	"\nUsage : smconv [options] [input]\n"
	"\nOptions are:"
	"\n\n--- Soundbank options ---"
	"\n-s                Soundbank creation mode."
	"\n                  (Can specify multiple files with soundbank mode.)"
	"\n                  (Otherwise specify only one file for SPC creation.)"
	"\n                  (Default is SPC creation mode)"
	"\n-b#               Bank number specification (Default is 5)."
	"\n\n--- File options ---"
	"\n-o [file]         Specify output file or file base."
	"\n                  (Specify SPC file for -s option)"
	"\n                  (Specify filename base for soundbank creation)"
	"\n                  (Required for soundbank mode)\n"
	"\n\n--- Memory options ---"
	"\n-i                Use HIROM mapping mode for soundbank."
	"\n\n--- Misc options ---"
	"\n-v                Enable verbose output."
	"\n-h                Show Help"
	"\n\n--- Tips ---"
	"\nTypical options to create soundbank for project:"
	"\n  smconv -s -o build/soundbank -h input1.it input2.it"
	"\n\nAnd for IT->SPC:"
	"\n  smconv input.it"
	"\n\nUse -v to view how much RAM the modules will use.\n"
};

std::string PATH;
bool VERBOSE;
int BANKNUM;

int main( int argc, char *argv[] ) {
	
	ConversionInput::OperationData od( argc, argv );

	VERBOSE = od.verbose_mode;
	BANKNUM = od.banknumber;
	
	if( argc < 2 ) {
		od.show_help = true;
	}

	if( od.show_help ) {
		printf( USAGE );
		return 0;
	}

	if( od.output.empty() ) {
		printf( "\nERROR : Missing output file." );
		return 0;
	}

	if( VERBOSE )
		printf( "\nLoading modules..." );

	ITLoader::Bank bank( od.files );

	if( VERBOSE )
		printf( "\nStarting conversion..." );

	IT2SPC::Bank result( bank, od.hirom );
	
	// export products
	if( od.spc_mode ) {
		result.MakeSPC( od.output.c_str() );
	} else {
		result.Export( od.output.c_str() );
	}
	
	return 0;
}
