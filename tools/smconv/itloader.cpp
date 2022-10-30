#include "itloader.h"
#include "io.h"

namespace ITLoader {

	template<typename T> static void deletePtrVector( std::vector<T*> &vecs ) {

		for(typename std::vector<T*>::iterator iter = vecs.begin(), ending = vecs.end();
			iter != ending;
			iter++ ) {

			if( *iter )
				delete (*iter);
		}

		vecs.clear();
	}

	Bank::Bank( const std::vector<const char *> &files ) {
		for( u32 i = 0; i < files.size(); i++ ) {
			AddModule( files[i] );
		}

//		for( u32 i = 0; i < source->effects.size(); i++ ) {
//			AddSound( source->effects[i]->filename.c_str() );
//		}
	}

	Bank::~Bank() {
		deletePtrVector<Module>( modules );
		deletePtrVector<SampleData>( sounds );
	}

	void Bank::AddSound( const char *filename ) {
		sounds.push_back( new SampleData( filename ) );
	}

	void Bank::AddModule( const char *filename ) {

		modules.push_back( new Module( filename ) );
	}

	Module::Module( const char *filename ) {
		IO::File file( filename, IO::MODE_READ );

		Filename = filename;

		if( file.Read8() != 'I' ) return;
		if( file.Read8() != 'M' ) return;
		if( file.Read8() != 'P' ) return;
		if( file.Read8() != 'M' ) return;

		for( int i = 0; i < 26; i++ )
			Title[i] = file.Read8();

		PatternHighlight = file.Read16();

		Length = file.Read16();
		InstrumentCount = file.Read16();
		SampleCount = file.Read16();
		PatternCount = file.Read16();
		Cwtv = file.Read16();
		Cmwt = file.Read16();
		Flags = file.Read16();
		Special = file.Read16();
		GlobalVolume = file.Read8();
		MixingVolume = file.Read8();
		InitialSpeed = file.Read8();
		InitialTempo = file.Read8();

		Sep = file.Read8();
		PWD = file.Read8();
		MessageLength = file.Read16();

		u32 MessageOffset = file.Read32();

		file.Skip( 4 ); // reserved

		for( int i = 0; i < 64; i++ )
			ChannelPan[i] = file.Read8();

		for( int i = 0; i < 64; i++ )
			ChannelVolume[i] = file.Read8();

		bool foundend=false;
		int ActualLength=Length;
		for( int i = 0; i < 256; i++ ) {
			Orders[i] = i < Length ? file.Read8() : 255;
			if( Orders[i] == 255 && !foundend ) {
				foundend=true;
				ActualLength = i+1;
			}
		}

		Length = ActualLength;

		Instruments = new Instrument*[InstrumentCount];
		Samples = new Sample*[SampleCount];
		Patterns = new Pattern*[PatternCount];

		u32 *InstrTable = new u32[InstrumentCount];
		u32 *SampleTable = new u32[SampleCount];
		u32 *PatternTable = new u32[PatternCount];

		for( int i = 0; i < InstrumentCount; i++ )
			InstrTable[i] = file.Read32();
		for( int i = 0; i < SampleCount; i++ )
			SampleTable[i] = file.Read32();
		for( int i = 0; i < PatternCount; i++ )
			PatternTable[i] = file.Read32();

		for( int i = 0; i < InstrumentCount; i++ ) {
			file.Seek( InstrTable[i] );
			Instruments[i] = new Instrument( file );
		}

		for( int i = 0; i < SampleCount; i++ ) {
			file.Seek( SampleTable[i] );
			Samples[i] = new Sample( file );
		}

		for( int i = 0; i < PatternCount; i++ ) {
			if( PatternTable[i] ) {
				file.Seek( PatternTable[i] );
				Patterns[i] = new Pattern( file );
			} else {
				Patterns[i] = new Pattern();
			}
		}

		if( MessageLength ) {
			Message = new char[MessageLength+1];
			file.Seek( MessageOffset );
			Message[MessageLength] = 0;
			for( int i = 0; i < MessageLength; i++ )
				Message[i] = file.Read8();
		} else {
			Message = 0;
		}

		delete[] InstrTable;
		delete[] SampleTable;
		delete[] PatternTable;
	}

