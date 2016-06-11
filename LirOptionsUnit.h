//---------------------------------------------------------------------------

#ifndef LirOptionsUnitH
#define LirOptionsUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <AppEvnts.hpp>
//---------------------------------------------------------------------------
class TLirOptionsForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *Lir0Status;
	TLabel *Lir1Status;
	TEdit *Lir0Edit;
	TEdit *Lir1Edit;
	TLabel *Label1;
	TLabel *Label2;
	TButton *OkBtn;
	TButton *CancelBtn;
	TApplicationEvents *ApplicationEvents;
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall OkBtnClick(TObject *Sender);
	void __fastcall CancelBtnClick(TObject *Sender);
	void __fastcall ApplicationEventsMessage(tagMSG &Msg, bool &Handled);





private:	// User declarations
public:		// User declarations
	__fastcall TLirOptionsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLirOptionsForm *LirOptionsForm;
//---------------------------------------------------------------------------
#endif
