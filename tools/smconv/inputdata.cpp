#include "inputdata.h"

extern std::string PATH;

namespace ConversionInput {


	template<typename T> static void deletePtrVector( std::vector<T*> &vecs ) {
		
		for(typename std::vector<T*>::iterator iter = vecs.begin(), ending = vecs.end();
			iter != ending; 
			iter++ ) {
		
			if( *iter )
				delete (*iter);
		}
		
		vecs.clear();
	}

	enum {
		ARG_OPTION,
		ARG_INPUT,
		ARG_INVALID
	};
	
	static int get_arg_type( const char *str ) {
		if( str[0] ) {
			if( str[0] == '-' ) {
				return ARG_OPTION;
			} else {
				return ARG_INPUT;
			}
		} else {
			return ARG_INVALID;
		}
	}

	static bool strmatch( const char *source, const char *test ) {
		for( int i = 0; test[i]; i++ )
			if( source[i] != test[i] )
				return false;
		return true;
	}
	
	OperationData::OperationData( int argc,  char *argv[] ) {

		spc_mode = true;
		show_help = false;
		hirom = false;
		verbose_mode = false;
		check_effect_size = false;
		banknumber = 5;
		
		// search for params
		for( int arg = 1; arg < argc; ) {
			int ptype = get_arg_type( argv[arg] );
			switch( ptype ) {
				case ARG_INVALID:
					// nothing
					arg++;
					break;
				case ARG_OPTION:
					// long command

#define TESTARG2(str1,str2) (strmatch( argv[arg], str1 ) || strmatch( argv[arg], str2 ))

					if( TESTARG2( "--soundbank", "-s" ) ) {
						
						spc_mode = false;
					} else if( TESTARG2( "--output", "-o" ) ) {

						arg++;
						if( arg == argc ) {
							printf( "\nERROR: No output file specified." );
							return;
						}
						output = argv[arg];

					} else if( TESTARG2( "--hirom", "-i" )) {
						hirom = true;
					} else if( TESTARG2( "--verbose", "-v" )) {
						verbose_mode = true;
					} else if( TESTARG2( "--help", "-h" )) {
						show_help = true;
					} else if( TESTARG2( "--effectsize", "-f" )) {
						check_effect_size = true;
					} else if( strmatch( argv[arg], "-b" ) ) {
						arg++;
						if( arg == argc ) {
							printf( "\nERROR: No bank number specified." );
							return;
						}
						if (isdigit(argv[arg][0])) {
							banknumber = atoi(argv[arg]);
						}
						else {
							printf( "\nERROR: Incorrect bank number." );
							return;
						}
					}
					arg++;
					break;
				case ARG_INPUT:
					// input

					files.push_back( argv[arg] );
					arg++;
					break;
			}
		}

		if( spc_mode ) {
			if( output.empty() ) {
				if( !files.empty() ) {
					output = files[0];
					int extstart = output.find_last_of('.');
					output = output.substr( 0 ,extstart );
					output += ".spc";
				}
			}
		}
		
	}
	
