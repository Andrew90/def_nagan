//---------------------------------------------------------------------------

#ifndef ADCSettingsH
#define ADCSettingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "cspin.h"
#include "IniFiles.hpp"
#include <AppEvnts.hpp>
#include "Global.h"
#include <ExtCtrls.hpp>
#include <vector>
#include <Registry.hpp>
//---------------------------------------------------------------------------
class TADCSettForm : public TForm
{
__published:	// IDE-managed Components
	TApplicationEvents *ApplicationEvents;
	TButton *bSave;
	TButton *bExit;
	TPageControl *PageControl;
	TTabSheet *ppPage;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TGroupBox *gbPP;
	TEdit *ppKadr;
	TEdit *ppRate;
	TEdit *ppSizeBuffer;
	TTabSheet *prPage;
	TLabel *LabelKadr;
	TLabel *LabelRate;
	TLabel *LabelSizeBuffer;
	TGroupBox *gbPR;
	TEdit *prSizeBuffer;
	TEdit *prRate;
	TEdit *prKadr;
	TTabSheet *OtherPage;
	TLabel *LabelMag;
	TLabel *LabelSG;
	TLabel *lWidthMedianFilter;
	TCheckBox *cbComWithASU;
	TRadioGroup *rgSolidGroupSource;
	TGroupBox *gbTransport;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TComboBox *cbWorkSpeed;
	TComboBox *cbTransitSpeed;
	TComboBox *cbReturnSpeed;
	TGroupBox *gbSolenoids;
	TLabel *LabelMagnetic;
	TLabel *LabelTemp2;
	TLabel *LabelTemp1;
	TLabel *Label4;
	TLabel *NumberCh;
	TCSpinEdit *SpinTemp1;
	TCSpinEdit *SpinTemp2;
	TCSpinEdit *SpinMagnetic;
	TComboBox *ComboBoxMG;
	TComboBox *ComboBoxT2;
	TComboBox *ComboBoxT1;
	TEdit *EditT1;
	TEdit *EditT2;
	TEdit *EditMG;
	TCSpinEdit *SpinCurr;
	TCSpinEdit *SpinSG;
	TCheckBox *cbMedianFilter;
	TComboBox *ComboBoxWidthMF;
	TCheckBox *cbCalcSolidGroup;
	TCSpinEdit *spinTemp;
	TCheckBox *cbTemp;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ApplicationEventsMessage(tagMSG &Msg, bool &Handled);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall bSaveClick(TObject *Sender);
	void __fastcall bExitClick(TObject *Sender);
	void __fastcall ppSizeBufferKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall PageControlChange(TObject *Sender);

private:	// User declarations
	static const int lin_sensors = Globals::LinSensors;
	static const int Cross_sensors = 12;
	static const int Counters = 3;
	TEdit		 *arGainPP		[Cross_sensors];
	TComboBox 	 *arComboBoxPp	[Cross_sensors];
	TCSpinEdit	 *arSpinEditPp	[Cross_sensors];
	TLabel		 *arLabelPp		[Cross_sensors];
	TEdit 		 *arGainPR		[lin_sensors];
	TComboBox 	 *arComboBoxPr	[lin_sensors];
	TLabel		 *arLabelPr		[lin_sensors];
	TCSpinEdit	 *arSpinEditPr	[lin_sensors];
	TLabeledEdit *arPerimiterOfLir[Counters];
	TCSpinEdit   *arLirChannel	  [Counters];
	TIniFile *ini;
	void block();
	bool fl;

	int ReadRange(int r);
	int WriteRange(int r);
	void FillAllControls();

public:		// User declarations
	__fastcall TADCSettForm(TComponent* Owner);


};
//---------------------------------------------------------------------------
extern PACKAGE TADCSettForm *ADCSettForm;
//---------------------------------------------------------------------------
#endif