	Module::~Module() {
		for( int i = 0; i < InstrumentCount; i++ )
			delete Instruments[i];
		delete[] Instruments;
		for( int i = 0; i < SampleCount; i++ )
			delete Samples[i];
		delete[] Samples;
		for( int i = 0; i < PatternCount; i++ )
			delete Patterns[i];
		delete[] Patterns;
		if( Message ) {
			delete[] Message;
		}
	}

	Instrument::Instrument( IO::File &file ) {
		file.Skip(4); // IMPI
		DOSFilename[12] = 0;
		for( int i = 0; i < 12; i++ )
			DOSFilename[i] = file.Read8();
		file.Skip(1);	// 00h
		NewNoteAction = file.Read8();
		DuplicateCheckType = file.Read8();
		DuplicateCheckAction = file.Read8();
		Fadeout = file.Read16();
		PPS = file.Read8();
		PPC = file.Read8();
		GlobalVolume = file.Read8();
		DefaultPan = file.Read8();
		RandomVolume = file.Read8();
		RandomPanning = file.Read8();
		TrackerVersion = file.Read16();
		NumberOfSamples = file.Read8();

		Name[26] = 0;
		for( int i = 0; i < 26; i++ )
			Name[i] = file.Read8();

		InitialFilterCutoff = file.Read8();
		InitialFilterResonance = file.Read8();

		MidiChannel = file.Read8();
		MidiProgram = file.Read8();
		MidiBank = file.Read16();

		file.Read8(); // reserved

		for( int i = 0; i < 120; i++ ) {
			Notemap[i].Note = file.Read8();
			Notemap[i].Sample = file.Read8();
		}

		VolumeEnvelope = new Envelope( file );
		PanningEnvelope = new Envelope( file );
		PitchEnvelope = new Envelope( file );
	}

	Instrument::~Instrument() {
		delete VolumeEnvelope;
		delete PanningEnvelope;
		delete PitchEnvelope;
	}

	Envelope::Envelope( IO::File &file ) {
		u8 FLG = file.Read8();
		Enabled = !!(FLG & 1);
		Loop = !!(FLG & 2);
		Sustain = !!(FLG & 4);
		IsFilter = !!(FLG & 128);

		Length = file.Read8();

		LoopStart = file.Read8();
		LoopEnd = file.Read8();

		SustainStart = file.Read8();
		SustainEnd = file.Read8();

		for( int i = 0; i < 25; i++ ) {
			Nodes[i].y = file.Read8();
			Nodes[i].x = file.Read16();
		}
	}

	Sample::Sample( IO::File &file ) {
		file.Skip(4);	// IMPS
		DOSFilename[12] = 0;
		for( int i = 0; i < 12; i++ )
			DOSFilename[i] = file.Read8();
		file.Skip(1);	// 00h
		GlobalVolume = file.Read8();
		u8 Flags = file.Read8();

		HasSample = !!(Flags & 1);
		Data.Bits16 = !!(Flags & 2);
		Stereo = !!(Flags & 4);
		Compressed = !!(Flags & 8);
		Data.Loop = !!(Flags & 16);
		Data.Sustain = !!(Flags & 32);
		Data.BidiLoop = !!(Flags & 64);
		Data.BidiSustain = !!(Flags & 128);

		DefaultVolume = file.Read8();

		Name[26] = 0;
		for( int i = 0; i < 26; i++ )
			Name[i] = file.Read8();

		Convert = file.Read8();
		DefaultPanning = file.Read8();

		Data.Length = file.Read32();
		Data.LoopStart = file.Read32();
		Data.LoopEnd = file.Read32();
		Data.C5Speed = file.Read32();
		Data.SustainStart = file.Read32();
		Data.SustainEnd = file.Read32();

		u32 SamplePointer = file.Read32();

		VibratoSpeed = file.Read8();
		VibratoDepth = file.Read8();
		VibratoRate = file.Read8();
		VibratoForm = file.Read8();

		file.Seek( SamplePointer );
		LoadData( file );
	}

