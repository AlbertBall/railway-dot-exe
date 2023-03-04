object PerfLogForm: TPerfLogForm
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsSingle
  BorderWidth = 1
  Caption = 'Performance log: left click here and hold button down to move'
  ClientHeight = 265
  ClientWidth = 468
  Color = clBtnFace
  DefaultMonitor = dmDesktop
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object PerformanceLogBox: TMemo
    Left = 0
    Top = 20
    Width = 468
    Height = 443
    DragMode = dmAutomatic
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
  end
end
