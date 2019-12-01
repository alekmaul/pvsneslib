#include "it2spc.h"
#include "itloader.h"
#include "math.h"

#include <ctype.h>

#include "spc_program.h"

u8 CurrentInstrument = 0;
u8 patch_byte = 0x3C;
u8 max_instruments = 64; // changing these values breaks 
u8 max_length = 200;     // everything even if the values
u8 max_patterns = 64;    // of MOD_SEQU, MOD_PTABLE_L, etc,
u8 max_samples = 64;     // in the sound driver are changed...
u16 header_size = 616;
u16 driver_base = 0x400;
u16 module_base = 0x18ca;
u16 sample_table_offset = 0x200;
u16 spc_ram_size = 0;
u16 totalsizem1 = 0;
u32 totalitsize = 0;
u32 totabanksize = 0;

bool ChkSfx;

//unsigned char* spc_program = (unsigned char*)spc_sm_program;
int spc_program_size = sizeof( spc_program );

extern bool VERBOSE;
extern int BANKNUM;

enum {
	SPC_RAM_SIZE = 58000,
	MODULE_BASE = 0x1A00
};

namespace IT2SPC {



	/***********************************************************************************************
	 *
	 * Bank
	 *
	 ***********************************************************************************************/

	Bank::Bank( const ITLoader::Bank &bank, bool HIROM, bool CHKSFX ) {

		HiROM = HIROM;
		ChkSfx = CHKSFX;
		
		for( int i = 0, n = bank.modules.size(); i < n; i++ ) {
			AddModule( *(bank.modules[i]) );
		}
		
		for( int i = 0, n = bank.sounds.size(); i < n; i++ ) {
			AddSource( *(bank.sounds[i]) );
		}
		
		if ( VERBOSE ) {
			printf( "\n-----------------------------------------------------------------------" );
			printf( "\n  Total Modules Size: [%6i bytes]", totabanksize );
			printf( "\n       Total IT Size: [%6i bytes]", totalitsize );
		}
	}

	void Bank::AddSource( const ITLoader::SampleData &src ) {
		Sources.push_back( new Source( src ) );
	}
	
	void Bank::AddModule( const ITLoader::Module &mod ) {

		int size = IO::FileSize( mod.Filename.c_str() );

		if( VERBOSE ) {
			printf( "\n-----------------------------------------------------------------------" );
			printf( "\nAdding module, Title: <%s>", mod.Title );
			printf( "\n             IT Size: [%5i bytes]", size );
		}
		totalitsize += size;

		std::vector<u16> source_list;
		std::vector<u8>	directory;

		for( int i = 0; i < mod.SampleCount; i++ ) {
	//		ConversionInput::SampleData *cisd = 0;
	//		for( u32 j = 0; j < cinput.samples.size(); j++ ) {
	//			if( cinput.samples[j]->index == (i+1) ) {
	//				cisd = cinput.samples[j];
	//				break;
	//			}
	//		}
			Source *s = new Source( mod.Samples[i]->Data );//, cisd );

			int index = AddSource(s);
			bool exists = false;
			
			for( u32 j = 0; j < source_list.size(); j++ ) {
				if( source_list[j] == index ) {
					directory.push_back( j );
					exists = true;
					break;
				}
			}

			if( !exists ) {
				directory.push_back( source_list.size() );
				source_list.push_back( index );
			}
		}
		Modules.push_back( new Module( mod,  source_list, directory, Sources ) );
	}

	int Bank::AddSource( Source *s ) {
		for( u32 i = 0; i < Sources.size(); i++ ) {
			if( Sources[i]->Compare( *s ) ) {
				delete s;
				return i;
			}
		}
		Sources.push_back( s );
		return Sources.size() - 1;
	}

	Bank::~Bank() {
		for( u32 i = 0; i < Modules.size(); i++ ) {
			delete Modules[i];
		}

		for( u32 i = 0; i < Sources.size(); i++ ) {
			delete Sources[i];
		}
	}
	
	/***********************************************************************************************
	 *
	 * Module
	 *
	 ***********************************************************************************************/

