//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LirOptionsUnit.h"
#include "Lir.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLirOptionsForm *LirOptionsForm;
//---------------------------------------------------------------------------
__fastcall TLirOptionsForm::TLirOptionsForm(TComponent* Owner)
	: TForm(Owner)
{
     	RegisterHotKey(LirOptionsForm->Handle,         // Handle окна, которому отправлять сообщения WM_HOTKEY
								0x00F,                 // УСЛОВНЫЙ идентификатор горячего ключа
								MOD_ALT + MOD_CONTROL, // модификаторы
								VK_RETURN              // код клавиши
								);
}
//---------------------------------------------------------------------------
void __fastcall TLirOptionsForm::FormActivate(TObject *Sender)
{
    OkBtn->Enabled = false;
	Lir &lir = Lir::Instance();
	LirChange &x = lir.lirChange;
	Lir0Status->Caption = "Вычисленное значение зоны ЛИР 1 в отчётах " + FloatToStr(x.samplesPerZone0);
	Lir1Status->Caption = "Вычисленное значение зоны ЛИР 2 в отчётах " + FloatToStr(x.samplesPerZone1);
	int l0, l1;
	lir.GetSamplesLir(l0, l1);
	Lir0Edit->Text = IntToStr(l0);
	Lir1Edit->Text = IntToStr(l1);
}
//---------------------------------------------------------------------------
void __fastcall TLirOptionsForm::OkBtnClick(TObject *Sender)
{
	int l0 = StrToInt(Lir0Edit->Text);
	if(0 == l0)
	{
	  Application->MessageBoxW(L"Некорректно введён параметр для ЛИР 1",L"!!!",MB_OK);
	  return;
    }
	int l1 = StrToInt(Lir1Edit->Text);
	if(0 == l0)
	{
	   Application->MessageBoxW(L"Некорректно введён параметр для ЛИР 2",L"!!!",MB_OK);
	   return;
	}
	Lir::Instance().ChangeSamplesLir(l0, l1);
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TLirOptionsForm::CancelBtnClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TLirOptionsForm::ApplicationEventsMessage(tagMSG &Msg, bool &Handled)
{
		if (Msg.message == WM_HOTKEY) // сообщение наше
		   if (Msg.wParam == 0x00F) // идентификатор наш
				OkBtn->Enabled ^= true;
}
//---------------------------------------------------------------------------

