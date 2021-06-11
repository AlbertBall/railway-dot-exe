// GraphicUnit.h
/*
      Comments in .h files are believed to be accurate and up to date

      This is a source code file for "railway.exe", a railway operation
      simulator, written originally in Borland C++ Builder 4 Professional with
      later updates in Embarcadero C++Builder 10.2.
      Copyright (C) 2010 Albert Ball [original development]

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// ---------------------------------------------------------------------------
#ifndef GraphicUnitH
#define GraphicUnitH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
// ---------------------------------------------------------------------------
// colour definitions based on the 216 web safe colours. All the graphics in the resource file use these colours
#define clB0G0R0    (TColor)0x000000 //offset, hex values 36  colour number 00
#define clB0G0R1    (TColor)0x000033 //3a   24
#define clB0G0R2    (TColor)0x000066 //3e   48
#define clB0G0R3    (TColor)0x000099 //42   6c
#define clB0G0R4    (TColor)0x0000CC //46   90
#define clB0G0R5    (TColor)0x0000FF //4a   b4

#define clB0G1R0    (TColor)0x003300 //4e   06
#define clB0G1R1    (TColor)0x003333 //52   2a
#define clB0G1R2    (TColor)0x003366 //56   4e
#define clB0G1R3    (TColor)0x003399 //5a   72
#define clB0G1R4    (TColor)0x0033CC //5e   96
#define clB0G1R5    (TColor)0x0033FF //62   ba

#define clB0G2R0    (TColor)0x006600 //66   0c
#define clB0G2R1    (TColor)0x006633 //6a   30
#define clB0G2R2    (TColor)0x006666 //6e   54
#define clB0G2R3    (TColor)0x006699 //72   78
#define clB0G2R4    (TColor)0x0066CC //76   9c
#define clB0G2R5    (TColor)0x0066FF //7a   c0

#define clB0G3R0    (TColor)0x009900 //7e   12
#define clB0G3R1    (TColor)0x009933 //82   36
#define clB0G3R2    (TColor)0x009966 //86   5a
#define clB0G3R3    (TColor)0x009999 //8a   7e
#define clB0G3R4    (TColor)0x0099CC //8e   a2
#define clB0G3R5    (TColor)0x0099FF //92   c6

#define clB0G4R0    (TColor)0x00CC00 //96   18
#define clB0G4R1    (TColor)0x00CC33 //9a   3c
#define clB0G4R2    (TColor)0x00CC66 //9e   60
#define clB0G4R3    (TColor)0x00CC99 //a2   84
#define clB0G4R4    (TColor)0x00CCCC //a6   a8
#define clB0G4R5    (TColor)0x00CCFF //aa   cc

#define clB0G5R0    (TColor)0x00FF00 //ae   1e
#define clB0G5R1    (TColor)0x00FF33 //b2   42
#define clB0G5R2    (TColor)0x00FF66 //b6   66
#define clB0G5R3    (TColor)0x00FF99 //ba   8a
#define clB0G5R4    (TColor)0x00FFCC //be   ae
#define clB0G5R5    (TColor)0x00FFFF //c2   d2

#define clB1G0R0    (TColor)0x330000 //c6   01
#define clB1G0R1    (TColor)0x330033 //ca   25
#define clB1G0R2    (TColor)0x330066 //ce   49
#define clB1G0R3    (TColor)0x330099 //d2   6d
#define clB1G0R4    (TColor)0x3300CC //d6   91
#define clB1G0R5    (TColor)0x3300FF //da   b5

#define clB1G1R0    (TColor)0x333300 //de   07
#define clB1G1R1    (TColor)0x333333 //e2   2b
#define clB1G1R2    (TColor)0x333366 //e6   4f
#define clB1G1R3    (TColor)0x333399 //ea   73
#define clB1G1R4    (TColor)0x3333CC //ee   97
#define clB1G1R5    (TColor)0x3333FF //f2   bb

#define clB1G2R0    (TColor)0x336600 //f6   0d
#define clB1G2R1    (TColor)0x336633 //fa   31
#define clB1G2R2    (TColor)0x336666 //fe   55
#define clB1G2R3    (TColor)0x336699 //102  79
#define clB1G2R4    (TColor)0x3366CC //106  9d
#define clB1G2R5    (TColor)0x3366FF //10a  c1

#define clB1G3R0    (TColor)0x339900 //10e  13
#define clB1G3R1    (TColor)0x339933 //11   37
#define clB1G3R2    (TColor)0x339966 //116  5b
#define clB1G3R3    (TColor)0x339999 //11a  7f
#define clB1G3R4    (TColor)0x3399CC //11e  a3
#define clB1G3R5    (TColor)0x3399FF //122  c7

#define clB1G4R0    (TColor)0x33CC00 //126  19
#define clB1G4R1    (TColor)0x33CC33 //12a  3d
#define clB1G4R2    (TColor)0x33CC66 //12e  61
#define clB1G4R3    (TColor)0x33CC99 //132  85
#define clB1G4R4    (TColor)0x33CCCC //136  a9
#define clB1G4R5    (TColor)0x33CCFF //13a  cd

#define clB1G5R0    (TColor)0x33FF00 //13e  1f
#define clB1G5R1    (TColor)0x33FF33 //142  43
#define clB1G5R2    (TColor)0x33FF66 //146  67
#define clB1G5R3    (TColor)0x33FF99 //14a  8b
#define clB1G5R4    (TColor)0x33FFCC //14e  af
#define clB1G5R5    (TColor)0x33FFFF //152  d3

#define clB2G0R0    (TColor)0x660000 //156  02
#define clB2G0R1    (TColor)0x660033 //15a  26
#define clB2G0R2    (TColor)0x660066 //15e  4a
#define clB2G0R3    (TColor)0x660099 //162  6e
#define clB2G0R4    (TColor)0x6600CC //166  92
#define clB2G0R5    (TColor)0x6600FF //16a  b6

#define clB2G1R0    (TColor)0x663300 //16e  08
#define clB2G1R1    (TColor)0x663333 //172  2c
#define clB2G1R2    (TColor)0x663366 //176  50
#define clB2G1R3    (TColor)0x663399 //17a  74
#define clB2G1R4    (TColor)0x6633CC //17e  98
#define clB2G1R5    (TColor)0x6633FF //182  bc

#define clB2G2R0    (TColor)0x666600 //186  0e
#define clB2G2R1    (TColor)0x666633 //18a  32
#define clB2G2R2    (TColor)0x666666 //18e  56
#define clB2G2R3    (TColor)0x666699 //192  7a
#define clB2G2R4    (TColor)0x6666CC //196  9e
#define clB2G2R5    (TColor)0x6666FF //19a  c2

#define clB2G3R0    (TColor)0x669900 //19e  14
#define clB2G3R1    (TColor)0x669933 //1a2  38
#define clB2G3R2    (TColor)0x669966 //1a6  5c
#define clB2G3R3    (TColor)0x669999 //1aa  80
#define clB2G3R4    (TColor)0x6699CC //1ae  a4
#define clB2G3R5    (TColor)0x6699FF //1b2  c8

#define clB2G4R0    (TColor)0x66CC00 //1b6  1a
#define clB2G4R1    (TColor)0x66CC33 //1ba  3e
#define clB2G4R2    (TColor)0x66CC66 //1be  62
#define clB2G4R3    (TColor)0x66CC99 //1c2  86
#define clB2G4R4    (TColor)0x66CCCC //1c6  aa
#define clB2G4R5    (TColor)0x66CCFF //1ca  ce

#define clB2G5R0    (TColor)0x66FF00 //1ce  20
#define clB2G5R1    (TColor)0x66FF33 //1d2  44
#define clB2G5R2    (TColor)0x66FF66 //1d6  68
#define clB2G5R3    (TColor)0x66FF99 //1da  8c
#define clB2G5R4    (TColor)0x66FFCC //1de  b0
#define clB2G5R5    (TColor)0x66FFFF //1e2  d4

#define clB3G0R0    (TColor)0x990000 //1e6  03
#define clB3G0R1    (TColor)0x990033 //1ea  27
#define clB3G0R2    (TColor)0x990066 //1ee  4b
#define clB3G0R3    (TColor)0x990099 //1f2  6f
#define clB3G0R4    (TColor)0x9900CC //1f6  93
#define clB3G0R5    (TColor)0x9900FF //1fa  b7

#define clB3G1R0    (TColor)0x993300 //1fe  09
#define clB3G1R1    (TColor)0x993333 //202  2d
#define clB3G1R2    (TColor)0x993366 //206  51
#define clB3G1R3    (TColor)0x993399 //20a  75
#define clB3G1R4    (TColor)0x9933CC //20e  99
#define clB3G1R5    (TColor)0x9933FF //212  bd

#define clB3G2R0    (TColor)0x996600 //216  0f
#define clB3G2R1    (TColor)0x996633 //21a  33
#define clB3G2R2    (TColor)0x996666 //21e  57
#define clB3G2R3    (TColor)0x996699 //222  7b
#define clB3G2R4    (TColor)0x9966CC //226  9f
#define clB3G2R5    (TColor)0x9966FF //22a  c3

#define clB3G3R0    (TColor)0x999900 //22e  15
#define clB3G3R1    (TColor)0x999933 //232  39
#define clB3G3R2    (TColor)0x999966 //236  5d
#define clB3G3R3    (TColor)0x999999 //23a  81
#define clB3G3R4    (TColor)0x9999CC //23e  a5
#define clB3G3R5    (TColor)0x9999FF //242  c9

#define clB3G4R0    (TColor)0x99CC00 //246  1b
#define clB3G4R1    (TColor)0x99CC33 //24a  3f
#define clB3G4R2    (TColor)0x99CC66 //24e  63
#define clB3G4R3    (TColor)0x99CC99 //252  87
#define clB3G4R4    (TColor)0x99CCCC //256  ab
#define clB3G4R5    (TColor)0x99CCFF //25a  cf

#define clB3G5R0    (TColor)0x99FF00 //25e  21
#define clB3G5R1    (TColor)0x99FF33 //262  45
#define clB3G5R2    (TColor)0x99FF66 //266  69
#define clB3G5R3    (TColor)0x99FF99 //26a  8d
#define clB3G5R4    (TColor)0x99FFCC //26e  b1
#define clB3G5R5    (TColor)0x99FFFF //272  d5

#define clB4G0R0    (TColor)0xCC0000 //276  04
#define clB4G0R1    (TColor)0xCC0033 //27a  28
#define clB4G0R2    (TColor)0xCC0066 //27e  4c
#define clB4G0R3    (TColor)0xCC0099 //282  70
#define clB4G0R4    (TColor)0xCC00CC //286  94
#define clB4G0R5    (TColor)0xCC00FF //28a  b8

#define clB4G1R0    (TColor)0xCC3300 //28e  0a
#define clB4G1R1    (TColor)0xCC3333 //292  2e
#define clB4G1R2    (TColor)0xCC3366 //296  52
#define clB4G1R3    (TColor)0xCC3399 //29a  76
#define clB4G1R4    (TColor)0xCC33CC //29e  9a
#define clB4G1R5    (TColor)0xCC33FF //2a2  be

#define clB4G2R0    (TColor)0xCC6600 //2a6  10
#define clB4G2R1    (TColor)0xCC6633 //2aa  34
#define clB4G2R2    (TColor)0xCC6666 //2ae  58
#define clB4G2R3    (TColor)0xCC6699 //2b2  7c
#define clB4G2R4    (TColor)0xCC66CC //2b6  a0
#define clB4G2R5    (TColor)0xCC66FF //2ba  c4

#define clB4G3R0    (TColor)0xCC9900 //2be  16
#define clB4G3R1    (TColor)0xCC9933 //2c2  3a
#define clB4G3R2    (TColor)0xCC9966 //2c6  5e
#define clB4G3R3    (TColor)0xCC9999 //2ca  82
#define clB4G3R4    (TColor)0xCC99CC //2ce  a6
#define clB4G3R5    (TColor)0xCC99FF //2d2  ca

#define clB4G4R0    (TColor)0xCCCC00 //2d6  1c
#define clB4G4R1    (TColor)0xCCCC33 //2da  40
#define clB4G4R2    (TColor)0xCCCC66 //2de  64
#define clB4G4R3    (TColor)0xCCCC99 //2e2  88
#define clB4G4R4    (TColor)0xCCCCCC //2e6  ac
#define clB4G4R5    (TColor)0xCCCCFF //2ea  d0

#define clB4G5R0    (TColor)0xCCFF00 //2ee  22
#define clB4G5R1    (TColor)0xCCFF33 //2f2  46
#define clB4G5R2    (TColor)0xCCFF66 //2f6  6a
#define clB4G5R3    (TColor)0xCCFF99 //2fa  8e
#define clB4G5R4    (TColor)0xCCFFCC //2fe  b2
#define clB4G5R5    (TColor)0xCCFFFF //302  d6

#define clB5G0R0    (TColor)0xFF0000 //306  05
#define clB5G0R1    (TColor)0xFF0033 //30a  29
#define clB5G0R2    (TColor)0xFF0066 //30e  4d
#define clB5G0R3    (TColor)0xFF0099 //312  71
#define clB5G0R4    (TColor)0xFF00CC //316  95
#define clB5G0R5    (TColor)0xFF00FF //31a  b9

#define clB5G1R0    (TColor)0xFF3300 //31e  0b
#define clB5G1R1    (TColor)0xFF3333 //322  2f
#define clB5G1R2    (TColor)0xFF3366 //326  53
#define clB5G1R3    (TColor)0xFF3399 //32a  77
#define clB5G1R4    (TColor)0xFF33CC //32e  9b
#define clB5G1R5    (TColor)0xFF33FF //332  bf

#define clB5G2R0    (TColor)0xFF6600 //336  11
#define clB5G2R1    (TColor)0xFF6633 //33a  35
#define clB5G2R2    (TColor)0xFF6666 //33e  59
#define clB5G2R3    (TColor)0xFF6699 //342  7d
#define clB5G2R4    (TColor)0xFF66CC //346  a1
#define clB5G2R5    (TColor)0xFF66FF //34a  c5

#define clB5G3R0    (TColor)0xFF9900 //34e  17
#define clB5G3R1    (TColor)0xFF9933 //352  3b
#define clB5G3R2    (TColor)0xFF9966 //356  5f
#define clB5G3R3    (TColor)0xFF9999 //35a  83
#define clB5G3R4    (TColor)0xFF99CC //35e  a7
#define clB5G3R5    (TColor)0xFF99FF //362  cb

#define clB5G4R0    (TColor)0xFFCC00 //366  1d
#define clB5G4R1    (TColor)0xFFCC33 //36a  41
#define clB5G4R2    (TColor)0xFFCC66 //36e  65
#define clB5G4R3    (TColor)0xFFCC99 //372  89
#define clB5G4R4    (TColor)0xFFCCCC //376  ad
#define clB5G4R5    (TColor)0xFFCCFF //37a  d1

#define clB5G5R0    (TColor)0xFFFF00 //37e  23
#define clB5G5R1    (TColor)0xFFFF33 //382  47
#define clB5G5R2    (TColor)0xFFFF66 //386  6b
#define clB5G5R3    (TColor)0xFFFF99 //38a  8f
#define clB5G5R4    (TColor)0xFFFFCC //38e  b3
#define clB5G5R5    (TColor)0xFFFFFF //392  d7

// train background colour definitions                                         offset  colour
// hex    no.
#define clBufferAttentionNeeded    (TColor)0xFFFF00 //cyan           16776960d     37e    23
#define clBufferStopBackground     (TColor)0xFFFFCC //pale cyan      16777164d     38e    b3
#define clCallOnBackground         (TColor)0xFF33FF //light magenta  16724991d     332    bf
#define clCrashedBackground        (TColor)0x0000FF //red                 255d      4a    b4
#define clDerailedBackground       (TColor)0x0000FF //red                 255d      4a    b4
#define clFrontCodeSignaller       (TColor)0xFF0000 //blue           16711680d     306    05
#define clFrontCodeTimetable       (TColor)0x0000FF //red                 255d      4a    b4
#define clNormalBackground         (TColor)0xCCCCCC //grey           13421772d     2e6    ac
#define clSignallerStopped         (TColor)0x99CCFF //caramel        10079487d     25a    cf
#define clSignalStopBackground     (TColor)0x00FF66 //green             65382d      b6    66
#define clSPADBackground           (TColor)0x00FFFF //yellow            65535d      c2    d2
#define clStationStopBackground    (TColor)0xCCFFCC //pale green     13434828d     2fe    b2
#define clStoppedTrainInFront      (TColor)0xFF9999 //lavender blue  16751001d     35a    83
#define clTRSBackground            (TColor)0xFFCCFF //light pink     16764159d     37a    d1
#define clTrainFailedBackground    (TColor)0x0066FF //orange            26367d      7a    c0

/// Handles graphic data & functions, single object defined
class TRailGraphics
{
public:

// transparent graphics (incorporated in the resource file borland.res)
    Graphics::TBitmap *AddPrefDir;
    Graphics::TBitmap *AddText;
    Graphics::TBitmap *AddTrack;
    Graphics::TBitmap *AutoSig;
    Graphics::TBitmap *CallingOn;
    Graphics::TBitmap *ClearAllPrefDir;
    Graphics::TBitmap *ClearOnePrefDir;
    Graphics::TBitmap *ConnectGaps;
    Graphics::TBitmap *Exit;
    Graphics::TBitmap *FontGraphic;
    Graphics::TBitmap *Hide;
    Graphics::TBitmap *MoveTextOrGraphic;
    Graphics::TBitmap *NameLocs;
    Graphics::TBitmap *NonSig;
    Graphics::TBitmap *PictureImage;
    Graphics::TBitmap *PrefTop;
    Graphics::TBitmap *PrefBottom;
    Graphics::TBitmap *RouteCancel;
    Graphics::TBitmap *SaveRailway;
    Graphics::TBitmap *SaveSession;
    Graphics::TBitmap *ScreenGrid;
    Graphics::TBitmap *SetDists;
    Graphics::TBitmap *Show;
    Graphics::TBitmap *Validate;
    Graphics::TBitmap *DistanceKey;
    Graphics::TBitmap *PrefDirKey;
    Graphics::TBitmap *BufferWarning;
    Graphics::TBitmap *CrashWarning;
    Graphics::TBitmap *DerailWarning;
    Graphics::TBitmap *SignalStopWarning;
    Graphics::TBitmap *SPADWarning;
    Graphics::TBitmap *BlackArrowDown;
    Graphics::TBitmap *BlackArrowLeft;
    Graphics::TBitmap *BlackArrowRight;
    Graphics::TBitmap *BlackArrowUp;
    Graphics::TBitmap *bm10;
    Graphics::TBitmap *bm100;
    Graphics::TBitmap *bm101;
    Graphics::TBitmap *bm106;
    Graphics::TBitmap *bm10Diverging;
    Graphics::TBitmap *bm10Straight;
    Graphics::TBitmap *bm11;
    Graphics::TBitmap *bm11Diverging;
    Graphics::TBitmap *bm11Straight;
    Graphics::TBitmap *bm12;
    Graphics::TBitmap *bm12Diverging;
    Graphics::TBitmap *bm12Straight;
    Graphics::TBitmap *bm13;
    Graphics::TBitmap *bm132;
    Graphics::TBitmap *bm132LeftFork;
    Graphics::TBitmap *bm132RightFork;
    Graphics::TBitmap *bm133;
    Graphics::TBitmap *bm133LeftFork;
    Graphics::TBitmap *bm133RightFork;
    Graphics::TBitmap *bm134;
    Graphics::TBitmap *bm134LeftFork;
    Graphics::TBitmap *bm134RightFork;
    Graphics::TBitmap *bm135;
    Graphics::TBitmap *bm135LeftFork;
    Graphics::TBitmap *bm135RightFork;
    Graphics::TBitmap *bm136;
    Graphics::TBitmap *bm136LeftFork;
    Graphics::TBitmap *bm136RightFork;
    Graphics::TBitmap *bm137;
    Graphics::TBitmap *bm137LeftFork;
    Graphics::TBitmap *bm137RightFork;
    Graphics::TBitmap *bm138;
    Graphics::TBitmap *bm138LeftFork;
    Graphics::TBitmap *bm138RightFork;
    Graphics::TBitmap *bm139;
    Graphics::TBitmap *bm139LeftFork;
    Graphics::TBitmap *bm139RightFork;
    Graphics::TBitmap *bm13Diverging;
    Graphics::TBitmap *bm13Straight;
    Graphics::TBitmap *bm14;
    Graphics::TBitmap *bm140;
    Graphics::TBitmap *bm141;
    Graphics::TBitmap *bm14Diverging;
    Graphics::TBitmap *bm14Straight;
    Graphics::TBitmap *bm16;
    Graphics::TBitmap *bm18;
    Graphics::TBitmap *bm20;
    Graphics::TBitmap *bm27;
    Graphics::TBitmap *bm28;
    Graphics::TBitmap *bm28Diverging;
    Graphics::TBitmap *bm28Straight;
    Graphics::TBitmap *bm29;
    Graphics::TBitmap *bm29Diverging;
    Graphics::TBitmap *bm29Straight;
    Graphics::TBitmap *bm30;
    Graphics::TBitmap *bm30Diverging;
    Graphics::TBitmap *bm30Straight;
    Graphics::TBitmap *bm31;
    Graphics::TBitmap *bm31Diverging;
    Graphics::TBitmap *bm31Straight;
    Graphics::TBitmap *bm32;
    Graphics::TBitmap *bm32Diverging;
    Graphics::TBitmap *bm32Straight;
    Graphics::TBitmap *bm33;
    Graphics::TBitmap *bm33Diverging;
    Graphics::TBitmap *bm33Straight;
    Graphics::TBitmap *bm34;
    Graphics::TBitmap *bm34Diverging;
    Graphics::TBitmap *bm34Straight;
    Graphics::TBitmap *bm35;
    Graphics::TBitmap *bm35Diverging;
    Graphics::TBitmap *bm35Straight;
    Graphics::TBitmap *bm36;
    Graphics::TBitmap *bm36Diverging;
    Graphics::TBitmap *bm36Straight;
    Graphics::TBitmap *bm37;
    Graphics::TBitmap *bm37Diverging;
    Graphics::TBitmap *bm37Straight;
    Graphics::TBitmap *bm38;
    Graphics::TBitmap *bm38Diverging;
    Graphics::TBitmap *bm38Straight;
    Graphics::TBitmap *bm39;
    Graphics::TBitmap *bm39Diverging;
    Graphics::TBitmap *bm39Straight;
    Graphics::TBitmap *bm40;
    Graphics::TBitmap *bm40Diverging;
    Graphics::TBitmap *bm40Straight;
    Graphics::TBitmap *bm41;
    Graphics::TBitmap *bm41Diverging;
    Graphics::TBitmap *bm41Straight;
    Graphics::TBitmap *bm42;
    Graphics::TBitmap *bm42Diverging;
    Graphics::TBitmap *bm42Straight;
    Graphics::TBitmap *bm43;
    Graphics::TBitmap *bm43Diverging;
    Graphics::TBitmap *bm43Straight;
    Graphics::TBitmap *bm45;
    Graphics::TBitmap *bm46;
    Graphics::TBitmap *bm50;
    Graphics::TBitmap *bm51;
    Graphics::TBitmap *bm53;
    Graphics::TBitmap *bm54;
    Graphics::TBitmap *bm56;
    Graphics::TBitmap *bm59;
    Graphics::TBitmap *bm65;
    Graphics::TBitmap *bm68CallingOn;
    Graphics::TBitmap *bm68dblyellow;
    Graphics::TBitmap *bm68grounddblred;
    Graphics::TBitmap *bm68grounddblwhite;
    Graphics::TBitmap *bm68green;
    Graphics::TBitmap *bm68yellow;
    Graphics::TBitmap *bm69CallingOn;
    Graphics::TBitmap *bm69dblyellow;
    Graphics::TBitmap *bm69grounddblred;
    Graphics::TBitmap *bm69grounddblwhite;
    Graphics::TBitmap *bm69green;
    Graphics::TBitmap *bm69yellow;
    Graphics::TBitmap *bm7;
    Graphics::TBitmap *bm70CallingOn;
    Graphics::TBitmap *bm70dblyellow;
    Graphics::TBitmap *bm70grounddblred;
    Graphics::TBitmap *bm70grounddblwhite;
    Graphics::TBitmap *bm70green;
    Graphics::TBitmap *bm70yellow;
    Graphics::TBitmap *bm71CallingOn;
    Graphics::TBitmap *bm71dblyellow;
    Graphics::TBitmap *bm71grounddblred;
    Graphics::TBitmap *bm71grounddblwhite;
    Graphics::TBitmap *bm71green;
    Graphics::TBitmap *bm71yellow;
    Graphics::TBitmap *bm72CallingOn;
    Graphics::TBitmap *bm72dblyellow;
    Graphics::TBitmap *bm72grounddblred;
    Graphics::TBitmap *bm72grounddblwhite;
    Graphics::TBitmap *bm72green;
    Graphics::TBitmap *bm72yellow;
    Graphics::TBitmap *bm73;
    Graphics::TBitmap *bm73CallingOn;
    Graphics::TBitmap *bm73dblyellow;
    Graphics::TBitmap *bm73grounddblred;
    Graphics::TBitmap *bm73grounddblwhite;
    Graphics::TBitmap *bm73green;
    Graphics::TBitmap *bm73yellow;
    Graphics::TBitmap *bm74;
    Graphics::TBitmap *bm74CallingOn;
    Graphics::TBitmap *bm74dblyellow;
    Graphics::TBitmap *bm74grounddblred;
    Graphics::TBitmap *bm74grounddblwhite;
    Graphics::TBitmap *bm74green;
    Graphics::TBitmap *bm74yellow;
    Graphics::TBitmap *bm75CallingOn;
    Graphics::TBitmap *bm75dblyellow;
    Graphics::TBitmap *bm75grounddblred;
    Graphics::TBitmap *bm75grounddblwhite;
    Graphics::TBitmap *bm75green;
    Graphics::TBitmap *bm75yellow;
    Graphics::TBitmap *bm77;
    Graphics::TBitmap *bm77Striped;
    Graphics::TBitmap *bm78;
    Graphics::TBitmap *bm78Striped;
    Graphics::TBitmap *bm7Diverging;
    Graphics::TBitmap *bm7Straight;
    Graphics::TBitmap *bm8;
    Graphics::TBitmap *bm85;
    Graphics::TBitmap *bm8Diverging;
    Graphics::TBitmap *bm8Straight;
    Graphics::TBitmap *bm9;
    Graphics::TBitmap *bm93set;
    Graphics::TBitmap *bm93unset;
    Graphics::TBitmap *bm94set;
    Graphics::TBitmap *bm94unset;
    Graphics::TBitmap *bm9Diverging;
    Graphics::TBitmap *bm9Straight;
    Graphics::TBitmap *bmGreenEllipse;
    Graphics::TBitmap *bmGreenRect;
    Graphics::TBitmap *bmGrid;
    Graphics::TBitmap *bmLightBlueRect;
    Graphics::TBitmap *bmName;
    Graphics::TBitmap *bmNameStriped;
    Graphics::TBitmap *bmRedEllipse;
    Graphics::TBitmap *bmRedRect;
    Graphics::TBitmap *bmRtCancELink1;
    Graphics::TBitmap *bmRtCancELink2;
    Graphics::TBitmap *bmRtCancELink3;
    Graphics::TBitmap *bmRtCancELink4;
    Graphics::TBitmap *bmRtCancELink6;
    Graphics::TBitmap *bmRtCancELink7;
    Graphics::TBitmap *bmRtCancELink8;
    Graphics::TBitmap *bmRtCancELink9;
    Graphics::TBitmap *br1;
    Graphics::TBitmap *br10;
    Graphics::TBitmap *br11;
    Graphics::TBitmap *br12;
    Graphics::TBitmap *br2;
    Graphics::TBitmap *br3;
    Graphics::TBitmap *br4;
    Graphics::TBitmap *br5;
    Graphics::TBitmap *br6;
    Graphics::TBitmap *br7;
    Graphics::TBitmap *br8;
    Graphics::TBitmap *br9;
    Graphics::TBitmap *Concourse;
    Graphics::TBitmap *ConcourseGlyph;
    Graphics::TBitmap *ConcourseStriped;
    Graphics::TBitmap *ELk1;
    Graphics::TBitmap *ELk2;
    Graphics::TBitmap *ELk3;
    Graphics::TBitmap *ELk4;
    Graphics::TBitmap *ELk6;
    Graphics::TBitmap *ELk7;
    Graphics::TBitmap *ELk8;
    Graphics::TBitmap *ELk9;
    Graphics::TBitmap *GapsNotSetGraphic;
    Graphics::TBitmap *GapsSetGraphic;
    Graphics::TBitmap *gl1;
    Graphics::TBitmap *gl10;
    Graphics::TBitmap *gl100;
    Graphics::TBitmap *gl101;
    Graphics::TBitmap *gl102;
    Graphics::TBitmap *gl103;
    Graphics::TBitmap *gl104;
    Graphics::TBitmap *gl105;
    Graphics::TBitmap *gl106;
    Graphics::TBitmap *gl107;
    Graphics::TBitmap *gl108;
    Graphics::TBitmap *gl109;
    Graphics::TBitmap *gl11;
    Graphics::TBitmap *gl110;
    Graphics::TBitmap *gl111;
    Graphics::TBitmap *gl112;
    Graphics::TBitmap *gl113;
    Graphics::TBitmap *gl114;
    Graphics::TBitmap *gl115;
    Graphics::TBitmap *gl116;
    Graphics::TBitmap *gl117;
    Graphics::TBitmap *gl118;
    Graphics::TBitmap *gl119;
    Graphics::TBitmap *gl12;
    Graphics::TBitmap *gl120;
    Graphics::TBitmap *gl121;
    Graphics::TBitmap *gl122;
    Graphics::TBitmap *gl123;
    Graphics::TBitmap *gl124;
    Graphics::TBitmap *gl125;
    Graphics::TBitmap *gl126;
    Graphics::TBitmap *gl127;
    Graphics::TBitmap *gl128;
    Graphics::TBitmap *gl129;
    Graphics::TBitmap *gl129Striped;
    Graphics::TBitmap *gl13;
    Graphics::TBitmap *gl130;
    Graphics::TBitmap *gl130Striped;
    Graphics::TBitmap *gl131;
    Graphics::TBitmap *gl132;
    Graphics::TBitmap *gl133;
    Graphics::TBitmap *gl134;
    Graphics::TBitmap *gl135;
    Graphics::TBitmap *gl136;
    Graphics::TBitmap *gl137;
    Graphics::TBitmap *gl138;
    Graphics::TBitmap *gl139;
    Graphics::TBitmap *gl14;
    Graphics::TBitmap *gl140;
    Graphics::TBitmap *gl141;
    Graphics::TBitmap *gl142;
    Graphics::TBitmap *gl143;
    Graphics::TBitmap *gl145;
    Graphics::TBitmap *gl145Striped;
    Graphics::TBitmap *gl146;
    Graphics::TBitmap *gl146Striped;
    Graphics::TBitmap *gl15;
    Graphics::TBitmap *gl16;
    Graphics::TBitmap *gl18;
    Graphics::TBitmap *gl19;
    Graphics::TBitmap *gl2;
    Graphics::TBitmap *gl20;
    Graphics::TBitmap *gl21;
    Graphics::TBitmap *gl22;
    Graphics::TBitmap *gl23;
    Graphics::TBitmap *gl24;
    Graphics::TBitmap *gl25;
    Graphics::TBitmap *gl26;
    Graphics::TBitmap *gl27;
    Graphics::TBitmap *gl28;
    Graphics::TBitmap *gl29;
    Graphics::TBitmap *gl3;
    Graphics::TBitmap *gl30;
    Graphics::TBitmap *gl31;
    Graphics::TBitmap *gl32;
    Graphics::TBitmap *gl33;
    Graphics::TBitmap *gl34;
    Graphics::TBitmap *gl35;
    Graphics::TBitmap *gl36;
    Graphics::TBitmap *gl37;
    Graphics::TBitmap *gl38;
    Graphics::TBitmap *gl39;
    Graphics::TBitmap *gl4;
    Graphics::TBitmap *gl40;
    Graphics::TBitmap *gl41;
    Graphics::TBitmap *gl42;
    Graphics::TBitmap *gl43;
    Graphics::TBitmap *gl44;
    Graphics::TBitmap *gl45;
    Graphics::TBitmap *gl46;
    Graphics::TBitmap *gl47;
    Graphics::TBitmap *gl48;
    Graphics::TBitmap *gl49;
    Graphics::TBitmap *gl5;
    Graphics::TBitmap *gl50;
    Graphics::TBitmap *gl51;
    Graphics::TBitmap *gl52;
    Graphics::TBitmap *gl53;
    Graphics::TBitmap *gl54;
    Graphics::TBitmap *gl55;
    Graphics::TBitmap *gl56;
    Graphics::TBitmap *gl57;
    Graphics::TBitmap *gl58;
    Graphics::TBitmap *gl59;
    Graphics::TBitmap *gl6;
    Graphics::TBitmap *gl60;
    Graphics::TBitmap *gl61;
    Graphics::TBitmap *gl62;
    Graphics::TBitmap *gl63;
    Graphics::TBitmap *gl64;
    Graphics::TBitmap *gl65;
    Graphics::TBitmap *gl66;
    Graphics::TBitmap *gl67;
    Graphics::TBitmap *gl68;
    Graphics::TBitmap *gl69;
    Graphics::TBitmap *gl7;
    Graphics::TBitmap *gl70;
    Graphics::TBitmap *gl71;
    Graphics::TBitmap *gl72;
    Graphics::TBitmap *gl73;
    Graphics::TBitmap *gl73grounddblred;
    Graphics::TBitmap *gl74;
    Graphics::TBitmap *gl74grounddblred;
    Graphics::TBitmap *gl75;
    Graphics::TBitmap *gl76;
    Graphics::TBitmap *gl76Striped;
    Graphics::TBitmap *gl77;
    Graphics::TBitmap *gl78;
    Graphics::TBitmap *gl79;
    Graphics::TBitmap *gl79Striped;
    Graphics::TBitmap *gl8;
    Graphics::TBitmap *gl80;
    Graphics::TBitmap *gl81;
    Graphics::TBitmap *gl82;
    Graphics::TBitmap *gl83;
    Graphics::TBitmap *gl84;
    Graphics::TBitmap *gl85;
    Graphics::TBitmap *gl86;
    Graphics::TBitmap *gl87;
    Graphics::TBitmap *gl88set;
    Graphics::TBitmap *gl88unset;
    Graphics::TBitmap *gl89set;
    Graphics::TBitmap *gl89unset;
    Graphics::TBitmap *gl9;
    Graphics::TBitmap *gl90set;
    Graphics::TBitmap *gl90unset;
    Graphics::TBitmap *gl91set;
    Graphics::TBitmap *gl91unset;
    Graphics::TBitmap *gl92set;
    Graphics::TBitmap *gl92unset;
    Graphics::TBitmap *gl93set;
    Graphics::TBitmap *gl94set;
    Graphics::TBitmap *gl95set;
    Graphics::TBitmap *gl95unset;
    Graphics::TBitmap *gl97;
    Graphics::TBitmap *gl98;
    Graphics::TBitmap *gl99;
    Graphics::TBitmap *HideLog;
    Graphics::TBitmap *Home;
    Graphics::TBitmap *LCBothHor;
    Graphics::TBitmap *LCBotHor;
    Graphics::TBitmap *LCBothVer;
    Graphics::TBitmap *LCLHSVer;
    Graphics::TBitmap *LCPlain;
    Graphics::TBitmap *LCRHSVer;
    Graphics::TBitmap *LCTopHor;
    Graphics::TBitmap *LCBothHorMan;
    Graphics::TBitmap *LCBotHorMan;
    Graphics::TBitmap *LCBothVerMan;
    Graphics::TBitmap *LCLHSVerMan;
    Graphics::TBitmap *LCPlainMan;
    Graphics::TBitmap *LCRHSVerMan;
    Graphics::TBitmap *LCTopHorMan;
    Graphics::TBitmap *LocNamesNotSetGraphic;
    Graphics::TBitmap *LocNamesSetGraphic;
    Graphics::TBitmap *NewHome;
    Graphics::TBitmap *PauseGraphic;
    Graphics::TBitmap *PixelPrecision1;
    Graphics::TBitmap *PixelPrecision16;
    Graphics::TBitmap *PixelPrecision2;
    Graphics::TBitmap *PixelPrecision4;
    Graphics::TBitmap *PixelPrecision8;
    Graphics::TBitmap *Plat68;
    Graphics::TBitmap *Plat68Striped;
    Graphics::TBitmap *Plat69;
    Graphics::TBitmap *Plat69Striped;
    Graphics::TBitmap *Plat70;
    Graphics::TBitmap *Plat70Striped;
    Graphics::TBitmap *Plat71;
    Graphics::TBitmap *Plat71Striped;
    Graphics::TBitmap *RunGraphic;
    Graphics::TBitmap *ShowLog;
    Graphics::TBitmap *sm1;
    Graphics::TBitmap *sm10;
    Graphics::TBitmap *sm100;
    Graphics::TBitmap *sm101;
    Graphics::TBitmap *sm102;
    Graphics::TBitmap *sm103;
    Graphics::TBitmap *sm104;
    Graphics::TBitmap *sm105;
    Graphics::TBitmap *sm106;
    Graphics::TBitmap *sm107;
    Graphics::TBitmap *sm108;
    Graphics::TBitmap *sm109;
    Graphics::TBitmap *sm11;
    Graphics::TBitmap *sm110;
    Graphics::TBitmap *sm111;
    Graphics::TBitmap *sm112;
    Graphics::TBitmap *sm115;
    Graphics::TBitmap *sm117;
    Graphics::TBitmap *sm12;
    Graphics::TBitmap *sm129;
    Graphics::TBitmap *sm129striped;
    Graphics::TBitmap *sm13;
    Graphics::TBitmap *sm130;
    Graphics::TBitmap *sm130striped;
    Graphics::TBitmap *sm131striped;
    Graphics::TBitmap *sm132;
    Graphics::TBitmap *sm133;
    Graphics::TBitmap *sm134;
    Graphics::TBitmap *sm135;
    Graphics::TBitmap *sm136;
    Graphics::TBitmap *sm137;
    Graphics::TBitmap *sm138;
    Graphics::TBitmap *sm139;
    Graphics::TBitmap *sm14;
    Graphics::TBitmap *sm15;
    Graphics::TBitmap *sm16;
    Graphics::TBitmap *sm18;
    Graphics::TBitmap *sm19;
    Graphics::TBitmap *sm2;
    Graphics::TBitmap *sm20;
    Graphics::TBitmap *sm21;
    Graphics::TBitmap *sm22;
    Graphics::TBitmap *sm23;
    Graphics::TBitmap *sm24;
    Graphics::TBitmap *sm25;
    Graphics::TBitmap *sm26;
    Graphics::TBitmap *sm27;
    Graphics::TBitmap *sm28;
    Graphics::TBitmap *sm29;
    Graphics::TBitmap *sm3;
    Graphics::TBitmap *sm30;
    Graphics::TBitmap *sm31;
    Graphics::TBitmap *sm32;
    Graphics::TBitmap *sm33;
    Graphics::TBitmap *sm34;
    Graphics::TBitmap *sm35;
    Graphics::TBitmap *sm36;
    Graphics::TBitmap *sm37;
    Graphics::TBitmap *sm38;
    Graphics::TBitmap *sm39;
    Graphics::TBitmap *sm4;
    Graphics::TBitmap *sm40;
    Graphics::TBitmap *sm41;
    Graphics::TBitmap *sm42;
    Graphics::TBitmap *sm43;
    Graphics::TBitmap *sm44;
    Graphics::TBitmap *sm45;
    Graphics::TBitmap *sm46;
    Graphics::TBitmap *sm47;
    Graphics::TBitmap *sm48;
    Graphics::TBitmap *sm49;
    Graphics::TBitmap *sm5;
    Graphics::TBitmap *sm50;
    Graphics::TBitmap *sm51;
    Graphics::TBitmap *sm52;
    Graphics::TBitmap *sm53;
    Graphics::TBitmap *sm54;
    Graphics::TBitmap *sm55;
    Graphics::TBitmap *sm56;
    Graphics::TBitmap *sm57;
    Graphics::TBitmap *sm58;
    Graphics::TBitmap *sm59;
    Graphics::TBitmap *sm6;
    Graphics::TBitmap *sm60;
    Graphics::TBitmap *sm61;
    Graphics::TBitmap *sm62;
    Graphics::TBitmap *sm63;
    Graphics::TBitmap *sm64;
    Graphics::TBitmap *sm65;
    Graphics::TBitmap *sm66;
    Graphics::TBitmap *sm67;
    Graphics::TBitmap *sm7;
    Graphics::TBitmap *sm76;
    Graphics::TBitmap *sm76striped;
    Graphics::TBitmap *sm77;
    Graphics::TBitmap *sm77striped;
    Graphics::TBitmap *sm78;
    Graphics::TBitmap *sm78striped;
    Graphics::TBitmap *sm79;
    Graphics::TBitmap *sm79striped;
    Graphics::TBitmap *sm8;
    Graphics::TBitmap *sm80;
    Graphics::TBitmap *sm81;
    Graphics::TBitmap *sm82;
    Graphics::TBitmap *sm83;
    Graphics::TBitmap *sm84;
    Graphics::TBitmap *sm85;
    Graphics::TBitmap *sm86;
    Graphics::TBitmap *sm87;
    Graphics::TBitmap *sm88;
    Graphics::TBitmap *sm89;
    Graphics::TBitmap *sm9;
    Graphics::TBitmap *sm90;
    Graphics::TBitmap *sm91;
    Graphics::TBitmap *sm92;
    Graphics::TBitmap *sm93;
    Graphics::TBitmap *sm94;
    Graphics::TBitmap *sm95;
    Graphics::TBitmap *sm96;
    Graphics::TBitmap *sm96striped;
    Graphics::TBitmap *sm97;
    Graphics::TBitmap *sm98;
    Graphics::TBitmap *sm99;
    Graphics::TBitmap *smBlack;
    Graphics::TBitmap *smBlue;
    Graphics::TBitmap *smBrightGreen;
    Graphics::TBitmap *smCaramel;
    Graphics::TBitmap *smCyan;
    Graphics::TBitmap *smLC; //added v2.9.0 to show in zoom out mode
    Graphics::TBitmap *smLightBlue;
    Graphics::TBitmap *smMagenta;
    Graphics::TBitmap *smName;
    Graphics::TBitmap *smOrange;
    Graphics::TBitmap *smPaleGreen;
    Graphics::TBitmap *smRed;
    Graphics::TBitmap *smYellow;
    Graphics::TBitmap *smTransparent;
    Graphics::TBitmap *TempBackground;
    Graphics::TBitmap *TempHeadCode;
    Graphics::TBitmap *TrackLinkedGraphic;
    Graphics::TBitmap *TrackNotLinkedGraphic;
    Graphics::TBitmap *UnderHFootbridge;
    Graphics::TBitmap *UnderVFootbridge;
    Graphics::TBitmap *ZoomIn;
    Graphics::TBitmap *ZoomOut;

// The following are created as new bitmaps from existing .bmp files
    Graphics::TBitmap *bmTransparentBgnd;
    Graphics::TBitmap *GridBitmap;

// The following are not created, they are just copies of existing bitmap pointers
    Graphics::TBitmap *sm68;
    Graphics::TBitmap *sm69;
    Graphics::TBitmap *sm70;
    Graphics::TBitmap *sm71;
    Graphics::TBitmap *sm72;
    Graphics::TBitmap *sm73;
    Graphics::TBitmap *sm74;
    Graphics::TBitmap *sm75;
    Graphics::TBitmap *sm113;
    Graphics::TBitmap *sm114;
    Graphics::TBitmap *sm116;
    Graphics::TBitmap *sm118;
    Graphics::TBitmap *sm119;
    Graphics::TBitmap *sm120;
    Graphics::TBitmap *sm121;
    Graphics::TBitmap *sm122;
    Graphics::TBitmap *sm123;
    Graphics::TBitmap *sm124;
    Graphics::TBitmap *sm125;
    Graphics::TBitmap *sm126;
    Graphics::TBitmap *sm127;
    Graphics::TBitmap *sm128;

// non-transparent graphics (incorporated in the resource file borland.res)
    Graphics::TBitmap *Code_a;
    Graphics::TBitmap *Code_b;
    Graphics::TBitmap *Code_c;
    Graphics::TBitmap *Code_d;
    Graphics::TBitmap *Code_e;
    Graphics::TBitmap *Code_f;
    Graphics::TBitmap *Code_g;
    Graphics::TBitmap *Code_h;
    Graphics::TBitmap *Code_i;
    Graphics::TBitmap *Code_j;
    Graphics::TBitmap *Code_k;
    Graphics::TBitmap *Code_l;
    Graphics::TBitmap *Code_m;
    Graphics::TBitmap *Code_n;
    Graphics::TBitmap *Code_o;
    Graphics::TBitmap *Code_p;
    Graphics::TBitmap *Code_q;
    Graphics::TBitmap *Code_r;
    Graphics::TBitmap *Code_s;
    Graphics::TBitmap *Code_t;
    Graphics::TBitmap *Code_u;
    Graphics::TBitmap *Code_v;
    Graphics::TBitmap *Code_w;
    Graphics::TBitmap *Code_x;
    Graphics::TBitmap *Code_y;
    Graphics::TBitmap *Code_z;
    Graphics::TBitmap *Code0;
    Graphics::TBitmap *Code1;
    Graphics::TBitmap *Code2;
    Graphics::TBitmap *Code3;
    Graphics::TBitmap *Code4;
    Graphics::TBitmap *Code5;
    Graphics::TBitmap *Code6;
    Graphics::TBitmap *Code7;
    Graphics::TBitmap *Code8;
    Graphics::TBitmap *Code9;
    Graphics::TBitmap *CodeA;
    Graphics::TBitmap *CodeB;
    Graphics::TBitmap *CodeC;
    Graphics::TBitmap *CodeD;
    Graphics::TBitmap *CodeE;
    Graphics::TBitmap *CodeF;
    Graphics::TBitmap *CodeG;
    Graphics::TBitmap *CodeH;
    Graphics::TBitmap *CodeI;
    Graphics::TBitmap *CodeJ;
    Graphics::TBitmap *CodeK;
    Graphics::TBitmap *CodeL;
    Graphics::TBitmap *CodeM;
    Graphics::TBitmap *CodeN;
    Graphics::TBitmap *CodeO;
    Graphics::TBitmap *CodeP;
    Graphics::TBitmap *CodeQ;
    Graphics::TBitmap *CodeR;
    Graphics::TBitmap *CodeS;
    Graphics::TBitmap *CodeT;
    Graphics::TBitmap *CodeU;
    Graphics::TBitmap *CodeV;
    Graphics::TBitmap *CodeW;
    Graphics::TBitmap *CodeX;
    Graphics::TBitmap *CodeY;
    Graphics::TBitmap *CodeZ;
    Graphics::TBitmap *bmSolidBgnd;
    Graphics::TBitmap *smSolidBgnd;
    Graphics::TBitmap *bmDiagonalSignalBlank;
    Graphics::TBitmap *bmPointBlank;
    Graphics::TBitmap *bmStraightEWSignalBlank;
    Graphics::TBitmap *bmStraightNSSignalBlank;

// graphics below created from existing graphics
// 1st group is for underbridge graphic segements - i.e. the lower single track segment (the overbridge segment is the normal track
// segment, 12 bridges
    Graphics::TBitmap *BridgeGraphicsPtr[12];
///< basic graphic for use in plotting the original graphic during route flashing
    Graphics::TBitmap *BridgePrefDirGraphicsPtr[12];
///< preferred direction graphic overlay
    Graphics::TBitmap *BridgeSigRouteGraphicsPtr[12];
///< route graphic for preferred routes overlay
    Graphics::TBitmap *BridgeNonSigRouteGraphicsPtr[12];
///< route graphic for unrestricted route overlay
    Graphics::TBitmap *BridgeRouteAutoSigsGraphicsPtr[12];
///< route graphic for automatic signal routes overlay

// 2nd group for ordinary single track segments - 16 for non underbridges & non-underfootbridges (track joining each valid pair of the 8 track element links),
// 4 for diagonal buffers & 8 for continuations, 2 for footbridges (not underpasses - these have track on top so included in first 16)
    Graphics::TBitmap *LinkGraphicsPtr[30];
///< basic single track graphic for use in plotting the original graphic during route flashing
    Graphics::TBitmap *LinkPrefDirGraphicsPtr[30];
///< preferred direction graphic overlay
    Graphics::TBitmap *LinkSigRouteGraphicsPtr[30];
///< preferred direction route graphic overlay
    Graphics::TBitmap *LinkNonSigRouteGraphicsPtr[30];
///< unrestricted route graphic overlay
    Graphics::TBitmap *LinkRouteAutoSigsGraphicsPtr[30];
///< auto signal route graphic overlay

// 3rd group for the direction arrows that indicate the direction of the route or of the preferred direction - 1 for each of 8 links,
// 0 included because arrays start with 0, 5 included as arrays must be consecutive, but neither of these are used, hence 10 total
    Graphics::TBitmap *DirectionPrefDirGraphicsPtr[10];
///< preferred direction marker arrows
    Graphics::TBitmap *DirectionNonSigRouteGraphicsPtr[10];
///< unrestricted route marker arrows
    Graphics::TBitmap *DirectionSigRouteGraphicsPtr[10];
///< preferred direction route marker arrows
    Graphics::TBitmap *DirectionRouteAutoSigsGraphicsPtr[10];
///< autosigs route marker arrows

    Graphics::TBitmap *PointModeGraphicsPtr[32][2];
///< for point fillets - 32 sets of points, each with two fillets

    Graphics::TBitmap *LockedRouteCancelPtr[10];
///< for locked route cancel graphic, 1 for each of 8 links, 0 & 5 included as for direction
// marker arrows but not used

// This group is for signal speedbutton glyphs, added at v2.3.0 when glyphs required to change hand for RH signals.  These have to
// stay black regardless of background colour.  Before v2.3.0 they were loaded directly from the resource file in InterfaceUnit.cpp,
// where they are already black, but after they can't because sometimes they are RH signals, so these are used instead of those
// that change colour.  They are created from the resource file at startup and they change hand when ConvertSignalsToOppositeHand is called.
    Graphics::TBitmap *SpeedBut68NormBlackGlyph;
    Graphics::TBitmap *SpeedBut69NormBlackGlyph;
    Graphics::TBitmap *SpeedBut70NormBlackGlyph;
    Graphics::TBitmap *SpeedBut71NormBlackGlyph;
    Graphics::TBitmap *SpeedBut72NormBlackGlyph;
    Graphics::TBitmap *SpeedBut73NormBlackGlyph;
    Graphics::TBitmap *SpeedBut74NormBlackGlyph;
    Graphics::TBitmap *SpeedBut75NormBlackGlyph;
    Graphics::TBitmap *SpeedBut68GrndBlackGlyph;
    Graphics::TBitmap *SpeedBut69GrndBlackGlyph;
    Graphics::TBitmap *SpeedBut70GrndBlackGlyph;
    Graphics::TBitmap *SpeedBut71GrndBlackGlyph;
    Graphics::TBitmap *SpeedBut72GrndBlackGlyph;
    Graphics::TBitmap *SpeedBut73GrndBlackGlyph;
    Graphics::TBitmap *SpeedBut74GrndBlackGlyph;
    Graphics::TBitmap *SpeedBut75GrndBlackGlyph;

// functions
    TRailGraphics::TRailGraphics();
///< constructor
    TRailGraphics::~TRailGraphics();
///< destructor
/// Changes all graphics to become compatible with a new background colour
///
/// Uses 'ChangeTransparentColour' method to change each graphic in turn
    void ChangeAllTransparentColours(TColor NewTransparentColour, TColor OldTransparentColour);
/// Change any pixels that are OldBackgroundColour in BitmapIn to NewBackgroundColour in BitmapOut
///
/// Checks that the change is correct and sets ColourError if not (Windows will choose nearest matching colour if
/// colour depth is incorrect and that will cause the error - lot of early problems due to this behaviour but all graphics now
/// 256 colours)
    void ChangeBackgroundColour(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap* BitmapOut, TColor NewBackgroundColour, TColor OldBackgroundColour,
                                bool &ColourError);
/// New function to do the same as the above but with fewer pixel changes - for use in LoadSession (& PlotStartPosition) to avoid resource failures on loading many session trains
    void ChangeBackgroundColour2(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap* BitmapOut, TColor NewBackgroundColour, TColor OldBackgroundColour);
/// as above but uses Scanline
    void ChangeBackgroundColour3(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap* BitmapOut, TColor NewBackgroundColour, TColor OldBackgroundColour);
/// Change any pixels that are not BackgroundColour in BitmapIn to NewForegroundColour in BitmapOut
/// Any that are BackgroundColour in BitmapIn are set to BackgroundColour in BitmapOut
    void ChangeForegroundColour(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap* BitmapOut, TColor NewForegroundColour, TColor BackgroundColour);
/// New function to do the same as the above but with fewer pixel changes - for use in LoadSession to avoid resource failures on loading many session trains
    void ChangeForegroundColour2(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap *BitmapOut, TColor NewForegroundColour, TColor BackgroundColour);
// ---------------------------------------------------------------------------
/// Change any pixels that are ColourToBeChanged in BitmapIn to NewColour in BitmapOut
/// Any that are not ColourToBeChanged in BitmapIn are set to their original colour in BitmapOut
    void ChangeSpecificColour(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap *BitmapOut, TColor ColourToBeChanged, TColor NewColour);
/// Changes a graphic to become compatible with a new background colour
///
/// Change any pixels that are OldTransparentColour in BitmapIn to NewTransparentColour in BitmapOut,
/// if NewTransparentColour dark then change any black pixels in BitmapIn to white in BitmapOut, or
/// if NewTransparentColour white then change any white pixels in BitmapIn to black in BitmapOut
    void ChangeTransparentColour(Graphics::TBitmap *BitmapIn, Graphics::TBitmap *BitmapOut, TColor NewTransparentColour, TColor OldTransparentColour);
/// used in ChangeBackgroundColour3 to convert a named colour to a number for Scanline
    int ColNametoNumber(int Caller, TColor Colour);
/// Converts all signal graphics to opposite hand new at v2.3.0
    void ConvertSignalsToOppositeHand(int Caller);
/// Sets all the derived graphics to be compatible with a new transparent colour
///
/// Used when changing the background from dark to light and vice versa.  The derived graphics are the preferred direction
/// and route graphics for tracks, underbridges, direction markers and all the point fillets.
    void SetUpAllDerivitiveGraphics(TColor TransparentColour);
};

// ---------------------------------------------------------------------------

extern TRailGraphics *RailGraphics;
///< the object pointer, object created in InterfaceUnit

// ---------------------------------------------------------------------------
#endif