	void ValidateID( std::string &id ) {
		for( u32 i = 0; i < id.size(); i++ ) {
			char c = id[i];
			if( (c >= 1 && c <= 47) ||
				(c >= 58 && c <= 64 ) ||
				(c >= 91 && c <= 96 ) ||
				(c >= 123) ) {
				id[i] = '_';
			}
		}
	}

	
	std::string Path2ID( const char *prefix, const std::string &source ) {
		std::string s;
		s = source;

		for( u32 i = 0; i < s.size(); i++ ) {
			s[i] = toupper(s[i]);
			if( s[i] == '\\' ) s[i] = '/';
		}
		
		u32 a = s.find_last_of( '/' );
		if( a != std::string::npos ) {
			s = s.substr( a+1 );
		}

		a = s.find_first_of( '.' );
		if( a != std::string::npos ) {
			s = s.substr( 0, a );
		}

		std::string res = prefix;
		res += s;

		ValidateID( res );

		return res;
	}

	Module::Module( 
		const ITLoader::Module &mod,  
		const std::vector<u16> &source_list,
		const std::vector<u8> &directory,
		const std::vector<Source*> &sources ) {

		id = Path2ID( "MOD_", mod.Filename );
//		id = cinput.id;

		InitialVolume = mod.GlobalVolume;
		InitialTempo = mod.InitialTempo;
		InitialSpeed = mod.InitialSpeed;

		SourceList = source_list;

		for( int i = 0; i < 8; i++ )
			InitialChannelVolume[i] = mod.ChannelVolume[i];

		for( int i = 0; i < 8; i++ )
			InitialChannelPanning[i] = mod.ChannelPan[i];

		ParseSMOptions(mod);

//		EchoVolumeL = cinput.EVL;
//		EchoVolumeR = cinput.EVR;
//		EchoDelay = cinput.EDL;
//		EchoFeedback = cinput.EFB;

//		for( int i = 0; i < 8; i++ ) {
//			EchoFIR[i] =  cinput.COEF[i];
//		}

//		EchoEnable = cinput.EON;

		for( int i = 0; i < max_length; i++ )
			Sequence[i] = i < mod.Length ? mod.Orders[i] : 255;

		for( int i = 0; i < mod.PatternCount; i++ ) {
			Patterns.push_back( new Pattern( *(mod.Patterns[i]) ) );
		}

		for( int i = 0; i < mod.InstrumentCount; i++ ) {
			Instruments.push_back( new Instrument( *(mod.Instruments[i]) ) );
		}
	
		for( int i = 0; i < mod.SampleCount; i++ ) {
			Samples.push_back( 
				new Sample( 
					*(mod.Samples[i]), 
					directory[i], 
					sources[source_list[directory[i]]]->GetTuningFactor() 
				)
			);
		}

		if( VERBOSE ) {
			int pattsize = 0;
			for( int i = 0; i < mod.PatternCount; i++ ) {
				pattsize += Patterns[i]->GetExportSize();
			}
			int sampsize = 0;
			for( u32 i = 0; i < source_list.size(); i++ ) {
				sampsize += sources[source_list[i]]->GetDataLength();
			}
			int instrsize = 0;
			for (u32 i = 0; i < Instruments.size(); i++ ) {
				instrsize += Instruments[i]->GetExportSize();
			}
			int envsize = 0;
			for (u32 i = 0; i < Instruments.size(); i++ ) {
				envsize += Instruments[i]->GetExportSize();
			}
			//othersize += GetExportSize_Header();

			u32 echosize = EchoDelay * 2048;
			u32 totalsize = pattsize + sampsize + instrsize + envsize + echosize;
			spc_ram_size = 65535 - module_base - header_size;
			u32 bytesfree = spc_ram_size - totalsize - header_size;
			totabanksize = totabanksize + totalsize;
			if (ChkSfx && (totalsizem1!=0) ) {
				printf( 
					"\nConversion report:\n"
					"    Pattern data: [%5i bytes]   Module Length: [%i/%i]\n"
					"     Sample data: [%5i bytes]        Patterns: [%i/%i]\n"
					" Instrument data: [%5i bytes]     Instruments: [%i/%i]\n"
					"   Envelope data: [%5i bytes]         Samples: [%i/%i]\n"
					"     Echo region: [%5i bytes]\n"
					"           Total: [%5i bytes]   *%i bytes free* *%i bytes free with 1st module*\n"
					,pattsize, mod.Length, max_length,
					sampsize, mod.PatternCount, max_patterns,
					instrsize, mod.InstrumentCount, max_instruments,
					envsize, mod.SampleCount, max_samples,
					echosize,
					totalsize, bytesfree, bytesfree-totalsizem1
				);
			} else {
				printf( 
					"\nConversion report:\n"
					"    Pattern data: [%5i bytes]   Module Length: [%i/%i]\n"
					"     Sample data: [%5i bytes]        Patterns: [%i/%i]\n"
					" Instrument data: [%5i bytes]     Instruments: [%i/%i]\n"
					"   Envelope data: [%5i bytes]         Samples: [%i/%i]\n"
					"     Echo region: [%5i bytes]\n"
					"           Total: [%5i bytes]   *%i bytes free*\n"
					,pattsize, mod.Length, max_length,
					sampsize, mod.PatternCount, max_patterns,
					instrsize, mod.InstrumentCount, max_instruments,
					envsize, mod.SampleCount, max_samples,
					echosize,
					totalsize, bytesfree
				);
			}
			if (totalsizem1==0)
					totalsizem1=totalsize;

			if( totalsize > spc_ram_size ) {
				printf( "\ERROR: Module is too big. Maximum is %i bytes.", spc_ram_size );
			}
		}
	}

