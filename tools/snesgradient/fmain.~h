//---------------------------------------------------------------------------

#ifndef fmainH
#define fmainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TImage *ImageDisplay;
        TEdit *EditColorIndex;
        TUpDown *UpDownColorIndex;
        TButton *Button1;
        TOpenDialog *OpenDialogGradient;
        TMemo *MemoOutput;
        TRadioButton *RadioButtonTypePalette1;
        TSpeedButton *SpeedButtonGradientLoad;
        TEdit *EditName;
        TImage *ImageGradient;
        TLabel *LabelToIndex;
        TLabel *LabelName;
        TRadioButton *RadioButtonTypeWindow;
        void __fastcall RadioButtonTypePalette1Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
	void __fastcall SpeedButtonGradientConvertClick(TObject *Sender);
        void __fastcall SpeedButtonGradientLoadClick(TObject *Sender);
	void __fastcall SpeedButtonGradientReloadClick(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall RadioButtonTypeWindowClick(TObject *Sender);
        void __fastcall SpeedButtonWindowConvertClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
	void __fastcall UpdateDisplay(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
