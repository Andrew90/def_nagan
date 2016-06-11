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
     	RegisterHotKey(LirOptionsForm->Handle,         // Handle ����, �������� ���������� ��������� WM_HOTKEY
								0x00F,                 // �������� ������������� �������� �����
								MOD_ALT + MOD_CONTROL, // ������������
								VK_RETURN              // ��� �������
								);
}
//---------------------------------------------------------------------------
void __fastcall TLirOptionsForm::FormActivate(TObject *Sender)
{
    OkBtn->Enabled = false;
	Lir &lir = Lir::Instance();
	LirChange &x = lir.lirChange;
	Lir0Status->Caption = "����������� �������� ���� ��� 1 � ������� " + FloatToStr(x.samplesPerZone0);
	Lir1Status->Caption = "����������� �������� ���� ��� 2 � ������� " + FloatToStr(x.samplesPerZone1);
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
	  Application->MessageBoxW(L"����������� ����� �������� ��� ��� 1",L"!!!",MB_OK);
	  return;
    }
	int l1 = StrToInt(Lir1Edit->Text);
	if(0 == l0)
	{
	   Application->MessageBoxW(L"����������� ����� �������� ��� ��� 2",L"!!!",MB_OK);
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
		if (Msg.message == WM_HOTKEY) // ��������� ����
		   if (Msg.wParam == 0x00F) // ������������� ���
				OkBtn->Enabled ^= true;
}
//---------------------------------------------------------------------------