	static int SearchForSNESMODtag( const char *message ) {

		const char *test = "[[SNESMOD]]";
		int matches = 0;

		for( int i = 0; message[i]; i++ ) {
			if( message[i] == test[matches] )
				matches++;
			else
				matches = 0;
			if( matches == 11 )
				return i - 10;
		}
		return -1;
	}

	static int SeekNextLine( const char *text, int offset ) {
		while( text[offset] != 0 &&
			text[offset] != '\n' &&
			text[offset] != '\r' )

			offset++;

		while( text[offset] == '\n' || text[offset] == '\r' )
			offset++;

		return offset;
	}
	
	static bool is_whitespace( char c ) {
		return c == ' ' || c == '\t';
	}

	static bool not_term( char c ) {
		return c != 0 && c != '\r' && c != '\n';
	}

	static int minmax( int value, int min, int max ) {
		value = value < min ? min : value;
		return value > max ? max : value;
	}
	
	void Module::ParseSMOption( const char *text ) {
		std::vector<std::string> args;
		if( !text ) return;
		if( !text[0] ) return;
		
		int offs = 0;
		
		// skip whitespace
		while( is_whitespace(text[offs]) ) {
			offs++;
		}
		
		while( not_term(text[offs]) ) { //!= 0 && text[offs] != '\r' && text[offs] != '\n' ) {
			int len;
			for( len = 0; not_term(text[offs+len]); len++ ) {
				if( is_whitespace( text[offs+len] ) ) {
					break;
				}
			}
			if( len == 0 ) {
				offs++;
			} else {
				args.push_back( "" );
				args[args.size()-1].assign( text + offs, len );
				offs += len;
			}

			// skip whitespace
			while( is_whitespace(text[offs]) ) {
				offs++;
			}
		}

		if( args.empty() ) {
			// no args?
			return;
		}

		// args is filled with arguments
		// determine command type

		for( u32 i = 0; i < args[0].size(); i++ ) {
			if( args[0].at(i) >= 'A' && args[0].at(i) <= 'Z' ) {
				args[0].at(i) += 'a' - 'A';
			}
		}

#define TESTCMD(cmd) if( args[0] == cmd )

		TESTCMD( "edl" ) {
			if( args.size() < 2 ) {
				return;
			}
			EchoDelay = minmax( atoi( args[1].c_str() ), 0, 15 );
		} else TESTCMD( "efb" ) {
			if( args.size() < 2 ) {
				return;
			}
			EchoFeedback = minmax( atoi( args[1].c_str() ), -128, 127 );
		} else TESTCMD( "evol" ) {
			if( args.size() < 2 ) {
				return;
			} else if( args.size() < 3 ) {
				EchoVolumeL = minmax( atoi( args[1].c_str() ), -128, 127 );
				EchoVolumeR = EchoVolumeL;
			} else {
				EchoVolumeL = minmax( atoi( args[1].c_str() ), -128, 127 );
				EchoVolumeR = minmax( atoi( args[2].c_str() ), -128, 127 );
			}
		} else TESTCMD( "efir" ) {
			for( u32 i = 0; i < 8; i++ ) {
				if( args.size() <= (1+i) ) {
					return;
				}
				EchoFIR[i] = minmax( atoi( args[1+i].c_str() ), -128, 127 );
			}
		} else TESTCMD( "eon" ) {
			for( u32 i = 1; i < args.size(); i++ ) {
				int c = atoi( args[i].c_str() );
				if( c >= 1 && c <= 8 ) {
					EchoEnable |= (1<<(c-1));
				}
			}
		}
	}
	