	OperationData::~OperationData() {
		
	}
/*
	void OperationData::GenerateFromXML( const TiXmlDocument *doc ) {
		
		for( const TiXmlNode *child = doc->FirstChild(); 
			 child; 
			 child = child->NextSibling() ) {

			if( child->Type() == TiXmlNode::ELEMENT ) {
				if( child->ValueTStr() == "soundbank" ) {
					
					targets.push_back( new SoundbankData( child->ToElement() ) );
				}
			}
		}
	}
*/
	/*
	SoundbankData::SoundbankData( const TiXmlElement *source ) {

		// search for attributes
		output = source->Attribute( "output" );
		hirom = (std::string)source->Attribute( "map" ) == "HiROM";
		spcpath = source->Attribute( "spc" );
		
	//	if( output.empty() ) {
	//		printf( "Missing output attribute, !!! fatal error !!!\n" );
	//		return;
	//	}

		output = PATH + output;
		
		for( const TiXmlNode *child = source->FirstChild();
			 child;
			 child = child->NextSibling() ) {
				
			if( child->Type() == TiXmlNode::ELEMENT ) {

				if( child->ValueTStr() == "module" ) {
					modules.push_back( new ModuleData(child->ToElement()) );
				} else if( child->ValueTStr() == "effect" ) {
					effects.push_back( new EffectData(child->ToElement()) );
				} else {
					printf( "Unknown soundbank member... %s\n", child->ValueTStr().c_str() );
				}
			}
			
		}
	}
	
	u8 ModuleData::ConvertBitString( const char *data ) {
		u8 value = 0;
		for( u32 i = 0; i < 8; i++ ) {
			if( !data[i] ) {
				return value;
			}
			if( data[i] == '1' ) {
				value |= (1<<i);
			}
		}
		return value;
	}
	
//	u8 ModuleData::TranslatePercentage( const TiXmlAttribute *v ) {
//		std::string s = v->Value(); 
//		if( s[s.length()-1] == '%' ) {
//			
//		} else {
//
//		}
//		if( v->Value value[value.length()-1] == '%' ) {
//			return (value. * 127) / 100;
//		else
//			return (value
//	}
	
	static const char *coef_names[] = { 
		"e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" 
	};

	ModuleData::ModuleData( const TiXmlElement *source ) {

		// set default attributes
		EDL = 0;
		EFB = 0;
		EVL = 0;
		EVR = 0;
		EON = 0;
		COEF[0] = 127;
		for( int i = 1; i < 8; i++ )
			COEF[i] = 0;

		// search for attributes
		for( const TiXmlAttribute *a = source->FirstAttribute();
			 a;
			 a = a->Next() ) {
			
			std::string name = a->Name();

			if( name == "file" ) filename = PATH + a->Value();
			else if( name == "id" ) {
				id = a->Value();
				ValidateID( id );
			} else if( name == "edl" ) EDL = a->IntValue();
			else if( name == "efb" ) EFB = a->IntValue();
			else if( name == "evl" ) EVL = a->IntValue();
			else if( name == "evr" ) EVR = a->IntValue();
			else if( name == "eon" ) EON = a->IntValue();
			else {
	
				bool f = false;
				for( u32 i = 0; i < 8; i++ ) {
					if( name == coef_names[i] ) {
						COEF[i] = a->IntValue();
						f = true;
						break;
					}
				}
				if( !f ) {
					printf( "Encountered unknown module attribute.\n" );
				}
			}
		}

		if( id.empty() ) {
			id = Path2ID( "MOD_", filename );
		}

		for( const TiXmlNode *child = source->FirstChild();
			 child;
			 child = child->NextSibling() ) {
				
			if( child->Type() == TiXmlNode::ELEMENT ) {

				if( child->ValueTStr() == "sample" ) {
					samples.push_back( new SampleData( child->ToElement() ) );
				}
			}
			
		}
	}

	ModuleData::~ModuleData() {
		deletePtrVector( samples );
	}
	
	SampleData::SampleData( const TiXmlElement *source ) {

		// set default attributes
		index = -1;
		force_filter = -1;
		force_loop_filter = -1;

		// search for attributes
		for( const TiXmlAttribute *a = source->FirstAttribute();
			 a;
			 a = a->Next() ) {
			
			std::string name = a->Name();

			if( name == "index" ) index = a->IntValue();
			else if( name == "id" ) {
				id = a->Value();
				ValidateID( id );
			} else if( name == "filter" ) force_filter = a->IntValue();
			else if( name == "loopfilter" ) force_loop_filter = a->IntValue();
			else {
				printf( "Encountered unknown sample attribute.\n" );
			}
		}

		// no children
	}

	EffectData::EffectData( const TiXmlElement *source ) {

		force_filter = -1;

		for( const TiXmlAttribute *a = source->FirstAttribute();
			 a; a = a->Next() ) {

			std::string name = a->Name();
			
			if( name == "file" ) filename = PATH + a->Value();
			else if( name == "id" ) id = a->Value();
			else if( name == "filter" ) force_filter = a->IntValue();
		}

		if( filename.empty() ) {
			printf( "error: effect missing filename\n" );
			return;
		}


		if( id.empty() ) {
			id = Path2ID( "SFX_", filename );
		}

		// no children
	}

	SampleData::SampleData( const EffectData &src ) {
		id = src.id;
		force_filter = src.force_filter;
		force_loop_filter = -1;
		index = -1;
	}
	*/
}
