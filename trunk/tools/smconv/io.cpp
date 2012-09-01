#include "io.h"

namespace IO {

	File::File() {
		
	}

	File::File( const char *filename, FileAccessMode m ) {
		isOpen = false;
		Open( filename, m );
	}

	File::~File() {
		Close();
	}

	bool File::Open( const char *filename, FileAccessMode m ) {
		if( isOpen ) {
			return false;
		}
		mode = m;
		file = fopen( filename, m ? "wb" : "rb" );

		isOpen = file ? true : false;
		return isOpen;
	}

	void File::Close() {
		if( isOpen ) {
			fclose( file );
			file = 0;
			isOpen = false;
		}
	}

	u8 File::Read8() {
		if( mode == MODE_WRITE || (!isOpen) ) return 0;
		u8 a[1];
		fread( &a, 1, 1, file );
		return a[0];
	}

	u16 File::Read16() {
		u16 a = Read8();
		a |= Read8() << 8;
		return a;
	}

	u32 File::Read32() {
		u32 a = Read16();
		a |= Read16() << 16;
		return a;
	}

	void File::Skip( int amount ) {
		if( isOpen ) {
			if( mode == MODE_WRITE ) {
				for( u32 i = amount; i; i-- ) {
					Write8( 0 );
				}
			} else {
				fseek( file, amount, SEEK_CUR );
			}
		}
	}

	void File::Write8( u8 data ) {
		if( isOpen && mode == MODE_WRITE ) {
			fwrite( &data, 1, 1, file );
		}
	}

	void File::Write16( u16 data ) {
		Write8( data & 0xFF );
		Write8( data >> 8 );
	}

	void File::Write32( u32 data ) {
		Write16( data & 0xFFFF );
		Write16( data >> 16 );
	}

	void File::WriteAscii( const char * str ) {
		for( int i = 0; str[i]; i++ ) {
			Write8( str[i] );
		}
	}

	void File::WriteAsciiF( const char * str, int length ) {
		int i;
		for( i = 0; str[i] && i < length; i++ ) {
			Write8( str[i] );
		}
		for( ; i < length; i++ ) {
			Write8( 0 );
		}
	}
	
	void File::ZeroFill( int amount ) {
		for( int i = 0; i  < amount; i++ ) {
			Write8( 0 );
		}
	}

	void File::WriteAlign( u32 boundary ) {
		int skip = Tell() % boundary;
		if( skip ) {
			Skip( boundary - skip );
		}
	}

	void File::Seek( u32 amount ) {
		if( isOpen )
			fseek( file, amount, SEEK_SET );
	}

	u32 File::Tell() {
		if( isOpen )
			return ftell( file );
		else
			return 0;
	}

	bool File::IsOpen() {
		return isOpen;
	}
	
	bool FileExists( const char *filename ) {
		FILE *f = fopen( filename, "rb" );
		bool result = f ? true : false;
		fclose(f);
		return result;
	}

	u32 FileSize( const char *filename ) {
		FILE *f = fopen( filename, "rb" );
		if( !f ) {
			return 0;
		}
		fseek( f, 0, SEEK_END );
		int size = ftell(f);
		fclose(f);
		return size;
	}
}