	void Module::ParseSMOptions( const ITLoader::Module &mod ) {
		EchoVolumeL = 0;
		EchoVolumeR = 0;
		EchoDelay = 0;
		EchoFeedback = 0;
		EchoFIR[0] = 127;
		for( int i = 1; i < 8; i++ )
			EchoFIR[i] = 0;
		
		EchoEnable = 0;

		if( mod.Message ) {
			int offset = SearchForSNESMODtag( mod.Message );
			if( offset != -1 ) {

				offset = SeekNextLine( mod.Message, offset );
				while( mod.Message[offset] != 0 ) {
					ParseSMOption( mod.Message + offset );
					offset = SeekNextLine( mod.Message, offset );
				}
				
				
			}
		}
	}

	Module::~Module() {
		for( u32 i = 0; i < Patterns.size(); i++ )
			delete Patterns[i];

		for( u32 i = 0; i < Instruments.size(); i++ )
			delete Instruments[i];

		for( u32 i = 0; i < Samples.size(); i++ )
			delete Samples[i];
	}

	/***********************************************************************************************
	 *
	 * Pattern
	 *
	 ***********************************************************************************************/

	Pattern::Pattern( ITLoader::Pattern &source ) {
		Rows = (u8)(source.Rows - 1);
		
		int row;
		u8 *read = source.Data;

		if( source.DataLength != 0 ) {
			
			std::vector<u8> row_buffer;

			u8 spc_hints;
			u8 data_bits;
			u8 p_maskvar[8];

			spc_hints = 0;
			data_bits = 0;

			for( row = 0; row < source.Rows; ) {
				u8 chvar = *read++;
				
				if( chvar == 0 ) {
					Data.push_back( 0xFF ); //spc_hints
					Data.push_back( data_bits );
					
					for( int i = 0, n = row_buffer.size(); i < n; i++ )
						Data.push_back( row_buffer[i] );
					row_buffer.clear();
					
					data_bits = 0;
					spc_hints = 0;
					row++;
					continue;
				}
				
				int channel = (chvar - 1) & 63;
				data_bits |= 1<<channel;

				u8 maskvar;
				if( chvar & 128 ) {
					maskvar = *read++;
					maskvar = ((maskvar>>4) | (maskvar<<4)) & 0xFF;
					maskvar |= maskvar>>4;
					p_maskvar[channel] = maskvar;
				} else {
					maskvar = p_maskvar[channel];
				}

				row_buffer.push_back( maskvar );

				if( maskvar & 16 ) {		// note
					row_buffer.push_back( *read++ );
				}
				if( maskvar & 32 ) {		// instr
					row_buffer.push_back( *read++ );
				}
				if( maskvar & 64 ) {		// vcmd
					row_buffer.push_back( *read++ );
				}

				u8 cmd,param;
				if( maskvar & 128 ) {		// cmd+param

					row_buffer.push_back( cmd = *read++ );
					row_buffer.push_back( param = *read++ );
				}

				if( (maskvar & 1) ) {
					spc_hints |= 1<<channel;
					if( maskvar & 128 ) {
						if( (cmd == 7) || (cmd == 19 && (param&0xF0) == 0xD0) ) { 
							// glissando or note delay:
							// cancel hint
							spc_hints &= ~(1<<channel);
						}
					}
				}
			}
		} else {
			// empty pattern
			for( int i = 0; i < source.Rows; i++ ) {
				Data.push_back( 0 );
				Data.push_back( 0 );
			}
		}
	}

	Pattern::~Pattern() {

	}

	/***********************************************************************************************
	 *
	 * Instrument
	 *
	 ***********************************************************************************************/