	Sample::~Sample() {
		if( Data.Bits16 ) {
			delete[] Data.Data16;
		} else {
			delete[] Data.Data8;
		}
	}

	void Sample::LoadData( IO::File &file ) {
		if( !Compressed ) {

			// subtract offset for unsigned samples
			int offset = (Convert&1) ? 0 : (Data.Bits16 ? -32768 : -128);

			// signed samples
			if( Data.Bits16 ) {
				Data.Data16 = new s16[ Data.Length ];
				for( int i = 0; i < Data.Length; i++ ) {
					Data.Data16[i] = file.Read16() + offset;
				}
			} else {
				Data.Data8 = new s8[ Data.Length ];
				for( int i = 0; i < Data.Length; i++ ) {
					Data.Data8[i] = file.Read8() + offset;
				}
			}
		} else {
			// TODO : accept compressed samples.
		}
	}

	Pattern::Pattern( IO::File &file ) {
		DataLength = file.Read16();
		Rows = file.Read16();
		file.Skip( 4 ); // reserved
		Data = new u8[ DataLength ];
		for( int i = 0; i < DataLength; i++ ) {
			Data[i] = file.Read8();
		}
	}

	Pattern::Pattern() {
		Rows = 64;
		Data = new u8[ Rows ];
		for( int i = 0; i < Rows; i++ ) {
			Data[i] = 0;
		}
	}

	Pattern::~Pattern() {
		if( Data ) {
			delete[] Data;
		}
	}

	static inline const unsigned int str4( const char *str ) {
		return (unsigned int)str[0] |
			((unsigned int)str[1] << 8) |
			((unsigned int)str[2] << 16) |
			((unsigned int)str[3] << 24);
	}

	SampleData::SampleData() {

	}

