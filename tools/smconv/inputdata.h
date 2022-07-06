#ifndef __INPUTDATA_H
#define __INPUTDATA_H

// param parser

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "basetypes.h"

namespace ConversionInput {
/*
	class EffectData {
	public:
		EffectData( const TiXmlElement * );
		std::string filename;
		std::string id;
		
		int force_filter;
		
	};
*/
	/*
	class SampleData {

	public:
		int index;
		int force_filter;
		int force_loop_filter;
		std::string id;
	
		SampleData( const TiXmlElement * );
		SampleData( const EffectData & );
	};
*/
	/*
	class ModuleData {

	private:
		u8 ConvertBitString( const char * );
		u8 TranslatePercentage( int );
		
	public:

		ModuleData( const TiXmlElement *source );
		~ModuleData();
		std::string filename;
		std::string id;
		
		u8	EDL;
		u8	EFB;
		u8	EVL;
		u8	EVR;
		u8	EON;
		u8	COEF[8];
		
		std::vector<SampleData*> samples;
	};
*/
	/*
	typedef struct {

		std::vector<const char *> files;
		bool hirom;
		std::string output;
	} SoundbankInfo;

	class SoundbankData {

	public:
		
		SoundbankData( const SoundbankInfo &info );
		
		std::string output;
		bool hirom;
		
		std::vector<ModuleData*> modules;
		std::vector<EffectData*> effects;
	};
*/
	class OperationData {

	private:
		

	public:

		~OperationData();

		OperationData( int argc, char *argv[] );

		std::string output;
		bool hirom;
		std::vector<const char *> files;
		bool spc_mode;
		bool verbose_mode;
		bool show_help;
        bool show_version;
		bool check_effect_size;
		int banknumber;

	};
}

#endif