	Instrument::Instrument( const ITLoader::Instrument &source ) {
		int a =  source.Fadeout / 4;
		Fadeout = a > 255 ? 255 : a;
		SampleIndex = source.Notemap[60].Sample - 1;
		GlobalVolume = source.GlobalVolume;
		SetPan = source.DefaultPan;

		// load envelope
		const ITLoader::Envelope &e = *source.VolumeEnvelope;

		EnvelopeLength = e.Enabled ? e.Length*4 : 0;
		CurrentInstrument++;

		if( EnvelopeLength ) {
			EnvelopeSustain = e.Sustain ? e.SustainStart*4 : 0xFF;
			EnvelopeLoopStart = e.Loop ? e.LoopStart*4 : 0xFF;
			EnvelopeLoopEnd = e.Loop ? e.LoopEnd*4 : 0xFF;

			EnvelopeData = new EnvelopeNode[ EnvelopeLength/4 ];
			for( int i = 0; i < EnvelopeLength/4; i++ ) {
				EnvelopeNode &ed = EnvelopeData[i];
				ed.y = e.Nodes[i].y;
				if( i != (EnvelopeLength/4)-1 ) {
					int duration = e.Nodes[i+1].x - e.Nodes[i].x;
					ed.duration = duration;
					if( duration > 0 ) {
					    ed.delta = ((e.Nodes[i+1].y - e.Nodes[i].y) * 256 + duration/2) / duration;
					} else {
					    ed.delta = 64;
					    ed.duration = 255;
					    printf("\nWARNING: Volume envelope for instrument %i must have more\n         than one node to play properly.\n\n", CurrentInstrument);
					}
				} else {
					ed.delta = 0;
					ed.duration = 0;
				}
			}
		} else {
			EnvelopeData = 0;
		}

	}

	Instrument::~Instrument() {
		if( EnvelopeData )
			delete EnvelopeData;
	}

	/***********************************************************************************************
	 *
	 * Sample
	 *
	 ***********************************************************************************************/

	Sample::Sample( const ITLoader::Sample &source, int directory, double tuning ) {
		DefaultVolume = source.DefaultVolume;
		GlobalVolume = source.GlobalVolume;
		SetPan = source.DefaultPanning ^ 128;

		double a = ((double)source.Data.C5Speed * tuning);
		PitchBase = (int)floor(log(a / 8363.0) / log(2.0) * 768.0 + 0.5);
		
		DirectoryIndex = directory;
	}

	/***********************************************************************************************
	 *
	 * Source
	 *
	 ***********************************************************************************************/

	bool Source::Compare( const Source &test ) const {
		if( Length != test.Length )
			return false;
		if( Loop != test.Loop )
			return false;
		for( int i = 0; i < Length; i++ ) {
			if( Data[i] != test.Data[i] ) {
				return false;
			}
		}
		return true;
	}

	/***********************************************************************************************
	 *
	 * Export
	 *
	 ***********************************************************************************************/

	void Bank::MakeSPC( const char *spcfile ) const {
		std::string nstr;

		IO::File file( spcfile, IO::MODE_WRITE );

//		for( u32 i = 0; i < Modules.size(); i++ ) {
//			nstr = output;
//			char tnumber[64];
//			sprintf( tnumber, "%i", i );
//			nstr += tnumber;
//			nstr += ".spc";
			// open file
		//	IO::File file( nstr.c_str(), IO::MODE_WRITE );
			
			file.WriteAscii( "SNES-SPC700 Sound File Data v0.30" );
			file.Write8( 26 ); // 26,26
			file.Write8( 26 );
			file.Write8( 26 );	// header contins id666 tag
			file.Write8( 30 ); // version minor

			// SPC700 registers
			file.Write16( driver_base );
			file.Write8( 0 );		// A
			file.Write8( 0 );		// X
			file.Write8( 0 );		// Y
			file.Write8( 0 );		// PSW
			file.Write8( 0xEF );	// SP
			file.Write16( 0 );		// reserved

			// ID666 tag
			file.WriteAsciiF( "<INSERT SONG TITLE>", 32 );
			file.WriteAsciiF( "<INSERT GAME TITLE>", 32 );
			file.WriteAsciiF( "NAME OF DUMPER", 16 );
			file.WriteAsciiF( "comments...", 32 );
			file.WriteAsciiF( "", 11 );
			file.WriteAsciiF( "180", 3 );
			file.WriteAsciiF( "5000", 5 );
			file.WriteAsciiF( "<INSERT SONG ARTIST>", 32 );
			file.Write8( 0);
			file.Write8( '0' );
			file.ZeroFill( 45 ); // reserved

			//-------------------------------------------------------
			// SPC memory
			//-------------------------------------------------------

			// zero fill upto program block
			file.ZeroFill( driver_base );

			int SampleTableOffset = file.Tell() - sample_table_offset;
			
			// write spc program
			for ( int i = 0; i < spc_program_size; i++ ) {
				if( i == patch_byte || i == patch_byte+1 ) // PATCH
					file.Write8( 0 );
				else
					file.Write8( spc_program[i] );
			}
			
			// zero fill upto module base
			file.ZeroFill( (module_base - driver_base - spc_program_size) );

			int StartOfModule = file.Tell();
			
			Modules[0]->Export( file, false );

			u16 source_table[2*max_samples];

			//!!TODO export sample list and create sample table!
			for( u32 i = 0; i < Modules[0]->SourceList.size(); i++ ) {
				source_table[i*2+0] = file.Tell() - StartOfModule + MODULE_BASE;
				source_table[i*2+1] = source_table[i*2] + Sources[ Modules[0]->SourceList[i] ]->GetLoopPoint();
				Sources[ Modules[0]->SourceList[i] ]->Export( file, true );
			}

			int EndOfData = file.Tell();
			file.Seek( SampleTableOffset );
			for( int i = 0; i < 128; i++ )
				file.Write16( source_table[i] );

			file.Seek( EndOfData );
			
			file.ZeroFill( 65792 - EndOfData );
		//file.ZeroFill( 65536 - (EndOfData - StartOfModule-MODULE_BASE) );
			
			// DSP registers
			for( int i = 0; i < 128; i++ )
				file.Write8( 0 );
			
			// unused/ipl rom
			for( int i = 0; i < 128; i++ )
				file.Write8( 0 );
			
			//file.WriteAsciiF
		file.Close();
	}
	
