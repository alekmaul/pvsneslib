object Form1: TForm1
  Left = 192
  Top = 124
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'SnesGradient v1.0'
  ClientHeight = 276
  ClientWidth = 836
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 401
    Height = 276
    Align = alLeft
    TabOrder = 1
    object ImageDisplay: TImage
      Left = 8
      Top = 8
      Width = 256
      Height = 224
    end
    object SpeedButtonGradientLoad: TSpeedButton
      Left = 96
      Top = 240
      Width = 92
      Height = 22
      Hint = 'Select a new gradient file and convert it'
      Caption = 'Load gradient'
      ParentShowHint = False
      ShowHint = True
      OnClick = SpeedButtonGradientLoadClick
    end
    object ImageGradient: TImage
      Left = 368
      Top = 128
      Width = 256
      Height = 224
      Visible = False
    end
    object LabelToIndex: TLabel
      Left = 272
      Top = 12
      Width = 44
      Height = 13
      Caption = 'To index:'
    end
    object LabelName: TLabel
      Left = 272
      Top = 42
      Width = 31
      Height = 13
      Caption = 'Name:'
    end
    object EditColorIndex: TEdit
      Left = 317
      Top = 8
      Width = 33
      Height = 21
      Hint = 'Palette index for the Palette1xHDMA format'
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 0
      Text = '0'
    end
    object UpDownColorIndex: TUpDown
      Left = 350
      Top = 8
      Width = 16
      Height = 21
      Associate = EditColorIndex
      Min = 0
      Position = 0
      TabOrder = 1
      Wrap = False
    end
    object Button1: TButton
      Left = 11
      Top = 240
      Width = 75
      Height = 22
      Caption = 'Close'
      ModalResult = 1
      TabOrder = 2
      OnClick = Button1Click
    end
    object RadioButtonTypePalette1: TRadioButton
      Left = 272
      Top = 64
      Width = 113
      Height = 17
      Caption = 'Palette 1xHDMA'
      Checked = True
      TabOrder = 3
      TabStop = True
      OnClick = RadioButtonTypePalette1Click
    end
    object EditName: TEdit
      Left = 317
      Top = 40
      Width = 81
      Height = 21
      Hint = 'Prefix to add to the labels'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
      Text = '0'
    end
    object RadioButtonTypeWindow: TRadioButton
      Left = 272
      Top = 88
      Width = 113
      Height = 17
      Caption = 'WIndows HDMA'
      TabOrder = 5
      OnClick = RadioButtonTypeWindowClick
    end
  end
  object MemoOutput: TMemo
    Left = 401
    Top = 0
    Width = 435
    Height = 276
    Align = alClient
    Lines.Strings = (
      'Select gradient mode'
      'Set nput array name into the text field'
      'Click Load gradient, select a 24-bit BMP file up to 256x224'
      'Copy generated data from this window')
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object OpenDialogGradient: TOpenDialog
    Filter = 'Bitmap|*.bmp'
    Left = 288
    Top = 232
  end
end