	SampleData::SampleData( const char *filename ) {

		// load data from WAV file

		unsigned int file_size;
		unsigned int bit_depth = 8;
		unsigned int hasformat = 0;
		//unsigned int hasdata = 0;
		unsigned int chunk_code;
		unsigned int chunk_size;
		unsigned int num_channels = 0;

		// initialize data
		Bits16 = false;
		Length = 0;
		LoopStart = 0;
		LoopEnd = 0;
		C5Speed = 32000;
		SustainStart = 0;
		SustainEnd = 0;
		Loop = false;
		Sustain = false;
		BidiLoop = false;
		BidiSustain = false;

		file_size = IO::FileSize( filename );
		IO::File file( filename, IO::MODE_READ );

		file.Read32();						// "RIFF"
		file.Read32();						// filesize-8
		file.Read32();						// "WAVE"

		while( 1 )
		{
			// break on end of file
			if( file.Tell() >= file_size ) break;

			// read chunk code and length
			chunk_code = file.Read32();
			chunk_size = file.Read32();

			// parse chunk code
			switch( chunk_code )
			{
			//---------------------------------------------------------------------
			case 0x20746D66:	// format chunk
			//---------------------------------------------------------------------

				// check compression code (1 = PCM)
				if( file.Read16() != 1 )
				{
					//if( verbose )
					//	printf( "Unsupported WAV format.\n" );
					//return LOADWAV_UNKNOWN_COMP;
					printf("\nsmconv: error 'Unsupported WAV format'\n" );
					return;
				}

				// read # of channels
				num_channels = file.Read16();

				// read sampling frequency

				C5Speed = file.Read32();
				// skip average something, wBlockAlign
				file.Read32();
				file.Read16();

				// get bit depth, catch unsupported values
				bit_depth = file.Read16();
				if( bit_depth != 8 && bit_depth != 16 )
				{
					//if( verbose )
					//	printf( "Unsupported bit-depth.\n" );
					printf("\nsmconv: error 'Unsupported WAV bit depth'\n");
					return;// LOADWAV_UNSUPPORTED_BD;
				}

				if( bit_depth == 16 )
					Bits16 = true;

				// print verbose data
			// /if( verbose )
			///	{
			//\		printf( "Sample Rate...%i\n", samp->frequency );
			// \	printf( "Bit Depth.....%i-bit\n", bit_depth );
			//	}

				// skip the rest of the chunk (if any)
				if( (chunk_size - 0x10) > 0 )
					file.Skip( (chunk_size - 0x10) );

				hasformat = 1;
				break;

			//---------------------------------------------------------------------
			case 0x61746164:	// data chunk
			//---------------------------------------------------------------------
			{
				int t, dat;
				u32 c;

				if( !hasformat )
				{
					printf("\nsmconv: error 'CORRUPT WAV FILE...'\n");
					return;// LOADWAV_CORRUPT;
				}

			//	if( verbose )
			//		printf( "Loading Sample Data...\n" );

				// clip chunk size against end of file (for some borked wavs...)
				{
					u32 br = file_size - file.Tell();//file_tell_read();
					chunk_size = chunk_size > br ? br : chunk_size;
				}

				Length = chunk_size / (bit_depth/8) / num_channels;

				if( bit_depth == 16 )
					Data16 = new s16[chunk_size/2];
				else
					Data8 = new s8[chunk_size];

				// read sample data
				for( t = 0; t < Length; t++ )
				{
					dat = 0;

					// for multi-channel samples, get average value
					for( c = 0; c < num_channels; c++ )
					{
						dat += bit_depth == 8 ? ((int)file.Read8()) - 128 : ((short)file.Read16());
					}
					dat /= num_channels;

					if( bit_depth == 8 )
					{
						Data8[t] = dat;
					}
					else
					{
						Data16[t] = dat;
					}
				}

				//hasdata = 1;

				break;
			}
			/*
			//------------------------------------------------------------------------------
			case 'lpms':	// sampler chunk
			//------------------------------------------------------------------------------
			{
				int pos;
				skip8( 	4		// manufacturer
						+4		// product
						+4		// sample period
						+4		// midi unity note
						+4		// midi pitch fraction
						+4		// smpte format
						+4		// smpte offset
						);
				int num_sample_loops = read32();

				read32();		// sample data

				pos = 36;

				// check for sample looping data
				if( num_sample_loops )
				{
					read32();	// cue point ID
					int loop_type = read32();
					pos += 8;

					if( loop_type < 2 )
					{
						// sample    | internal
						// 0=forward | 1
						// 1=bidi    | 2
						samp->loop_type = loop_type + 1;
						samp->loop_start = read32();
						samp->loop_end = read32();

						// clip loop start against sample length
						if( samp->loop_end > samp->sample_length ) {
							samp->loop_end = samp->sample_length;
						}

						// disable tiny loop
						// catch invalid loop
						if( (samp->loop_start > samp->sample_length) ||
							(samp->loop_end - samp->loop_start < 16) ) {

							samp->loop_type = 0;
							samp->loop_start = 0;
							samp->loop_end = 0;
						}

						// ignore fractional
						// ignore play count
						pos += 8;
					}
				}

				file.Skip( chunk_size - pos );
				break;
			}	*/
			default:
				file.Skip( chunk_size );
			}
		}
		/*
		if( hasformat && hasdata )
		{
			if( fix ) FixSample( samp );
			return LOADWAV_OK;
		}
		else
		{
			return LOADWAV_CORRUPT;
		}*/
	}
};