	void Bank::Export( const char *output ) const {
		std::string bin_output = output;
		bin_output += ".bnk";
		IO::File file( bin_output.c_str(), IO::MODE_WRITE );
		
		file.Write16( Sources.size() );
		file.Write16( Modules.size() );

		// reserve space for tables
		for( u32 i = 0; i < 3*128; i++ )
			file.Write8( 0xAA );

		for( u32 i = 0; i < Sources.size() * 3; i++ ) {
			file.Write8( 0xAA );
		}

		std::vector<u32> module_ptr;
		std::vector<u32> source_ptr;
		
		for( u32 i = 0; i < Modules.size(); i++ ) {
			module_ptr.push_back( file.Tell() );
			Modules[i]->Export( file, true );
		}

		for( u32 i = 0; i < Sources.size(); i++ ) {
			source_ptr.push_back( file.Tell() );
			Sources[i]->Export( file, false );
		}

		// export module pointers
		file.Seek( 4 );
		for( u32 i = 0; i < 128; i++ ) {
			int addr, bank;
			if( i < Modules.size() ) {
				if( HiROM ) { // 64k banks
					addr = module_ptr[i] & 65535;
					bank = module_ptr[i] >> 16;
				} else { // 32k banks
					addr = 0x8000 + (module_ptr[i] & 32767);
					bank = module_ptr[i] >> 15;
				}
			} else {
				bank = addr = 0;
			}
			file.Write16( addr );
			file.Write8( bank );
		}

		// export source pointers
		for( u32 i = 0; i < Sources.size(); i++ ) {
			int addr, bank;
			if( HiROM ) { // 64k banks
				addr = source_ptr[i] & 65535;
				bank = source_ptr[i] >> 16;
			} else { // 32k banks
				addr = 0x8000 + (source_ptr[i] & 32767);
				bank = source_ptr[i] >> 15;
			}
			file.Write16( addr );
			file.Write8( bank );
		}

		file.Close();

		std::string asm_out = output;
		asm_out += ".asm";
		ExportASM( bin_output.c_str(), asm_out.c_str() );

		std::string inc_out = output;
		inc_out += ".h";
		ExportINC( inc_out.c_str() );
	}

	void Bank::ExportINC( const char *output ) const {
		FILE *f = fopen( output, "w" );

		fprintf( f,
			"// snesmod soundbank definitions\n\n"
			"#ifndef __SOUNDBANK_DEFINITIONS__\n"
			"#define __SOUNDBANK_DEFINITIONS__\n\n"
			);
			//".import __SOUNDBANK__\n\n");

		for( u32 i = 0; i < Modules.size(); i++ ) {
			if( !Modules[i]->id.empty() ) {
				fprintf( f, "#define %-32s	%i\n", Modules[i]->id.c_str(), i );
			}
		}
		fprintf( f, "\n" );

		for( u32 i = 0; i < Sources.size(); i++ ) {
			if( !Sources[i]->id.empty() ) {
				fprintf( f, "#define %-32s	%i\n", Sources[i]->id.c_str(), i );
			}
		}
		fprintf( f, "\n#endif // __SOUNDBANK_DEFINITIONS__\n" );
		fclose(f);
	}

