object ActionsDueForm: TActionsDueForm
  Left = 0
  Top = 0
  Margins.Left = 1
  Margins.Top = 1
  Margins.Right = 1
  Margins.Bottom = 1
  VertScrollBar.Size = 1
  VertScrollBar.Style = ssFlat
  VertScrollBar.Visible = False
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = '   Actions Due'
  ClientHeight = 295
  ClientWidth = 100
  Color = clBtnFace
  Constraints.MinHeight = 10
  Constraints.MinWidth = 10
  DefaultMonitor = dmDesktop
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = [fsBold]
  OldCreateOrder = False
  Visible = True
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object ActionsDuePanel: TPanel
    Left = 0
    Top = 0
    Width = 100
    Height = 298
    DragMode = dmAutomatic
    ParentBackground = False
    TabOrder = 0
    object ActionsDuePanelLabel: TLabel
      Left = 32
      Top = 4
      Width = 30
      Height = 13
      Caption = '(mins) '
      DragMode = dmAutomatic
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      WordWrap = True
    end
    object ActionsDueListBox: TListBox
      Left = 11
      Top = 13
      Width = 78
      Height = 270
      BevelEdges = []
      BevelInner = bvNone
      BevelOuter = bvNone
      Constraints.MinHeight = 10
      Constraints.MinWidth = 10
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 0
      TabWidth = 30
      OnMouseDown = ActionsDueListBoxMouseDown
      OnMouseUp = ActionsDueListBoxMouseUp
    end
  end
end
