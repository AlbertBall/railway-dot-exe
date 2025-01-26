object PerfLogForm: TPerfLogForm
  Left = 0
  Top = 0
  HorzScrollBar.Visible = False
  BorderIcons = []
  BorderWidth = 1
  Caption = 'Performance log: left click here and hold button down to move'
  ClientHeight = 264
  ClientWidth = 464
  Color = clBtnFace
  DefaultMonitor = dmDesktop
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OnCreate = FormCreate
  OnResize = FormResize
  TextHeight = 13
  object PerformanceLogBox: TMemo
    Left = 0
    Top = 0
    Width = 468
    Height = 263
    BorderStyle = bsNone
    DragMode = dmAutomatic
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
  end
end
