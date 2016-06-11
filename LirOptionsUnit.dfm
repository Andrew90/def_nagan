object LirOptionsForm: TLirOptionsForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1072' '#1051#1048#1056
  ClientHeight = 211
  ClientWidth = 422
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  PixelsPerInch = 96
  TextHeight = 19
  object Lir0Status: TLabel
    Left = 24
    Top = 8
    Width = 70
    Height = 19
    Caption = 'Lir0Status'
  end
  object Lir1Status: TLabel
    Left = 24
    Top = 33
    Width = 70
    Height = 19
    Caption = 'Lir1Status'
  end
  object Label1: TLabel
    Left = 160
    Top = 82
    Width = 153
    Height = 19
    Caption = #1050#1086#1088#1088#1077#1082#1090#1080#1088#1074#1082#1072' '#1051#1048#1056' 0'
  end
  object Label2: TLabel
    Left = 160
    Top = 115
    Width = 162
    Height = 19
    Caption = #1050#1086#1088#1088#1077#1082#1090#1080#1088#1086#1074#1082#1072' '#1051#1048#1056' 1'
  end
  object Lir0Edit: TEdit
    Left = 24
    Top = 74
    Width = 121
    Height = 27
    TabOrder = 0
    Text = '180'
  end
  object Lir1Edit: TEdit
    Left = 24
    Top = 107
    Width = 121
    Height = 27
    TabOrder = 1
    Text = '180'
  end
  object OkBtn: TButton
    Left = 96
    Top = 160
    Width = 89
    Height = 33
    Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
    Enabled = False
    TabOrder = 2
    OnClick = OkBtnClick
  end
  object CancelBtn: TButton
    Left = 216
    Top = 160
    Width = 89
    Height = 33
    Cancel = True
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 3
    OnClick = CancelBtnClick
  end
  object ApplicationEvents: TApplicationEvents
    OnMessage = ApplicationEventsMessage
    Left = 552
    Top = 8
  end
end
