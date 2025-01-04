#ifndef IT2SPC_H
#define IT2SPC_H

#include "itloader.h"

namespace IT2SPC {

	//---------------------------------------------
	class Source {
	//---------------------------------------------
	private:
		u16		Length;
		u16		Loop;
		u8		*Data;

		double	TuningFactor;

	public:
		Source(
			const ITLoader::SampleData & );
		~Source();

		std::string id;

		bool	Compare( const Source& ) const;
		void	Export( IO::File &, bool ) const;

		int		GetDataLength() const {
			return Length;
		}

		int GetLoopPoint() const {
			return Loop;
		}

		double GetTuningFactor() const {
			return TuningFactor;
		}
	};

	//---------------------------------------------
	class Sample {
	//---------------------------------------------
	private:
		u8		DefaultVolume;
		u8		GlobalVolume;
		u16		PitchBase;
		u8		DirectoryIndex;
		u8		SetPan;

	public:
		Sample( const ITLoader::Sample &, int, double );

		void Export( IO::File & ) const;
	};

	typedef struct{
		u8	y;
		u8	duration;
		s16 delta;
	} EnvelopeNode;

	//---------------------------------------------
	class Instrument {
	//---------------------------------------------
	private:
		u8		Fadeout;
		u8		SampleIndex;
		u8		GlobalVolume;
		u8		SetPan;
		u8		EnvelopeLength;
		u8		EnvelopeSustain;
		u8		EnvelopeLoopStart;
		u8		EnvelopeLoopEnd;
		EnvelopeNode *EnvelopeData;

	public:
		Instrument( const ITLoader::Instrument & );
		~Instrument();

		int GetExportSize() const {
			return 5 + (!EnvelopeLength) ? 0 : (3 + (EnvelopeLength/4) * 4);
		}

		void Export( IO::File & ) const;
	};

	//---------------------------------------------
	class Pattern {
	//---------------------------------------------
	private:
		u8		Rows;

		std::vector<u8> Data;
	public:
		Pattern( ITLoader::Pattern & );
		~Pattern();

		int GetExportSize() {
			return 1 + Data.size();
		}

		void Export( IO::File &file ) const;
	};

	//---------------------------------------------
	class Module {
	//---------------------------------------------
		u8	InitialVolume;
		u8	InitialTempo;
		u8	InitialSpeed;
		u8	InitialChannelVolume[8];
		u8	InitialChannelPanning[8];

		u8	EchoVolumeL;
		u8	EchoVolumeR;
		u8	EchoDelay;
		u8	EchoFeedback;
		u8	EchoFIR[8];
		u8	EchoEnable;

		u8	Sequence[200];

		// <pointers>
		std::vector<Pattern*> Patterns;
		std::vector<Instrument*> Instruments;
		std::vector<Sample*> Samples;


		void ParseSMOption( const char * );
		void ParseSMOptions( const ITLoader::Module & );

	public:
		Module(
			const ITLoader::Module &,
			const std::vector<u16> &,
			const std::vector<u8> &,
			const std::vector<Source*> & );

		~Module();

		std::string id;
		std::vector<u16> SourceList;
		u32 totalsize;

		int GetExportSize_Header() const {
			return 616;
		}

		void Export( IO::File &file, bool ) const;
	};

	//---------------------------------------------
	class Bank {
	//---------------------------------------------
		std::vector<Module*> Modules;
		std::vector<Source*> Sources;

	public:
		Bank( const ITLoader::Bank &, bool, bool );
		~Bank();

		void AddModule( const ITLoader::Module & );
		int AddSource( Source * );
		void AddSource( const ITLoader::SampleData & );

		bool HiROM;

		void Export( const char * ) const;
		void ExportASM( const char *, const char * ) const;
		void ExportINC( const char * ) const;

		void MakeSPC( const char * ) const;
	};
}

#endif // IT2SPC_H