	void Bank::ExportASM( const char *inputfile, const char *outputfile ) const {
		FILE *f = fopen( outputfile, "w" );

		int size = IO::FileSize( inputfile );
		
		fprintf( f, 
			";************************************************\n"
			"; snesmod soundbank data                        *\n"
			"; total size: %10i bytes                  *\n"
			";************************************************\n"
			"\n"
			".include \"hdr.asm\"\n"
			"\n",
			size
		);
		
		if (size<=32768) {
			fprintf( f, 
				".bank %i\n"
				".section \"SOUNDBANK\" ; need dedicated bank(s)\n\n"
				"__SOUNDBANK__:\n",
				BANKNUM
			);
		
			std::string foo = inputfile;
			
			for( u32 i = 0; i < foo.size(); i++ ) {
				if( foo[i] == '\\' ) foo[i] = '/';
			}
			int ffo = foo.find_last_of( '/' );
			if( ffo != std::string::npos )
				foo = foo.substr( ffo + 1 );
		
			fprintf( f, ".incbin \"%s\"\n", foo.c_str() );
		
			fprintf( f, ".ends\n");
		}
		else {
			u32 lastbank = size/32768;
			for( u32 j = 0; j <= lastbank; j++ ) {
				fprintf( f, 
					".bank %i\n"
					".section \"SOUNDBANK%i\" ; need dedicated bank(s)\n\n"
					"__SOUNDBANK__%i:\n",
					BANKNUM+j,j,j
				);
		
				std::string foo = inputfile;
			
				for( u32 i = 0; i < foo.size(); i++ ) {
					if( foo[i] == '\\' ) foo[i] = '/';
				}
				int ffo = foo.find_last_of( '/' );
				if( ffo != std::string::npos )
					foo = foo.substr( ffo + 1 );
		
				if (j == 0) 
					fprintf( f, ".incbin \"%s\" read $8000\n", foo.c_str() );
				else if (j == lastbank)
					fprintf( f, ".incbin \"%s\" skip $%x\n", foo.c_str(), j*0x8000 );
				else
					fprintf( f, ".incbin \"%s\" skip $%x read $8000\n", foo.c_str(), j*0x8000 );
		
				fprintf( f, ".ends\n\n");
			}
		}
		
/*
		IO::File fin( inputfile, IO::MODE_READ );

		const int blocklen = 32;
		
		int block_idx=0;
		int read=0;
		while( read < size ) {
			fprintf( f, "\t.byte " );
			for( block_idx = 0; block_idx < blocklen; block_idx++ ) {
				bool last = read+block_idx == size-1;
				fprintf( f, "$%02x%s", fin.Read8(), (block_idx == blocklen-1 || last) ? "" : ", " );
				if( last ) break;
			}
			fprintf( f, "\n" );
			read += blocklen;
		}
		
		fclose(f) ;
		*/
	}

	void Source::Export( IO::File &file, bool spc_direct ) const {

		if( !spc_direct ) {
			file.Write16( Length );
			file.Write16( Loop );
		}

		for( int i = 0; i < Length; i++ ) {
			file.Write8( Data[i] );
		}

		if( !spc_direct ) {
			if( Length & 1 ) 
				file.Write8( 0 ); // align by 2
		}
	}

	void Module::Export( IO::File &file, bool write_header ) const {

		int HeaderStart = file.Tell();

		if( write_header ) {

			file.Write16( 0xaaaa );	// reserve for module size
			file.Write16( SourceList.size() );

			for( u32 i = 0; i < SourceList.size(); i++ ) {
				file.Write16( SourceList[i] );
			}
		}

		int ModuleStart = file.Tell();

		file.Write8( InitialVolume );
		file.Write8( InitialTempo );
		file.Write8( InitialSpeed );

		for( int i = 0; i < 8; i++ )
			file.Write8( InitialChannelVolume[i] );
		
		for( int i = 0; i < 8; i++ )
			file.Write8( InitialChannelPanning[i] );

		file.Write8( EchoVolumeL );
		file.Write8( EchoVolumeR );
		file.Write8( EchoDelay );
		file.Write8( EchoFeedback );
		
		for( int i = 0; i < 8; i++ ) 
			file.Write8( EchoFIR[i] ) ;

		file.Write8( EchoEnable );

		for( int i = 0; i < max_length; i++ )
			file.Write8( Sequence[i] );
		
		std::vector<u16> pattern_ptr;
		std::vector<u16> instrument_ptr;
		std::vector<u16> sample_ptr;

		u32 start_of_tables = file.Tell();
		
		// reserve space for pointers
		for( int i = 0; i < ( max_patterns + max_instruments + max_samples ); i++ ) {
			file.Write16( 0xBAAA );
		}

		for( u32 i = 0; i < Patterns.size(); i++ ) {
			u32 ptr = file.Tell() - ModuleStart;
			
			if( ptr > SPC_RAM_SIZE )
				printf( "\nERROR: Module is too big." );

			pattern_ptr.push_back( (u16)ptr );
			Patterns[i]->Export( file );
		}

		for( u32 i = 0; i < Instruments.size(); i++ ) {
			u32 ptr = file.Tell() - ModuleStart;

			if( ptr > SPC_RAM_SIZE )
				printf( "\nERROR: Module is too big." );

			instrument_ptr.push_back( (u16)ptr );
			Instruments[i]->Export( file );
		}

		for( u32 i = 0; i < Samples.size(); i++ ) {
			u32 ptr = file.Tell() - ModuleStart;

			if( ptr > SPC_RAM_SIZE )
				printf( "\nERROR: Module is too big." );

			sample_ptr.push_back( (u16)ptr );
			Samples[i]->Export( file );
		}

		u32 end_of_mod = file.Tell();

		if( write_header ) {
			file.Seek( HeaderStart );
			file.Write16( (end_of_mod - ModuleStart + 1) >> 1 );
		}

		file.Seek( start_of_tables );

		for( u32 i = 0; i < max_patterns; i++ )
			file.Write8( i < Patterns.size() ? ((pattern_ptr[i] + MODULE_BASE) & 0xFF) : 0xFF );
		for( u32 i = 0; i < max_patterns; i++ )
			file.Write8( i < Patterns.size() ? ((pattern_ptr[i] + MODULE_BASE) >> 8) : 0xFF );

		for( u32 i = 0; i < max_instruments; i++ )
			file.Write8( i < Instruments.size() ? ((instrument_ptr[i] + MODULE_BASE) & 0xFF) : 0xFF );
		for( u32 i = 0; i < max_instruments; i++ )
			file.Write8( i < Instruments.size() ? ((instrument_ptr[i] + MODULE_BASE) >> 8) : 0xFF );

		for( u32 i = 0; i < max_samples; i++ )
			file.Write8( i < Samples.size() ? ((sample_ptr[i] + MODULE_BASE) & 0xFF) : 0xFF );
		for( u32 i = 0; i < max_samples; i++ )
			file.Write8( i < Samples.size() ? ((sample_ptr[i] + MODULE_BASE) >> 8) : 0xFF );

		file.Seek( end_of_mod );

		if( write_header ) {
			if( file.Tell() & 1 )
				file.Write8( 0 ); // align by 2
		}
	}

	void Pattern::Export( IO::File &file ) const {
		file.Write8( Rows );
		for( u32 i = 0; i < Data.size(); i++ ) {
			file.Write8( Data[i] );
		}
	}

	void Instrument::Export( IO::File &file ) const {
		file.Write8( Fadeout );
		file.Write8( SampleIndex );
		file.Write8( GlobalVolume );
		file.Write8( SetPan );
		file.Write8( EnvelopeLength );
		if( EnvelopeLength ) {
			file.Write8( EnvelopeSustain );
			file.Write8( EnvelopeLoopStart );
			file.Write8( EnvelopeLoopEnd );
			for( int i = 0; i < EnvelopeLength/4; i++ ) {
				file.Write8( EnvelopeData[i].y );
				file.Write8( EnvelopeData[i].duration );
				file.Write16( EnvelopeData[i].delta );
			}
		}
	}

	void Sample::Export( IO::File &file ) const {
		file.Write8( DefaultVolume );
		file.Write8( GlobalVolume );
		file.Write16( PitchBase );
		file.Write8( DirectoryIndex );
		file.Write8( SetPan );
	}
}
