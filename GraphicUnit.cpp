// GraphicUnit.cpp
/*
      BEWARE OF COMMENTS in .cpp files:  they were accurate when written but have
      sometimes been overtaken by changes and not updated
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

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <Graphics.hpp>
#include <ComCtrls.hpp>
#include <fstream>
#include <vector>
#include <vcl.h>

#pragma hdrstop

#include "GraphicUnit.h"
#include "Utilities.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
// ---------------------------------------------------------------------------

TRailGraphics *RailGraphics;

// ---------------------------------------------------------------------------

TRailGraphics::TRailGraphics()
{
// See Graphics.xlsx for details of all graphics

    // transparent graphics
    bm10 = new Graphics::TBitmap;
    bm10->LoadFromResourceName(0, "bm10");
    bm10->Transparent = true;
    bm10->TransparentColor = clB5G5R5;
    bm100 = new Graphics::TBitmap;
    bm100->LoadFromResourceName(0, "bm100");
    bm100->Transparent = true;
    bm100->TransparentColor = clB5G5R5;
    bm101 = new Graphics::TBitmap;
    bm101->LoadFromResourceName(0, "bm101");
    bm101->Transparent = true;
    bm101->TransparentColor = clB5G5R5;
    bm106 = new Graphics::TBitmap;
    bm106->LoadFromResourceName(0, "bm106");
    bm106->Transparent = true;
    bm106->TransparentColor = clB5G5R5;
    bm10Diverging = new Graphics::TBitmap;
    bm10Diverging->LoadFromResourceName(0, "bm10Diverging");
    bm10Diverging->Transparent = true;
    bm10Diverging->TransparentColor = clB5G5R5;
    bm10Straight = new Graphics::TBitmap;
    bm10Straight->LoadFromResourceName(0, "bm10Straight");
    bm10Straight->Transparent = true;
    bm10Straight->TransparentColor = clB5G5R5;
    bm11 = new Graphics::TBitmap;
    bm11->LoadFromResourceName(0, "bm11");
    bm11->Transparent = true;
    bm11->TransparentColor = clB5G5R5;
    bm11Diverging = new Graphics::TBitmap;
    bm11Diverging->LoadFromResourceName(0, "bm11Diverging");
    bm11Diverging->Transparent = true;
    bm11Diverging->TransparentColor = clB5G5R5;
    bm11Straight = new Graphics::TBitmap;
    bm11Straight->LoadFromResourceName(0, "bm11Straight");
    bm11Straight->Transparent = true;
    bm11Straight->TransparentColor = clB5G5R5;
    bm12 = new Graphics::TBitmap;
    bm12->LoadFromResourceName(0, "bm12");
    bm12->Transparent = true;
    bm12->TransparentColor = clB5G5R5;
    bm12Diverging = new Graphics::TBitmap;
    bm12Diverging->LoadFromResourceName(0, "bm12Diverging");
    bm12Diverging->Transparent = true;
    bm12Diverging->TransparentColor = clB5G5R5;
    bm12Straight = new Graphics::TBitmap;
    bm12Straight->LoadFromResourceName(0, "bm12Straight");
    bm12Straight->Transparent = true;
    bm12Straight->TransparentColor = clB5G5R5;
    bm13 = new Graphics::TBitmap;
    bm13->LoadFromResourceName(0, "bm13");
    bm13->Transparent = true;
    bm13->TransparentColor = clB5G5R5;
    bm132 = new Graphics::TBitmap;
    bm132->LoadFromResourceName(0, "bm132");
    bm132->Transparent = true;
    bm132->TransparentColor = clB5G5R5;
    bm132LeftFork = new Graphics::TBitmap;
    bm132LeftFork->LoadFromResourceName(0, "bm132LeftFork");
    bm132LeftFork->Transparent = true;
    bm132LeftFork->TransparentColor = clB5G5R5;
    bm132RightFork = new Graphics::TBitmap;
    bm132RightFork->LoadFromResourceName(0, "bm132RightFork");
    bm132RightFork->Transparent = true;
    bm132RightFork->TransparentColor = clB5G5R5;
    bm133 = new Graphics::TBitmap;
    bm133->LoadFromResourceName(0, "bm133");
    bm133->Transparent = true;
    bm133->TransparentColor = clB5G5R5;
    bm133LeftFork = new Graphics::TBitmap;
    bm133LeftFork->LoadFromResourceName(0, "bm133LeftFork");
    bm133LeftFork->Transparent = true;
    bm133LeftFork->TransparentColor = clB5G5R5;
    bm133RightFork = new Graphics::TBitmap;
    bm133RightFork->LoadFromResourceName(0, "bm133RightFork");
    bm133RightFork->Transparent = true;
    bm133RightFork->TransparentColor = clB5G5R5;
    bm134 = new Graphics::TBitmap;
    bm134->LoadFromResourceName(0, "bm134");
    bm134->Transparent = true;
    bm134->TransparentColor = clB5G5R5;
    bm134LeftFork = new Graphics::TBitmap;
    bm134LeftFork->LoadFromResourceName(0, "bm134LeftFork");
    bm134LeftFork->Transparent = true;
    bm134LeftFork->TransparentColor = clB5G5R5;
    bm134RightFork = new Graphics::TBitmap;
    bm134RightFork->LoadFromResourceName(0, "bm134RightFork");
    bm134RightFork->Transparent = true;
    bm134RightFork->TransparentColor = clB5G5R5;
    bm135 = new Graphics::TBitmap;
    bm135->LoadFromResourceName(0, "bm135");
    bm135->Transparent = true;
    bm135->TransparentColor = clB5G5R5;
    bm135LeftFork = new Graphics::TBitmap;
    bm135LeftFork->LoadFromResourceName(0, "bm135LeftFork");
    bm135LeftFork->Transparent = true;
    bm135LeftFork->TransparentColor = clB5G5R5;
    bm135RightFork = new Graphics::TBitmap;
    bm135RightFork->LoadFromResourceName(0, "bm135RightFork");
    bm135RightFork->Transparent = true;
    bm135RightFork->TransparentColor = clB5G5R5;
    bm136 = new Graphics::TBitmap;
    bm136->LoadFromResourceName(0, "bm136");
    bm136->Transparent = true;
    bm136->TransparentColor = clB5G5R5;
    bm136LeftFork = new Graphics::TBitmap;
    bm136LeftFork->LoadFromResourceName(0, "bm136LeftFork");
    bm136LeftFork->Transparent = true;
    bm136LeftFork->TransparentColor = clB5G5R5;
    bm136RightFork = new Graphics::TBitmap;
    bm136RightFork->LoadFromResourceName(0, "bm136RightFork");
    bm136RightFork->Transparent = true;
    bm136RightFork->TransparentColor = clB5G5R5;
    bm137 = new Graphics::TBitmap;
    bm137->LoadFromResourceName(0, "bm137");
    bm137->Transparent = true;
    bm137->TransparentColor = clB5G5R5;
    bm137LeftFork = new Graphics::TBitmap;
    bm137LeftFork->LoadFromResourceName(0, "bm137LeftFork");
    bm137LeftFork->Transparent = true;
    bm137LeftFork->TransparentColor = clB5G5R5;
    bm137RightFork = new Graphics::TBitmap;
    bm137RightFork->LoadFromResourceName(0, "bm137RightFork");
    bm137RightFork->Transparent = true;
    bm137RightFork->TransparentColor = clB5G5R5;
    bm138 = new Graphics::TBitmap;
    bm138->LoadFromResourceName(0, "bm138");
    bm138->Transparent = true;
    bm138->TransparentColor = clB5G5R5;
    bm138LeftFork = new Graphics::TBitmap;
    bm138LeftFork->LoadFromResourceName(0, "bm138LeftFork");
    bm138LeftFork->Transparent = true;
    bm138LeftFork->TransparentColor = clB5G5R5;
    bm138RightFork = new Graphics::TBitmap;
    bm138RightFork->LoadFromResourceName(0, "bm138RightFork");
    bm138RightFork->Transparent = true;
    bm138RightFork->TransparentColor = clB5G5R5;
    bm139 = new Graphics::TBitmap;
    bm139->LoadFromResourceName(0, "bm139");
    bm139->Transparent = true;
    bm139->TransparentColor = clB5G5R5;
    bm139LeftFork = new Graphics::TBitmap;
    bm139LeftFork->LoadFromResourceName(0, "bm139LeftFork");
    bm139LeftFork->Transparent = true;
    bm139LeftFork->TransparentColor = clB5G5R5;
    bm139RightFork = new Graphics::TBitmap;
    bm139RightFork->LoadFromResourceName(0, "bm139RightFork");
    bm139RightFork->Transparent = true;
    bm139RightFork->TransparentColor = clB5G5R5;
    bm13Diverging = new Graphics::TBitmap;
    bm13Diverging->LoadFromResourceName(0, "bm13Diverging");
    bm13Diverging->Transparent = true;
    bm13Diverging->TransparentColor = clB5G5R5;
    bm13Straight = new Graphics::TBitmap;
    bm13Straight->LoadFromResourceName(0, "bm13Straight");
    bm13Straight->Transparent = true;
    bm13Straight->TransparentColor = clB5G5R5;
    bm14 = new Graphics::TBitmap;
    bm14->LoadFromResourceName(0, "bm14");
    bm14->Transparent = true;
    bm14->TransparentColor = clB5G5R5;
    bm140 = new Graphics::TBitmap;
    bm140->LoadFromResourceName(0, "bm140");
    bm140->Transparent = true;
    bm140->TransparentColor = clB5G5R5;
    bm141 = new Graphics::TBitmap;
    bm141->LoadFromResourceName(0, "bm141");
    bm141->Transparent = true;
    bm141->TransparentColor = clB5G5R5;
    bm14Diverging = new Graphics::TBitmap;
    bm14Diverging->LoadFromResourceName(0, "bm14Diverging");
    bm14Diverging->Transparent = true;
    bm14Diverging->TransparentColor = clB5G5R5;
    bm14Straight = new Graphics::TBitmap;
    bm14Straight->LoadFromResourceName(0, "bm14Straight");
    bm14Straight->Transparent = true;
    bm14Straight->TransparentColor = clB5G5R5;
    bm16 = new Graphics::TBitmap;
    bm16->LoadFromResourceName(0, "bm16");
    bm16->Transparent = true;
    bm16->TransparentColor = clB5G5R5;
    bm18 = new Graphics::TBitmap;
    bm18->LoadFromResourceName(0, "bm18");
    bm18->Transparent = true;
    bm18->TransparentColor = clB5G5R5;
    bm20 = new Graphics::TBitmap;
    bm20->LoadFromResourceName(0, "bm20");
    bm20->Transparent = true;
    bm20->TransparentColor = clB5G5R5;
    bm27 = new Graphics::TBitmap;
    bm27->LoadFromResourceName(0, "bm27");
    bm27->Transparent = true;
    bm27->TransparentColor = clB5G5R5;
    bm28 = new Graphics::TBitmap;
    bm28->LoadFromResourceName(0, "bm28");
    bm28->Transparent = true;
    bm28->TransparentColor = clB5G5R5;
    bm28Diverging = new Graphics::TBitmap;
    bm28Diverging->LoadFromResourceName(0, "bm28Diverging");
    bm28Diverging->Transparent = true;
    bm28Diverging->TransparentColor = clB5G5R5;
    bm28Straight = new Graphics::TBitmap;
    bm28Straight->LoadFromResourceName(0, "bm28Straight");
    bm28Straight->Transparent = true;
    bm28Straight->TransparentColor = clB5G5R5;
    bm29 = new Graphics::TBitmap;
    bm29->LoadFromResourceName(0, "bm29");
    bm29->Transparent = true;
    bm29->TransparentColor = clB5G5R5;
    bm29Diverging = new Graphics::TBitmap;
    bm29Diverging->LoadFromResourceName(0, "bm29Diverging");
    bm29Diverging->Transparent = true;
    bm29Diverging->TransparentColor = clB5G5R5;
    bm29Straight = new Graphics::TBitmap;
    bm29Straight->LoadFromResourceName(0, "bm29Straight");
    bm29Straight->Transparent = true;
    bm29Straight->TransparentColor = clB5G5R5;
    bm30 = new Graphics::TBitmap;
    bm30->LoadFromResourceName(0, "bm30");
    bm30->Transparent = true;
    bm30->TransparentColor = clB5G5R5;
    bm30Diverging = new Graphics::TBitmap;
    bm30Diverging->LoadFromResourceName(0, "bm30Diverging");
    bm30Diverging->Transparent = true;
    bm30Diverging->TransparentColor = clB5G5R5;
    bm30Straight = new Graphics::TBitmap;
    bm30Straight->LoadFromResourceName(0, "bm30Straight");
    bm30Straight->Transparent = true;
    bm30Straight->TransparentColor = clB5G5R5;
    bm31 = new Graphics::TBitmap;
    bm31->LoadFromResourceName(0, "bm31");
    bm31->Transparent = true;
    bm31->TransparentColor = clB5G5R5;
    bm31Diverging = new Graphics::TBitmap;
    bm31Diverging->LoadFromResourceName(0, "bm31Diverging");
    bm31Diverging->Transparent = true;
    bm31Diverging->TransparentColor = clB5G5R5;
    bm31Straight = new Graphics::TBitmap;
    bm31Straight->LoadFromResourceName(0, "bm31Straight");
    bm31Straight->Transparent = true;
    bm31Straight->TransparentColor = clB5G5R5;
    bm32 = new Graphics::TBitmap;
    bm32->LoadFromResourceName(0, "bm32");
    bm32->Transparent = true;
    bm32->TransparentColor = clB5G5R5;
    bm32Diverging = new Graphics::TBitmap;
    bm32Diverging->LoadFromResourceName(0, "bm32Diverging");
    bm32Diverging->Transparent = true;
    bm32Diverging->TransparentColor = clB5G5R5;
    bm32Straight = new Graphics::TBitmap;
    bm32Straight->LoadFromResourceName(0, "bm32Straight");
    bm32Straight->Transparent = true;
    bm32Straight->TransparentColor = clB5G5R5;
    bm33 = new Graphics::TBitmap;
    bm33->LoadFromResourceName(0, "bm33");
    bm33->Transparent = true;
    bm33->TransparentColor = clB5G5R5;
    bm33Diverging = new Graphics::TBitmap;
    bm33Diverging->LoadFromResourceName(0, "bm33Diverging");
    bm33Diverging->Transparent = true;
    bm33Diverging->TransparentColor = clB5G5R5;
    bm33Straight = new Graphics::TBitmap;
    bm33Straight->LoadFromResourceName(0, "bm33Straight");
    bm33Straight->Transparent = true;
    bm33Straight->TransparentColor = clB5G5R5;
    bm34 = new Graphics::TBitmap;
    bm34->LoadFromResourceName(0, "bm34");
    bm34->Transparent = true;
    bm34->TransparentColor = clB5G5R5;
    bm34Diverging = new Graphics::TBitmap;
    bm34Diverging->LoadFromResourceName(0, "bm34Diverging");
    bm34Diverging->Transparent = true;
    bm34Diverging->TransparentColor = clB5G5R5;
    bm34Straight = new Graphics::TBitmap;
    bm34Straight->LoadFromResourceName(0, "bm34Straight");
    bm34Straight->Transparent = true;
    bm34Straight->TransparentColor = clB5G5R5;
    bm35 = new Graphics::TBitmap;
    bm35->LoadFromResourceName(0, "bm35");
    bm35->Transparent = true;
    bm35->TransparentColor = clB5G5R5;
    bm35Diverging = new Graphics::TBitmap;
    bm35Diverging->LoadFromResourceName(0, "bm35Diverging");
    bm35Diverging->Transparent = true;
    bm35Diverging->TransparentColor = clB5G5R5;
    bm35Straight = new Graphics::TBitmap;
    bm35Straight->LoadFromResourceName(0, "bm35Straight");
    bm35Straight->Transparent = true;
    bm35Straight->TransparentColor = clB5G5R5;
    bm36 = new Graphics::TBitmap;
    bm36->LoadFromResourceName(0, "bm36");
    bm36->Transparent = true;
    bm36->TransparentColor = clB5G5R5;
    bm36Diverging = new Graphics::TBitmap;
    bm36Diverging->LoadFromResourceName(0, "bm36Diverging");
    bm36Diverging->Transparent = true;
    bm36Diverging->TransparentColor = clB5G5R5;
    bm36Straight = new Graphics::TBitmap;
    bm36Straight->LoadFromResourceName(0, "bm36Straight");
    bm36Straight->Transparent = true;
    bm36Straight->TransparentColor = clB5G5R5;
    bm37 = new Graphics::TBitmap;
    bm37->LoadFromResourceName(0, "bm37");
    bm37->Transparent = true;
    bm37->TransparentColor = clB5G5R5;
    bm37Diverging = new Graphics::TBitmap;
    bm37Diverging->LoadFromResourceName(0, "bm37Diverging");
    bm37Diverging->Transparent = true;
    bm37Diverging->TransparentColor = clB5G5R5;
    bm37Straight = new Graphics::TBitmap;
    bm37Straight->LoadFromResourceName(0, "bm37Straight");
    bm37Straight->Transparent = true;
    bm37Straight->TransparentColor = clB5G5R5;
    bm38 = new Graphics::TBitmap;
    bm38->LoadFromResourceName(0, "bm38");
    bm38->Transparent = true;
    bm38->TransparentColor = clB5G5R5;
    bm38Diverging = new Graphics::TBitmap;
    bm38Diverging->LoadFromResourceName(0, "bm38Diverging");
    bm38Diverging->Transparent = true;
    bm38Diverging->TransparentColor = clB5G5R5;
    bm38Straight = new Graphics::TBitmap;
    bm38Straight->LoadFromResourceName(0, "bm38Straight");
    bm38Straight->Transparent = true;
    bm38Straight->TransparentColor = clB5G5R5;
    bm39 = new Graphics::TBitmap;
    bm39->LoadFromResourceName(0, "bm39");
    bm39->Transparent = true;
    bm39->TransparentColor = clB5G5R5;
    bm39Diverging = new Graphics::TBitmap;
    bm39Diverging->LoadFromResourceName(0, "bm39Diverging");
    bm39Diverging->Transparent = true;
    bm39Diverging->TransparentColor = clB5G5R5;
    bm39Straight = new Graphics::TBitmap;
    bm39Straight->LoadFromResourceName(0, "bm39Straight");
    bm39Straight->Transparent = true;
    bm39Straight->TransparentColor = clB5G5R5;
    bm40 = new Graphics::TBitmap;
    bm40->LoadFromResourceName(0, "bm40");
    bm40->Transparent = true;
    bm40->TransparentColor = clB5G5R5;
    bm40Diverging = new Graphics::TBitmap;
    bm40Diverging->LoadFromResourceName(0, "bm40Diverging");
    bm40Diverging->Transparent = true;
    bm40Diverging->TransparentColor = clB5G5R5;
    bm40Straight = new Graphics::TBitmap;
    bm40Straight->LoadFromResourceName(0, "bm40Straight");
    bm40Straight->Transparent = true;
    bm40Straight->TransparentColor = clB5G5R5;
    bm41 = new Graphics::TBitmap;
    bm41->LoadFromResourceName(0, "bm41");
    bm41->Transparent = true;
    bm41->TransparentColor = clB5G5R5;
    bm41Diverging = new Graphics::TBitmap;
    bm41Diverging->LoadFromResourceName(0, "bm41Diverging");
    bm41Diverging->Transparent = true;
    bm41Diverging->TransparentColor = clB5G5R5;
    bm41Straight = new Graphics::TBitmap;
    bm41Straight->LoadFromResourceName(0, "bm41Straight");
    bm41Straight->Transparent = true;
    bm41Straight->TransparentColor = clB5G5R5;
    bm42 = new Graphics::TBitmap;
    bm42->LoadFromResourceName(0, "bm42");
    bm42->Transparent = true;
    bm42->TransparentColor = clB5G5R5;
    bm42Diverging = new Graphics::TBitmap;
    bm42Diverging->LoadFromResourceName(0, "bm42Diverging");
    bm42Diverging->Transparent = true;
    bm42Diverging->TransparentColor = clB5G5R5;
    bm42Straight = new Graphics::TBitmap;
    bm42Straight->LoadFromResourceName(0, "bm42Straight");
    bm42Straight->Transparent = true;
    bm42Straight->TransparentColor = clB5G5R5;
    bm43 = new Graphics::TBitmap;
    bm43->LoadFromResourceName(0, "bm43");
    bm43->Transparent = true;
    bm43->TransparentColor = clB5G5R5;
    bm43Diverging = new Graphics::TBitmap;
    bm43Diverging->LoadFromResourceName(0, "bm43Diverging");
    bm43Diverging->Transparent = true;
    bm43Diverging->TransparentColor = clB5G5R5;
    bm43Straight = new Graphics::TBitmap;
    bm43Straight->LoadFromResourceName(0, "bm43Straight");
    bm43Straight->Transparent = true;
    bm43Straight->TransparentColor = clB5G5R5;
    bm45 = new Graphics::TBitmap;
    bm45->LoadFromResourceName(0, "bm45");
    bm45->Transparent = true;
    bm45->TransparentColor = clB5G5R5;
    bm46 = new Graphics::TBitmap;
    bm46->LoadFromResourceName(0, "bm46");
    bm46->Transparent = true;
    bm46->TransparentColor = clB5G5R5;
    bm50 = new Graphics::TBitmap;
    bm50->LoadFromResourceName(0, "bm50");
    bm50->Transparent = true;
    bm50->TransparentColor = clB5G5R5;
    bm51 = new Graphics::TBitmap;
    bm51->LoadFromResourceName(0, "bm51");
    bm51->Transparent = true;
    bm51->TransparentColor = clB5G5R5;
    bm53 = new Graphics::TBitmap;
    bm53->LoadFromResourceName(0, "bm53");
    bm53->Transparent = true;
    bm53->TransparentColor = clB5G5R5;
    bm54 = new Graphics::TBitmap;
    bm54->LoadFromResourceName(0, "bm54");
    bm54->Transparent = true;
    bm54->TransparentColor = clB5G5R5;
    bm56 = new Graphics::TBitmap;
    bm56->LoadFromResourceName(0, "bm56");
    bm56->Transparent = true;
    bm56->TransparentColor = clB5G5R5;
    bm59 = new Graphics::TBitmap;
    bm59->LoadFromResourceName(0, "bm59");
    bm59->Transparent = true;
    bm59->TransparentColor = clB5G5R5;
    bm65 = new Graphics::TBitmap;
    bm65->LoadFromResourceName(0, "bm65");
    bm65->Transparent = true;
    bm65->TransparentColor = clB5G5R5;
    bm68CallingOn = new Graphics::TBitmap;
    bm68CallingOn->LoadFromResourceName(0, "bm68CallingOn");
    bm68CallingOn->Transparent = true;
    bm68CallingOn->TransparentColor = clB5G5R5;
    bm68dblyellow = new Graphics::TBitmap;
    bm68dblyellow->LoadFromResourceName(0, "bm68dblyellow");
    bm68dblyellow->Transparent = true;
    bm68dblyellow->TransparentColor = clB5G5R5;
    bm68grounddblred = new Graphics::TBitmap;
    bm68grounddblred->LoadFromResourceName(0, "bm68grounddblred");
    bm68grounddblred->Transparent = true;
    bm68grounddblred->TransparentColor = clB5G5R5;
    bm68grounddblwhite = new Graphics::TBitmap;
    bm68grounddblwhite->LoadFromResourceName(0, "bm68grounddblwhite");
    bm68grounddblwhite->Transparent = true;
    bm68grounddblwhite->TransparentColor = clB5G5R5;
    bm68green = new Graphics::TBitmap;
    bm68green->LoadFromResourceName(0, "bm68green");
    bm68green->Transparent = true;
    bm68green->TransparentColor = clB5G5R5;
    bm68yellow = new Graphics::TBitmap;
    bm68yellow->LoadFromResourceName(0, "bm68yellow");
    bm68yellow->Transparent = true;
    bm68yellow->TransparentColor = clB5G5R5;
    bm69CallingOn = new Graphics::TBitmap;
    bm69CallingOn->LoadFromResourceName(0, "bm69CallingOn");
    bm69CallingOn->Transparent = true;
    bm69CallingOn->TransparentColor = clB5G5R5;
    bm69dblyellow = new Graphics::TBitmap;
    bm69dblyellow->LoadFromResourceName(0, "bm69dblyellow");
    bm69dblyellow->Transparent = true;
    bm69dblyellow->TransparentColor = clB5G5R5;
    bm69grounddblred = new Graphics::TBitmap;
    bm69grounddblred->LoadFromResourceName(0, "bm69grounddblred");
    bm69grounddblred->Transparent = true;
    bm69grounddblred->TransparentColor = clB5G5R5;
    bm69grounddblwhite = new Graphics::TBitmap;
    bm69grounddblwhite->LoadFromResourceName(0, "bm69grounddblwhite");
    bm69grounddblwhite->Transparent = true;
    bm69grounddblwhite->TransparentColor = clB5G5R5;
    bm69green = new Graphics::TBitmap;
    bm69green->LoadFromResourceName(0, "bm69green");
    bm69green->Transparent = true;
    bm69green->TransparentColor = clB5G5R5;
    bm69yellow = new Graphics::TBitmap;
    bm69yellow->LoadFromResourceName(0, "bm69yellow");
    bm69yellow->Transparent = true;
    bm69yellow->TransparentColor = clB5G5R5;
    bm7 = new Graphics::TBitmap;
    bm7->LoadFromResourceName(0, "bm7");
    bm7->Transparent = true;
    bm7->TransparentColor = clB5G5R5;
    bm70CallingOn = new Graphics::TBitmap;
    bm70CallingOn->LoadFromResourceName(0, "bm70CallingOn");
    bm70CallingOn->Transparent = true;
    bm70CallingOn->TransparentColor = clB5G5R5;
    bm70dblyellow = new Graphics::TBitmap;
    bm70dblyellow->LoadFromResourceName(0, "bm70dblyellow");
    bm70dblyellow->Transparent = true;
    bm70dblyellow->TransparentColor = clB5G5R5;
    bm70grounddblred = new Graphics::TBitmap;
    bm70grounddblred->LoadFromResourceName(0, "bm70grounddblred");
    bm70grounddblred->Transparent = true;
    bm70grounddblred->TransparentColor = clB5G5R5;
    bm70grounddblwhite = new Graphics::TBitmap;
    bm70grounddblwhite->LoadFromResourceName(0, "bm70grounddblwhite");
    bm70grounddblwhite->Transparent = true;
    bm70grounddblwhite->TransparentColor = clB5G5R5;
    bm70green = new Graphics::TBitmap;
    bm70green->LoadFromResourceName(0, "bm70green");
    bm70green->Transparent = true;
    bm70green->TransparentColor = clB5G5R5;
    bm70yellow = new Graphics::TBitmap;
    bm70yellow->LoadFromResourceName(0, "bm70yellow");
    bm70yellow->Transparent = true;
    bm70yellow->TransparentColor = clB5G5R5;
    bm71CallingOn = new Graphics::TBitmap;
    bm71CallingOn->LoadFromResourceName(0, "bm71CallingOn");
    bm71CallingOn->Transparent = true;
    bm71CallingOn->TransparentColor = clB5G5R5;
    bm71dblyellow = new Graphics::TBitmap;
    bm71dblyellow->LoadFromResourceName(0, "bm71dblyellow");
    bm71dblyellow->Transparent = true;
    bm71dblyellow->TransparentColor = clB5G5R5;
    bm71grounddblred = new Graphics::TBitmap;
    bm71grounddblred->LoadFromResourceName(0, "bm71grounddblred");
    bm71grounddblred->Transparent = true;
    bm71grounddblred->TransparentColor = clB5G5R5;
    bm71grounddblwhite = new Graphics::TBitmap;
    bm71grounddblwhite->LoadFromResourceName(0, "bm71grounddblwhite");
    bm71grounddblwhite->Transparent = true;
    bm71grounddblwhite->TransparentColor = clB5G5R5;
    bm71green = new Graphics::TBitmap;
    bm71green->LoadFromResourceName(0, "bm71green");
    bm71green->Transparent = true;
    bm71green->TransparentColor = clB5G5R5;
    bm71yellow = new Graphics::TBitmap;
    bm71yellow->LoadFromResourceName(0, "bm71yellow");
    bm71yellow->Transparent = true;
    bm71yellow->TransparentColor = clB5G5R5;
    bm72CallingOn = new Graphics::TBitmap;
    bm72CallingOn->LoadFromResourceName(0, "bm72CallingOn");
    bm72CallingOn->Transparent = true;
    bm72CallingOn->TransparentColor = clB5G5R5;
    bm72dblyellow = new Graphics::TBitmap;
    bm72dblyellow->LoadFromResourceName(0, "bm72dblyellow");
    bm72dblyellow->Transparent = true;
    bm72dblyellow->TransparentColor = clB5G5R5;
    bm72grounddblred = new Graphics::TBitmap;
    bm72grounddblred->LoadFromResourceName(0, "bm72grounddblred");
    bm72grounddblred->Transparent = true;
    bm72grounddblred->TransparentColor = clB5G5R5;
    bm72grounddblwhite = new Graphics::TBitmap;
    bm72grounddblwhite->LoadFromResourceName(0, "bm72grounddblwhite");
    bm72grounddblwhite->Transparent = true;
    bm72grounddblwhite->TransparentColor = clB5G5R5;
    bm72green = new Graphics::TBitmap;
    bm72green->LoadFromResourceName(0, "bm72green");
    bm72green->Transparent = true;
    bm72green->TransparentColor = clB5G5R5;
    bm72yellow = new Graphics::TBitmap;
    bm72yellow->LoadFromResourceName(0, "bm72yellow");
    bm72yellow->Transparent = true;
    bm72yellow->TransparentColor = clB5G5R5;
    bm73 = new Graphics::TBitmap;
    bm73->LoadFromResourceName(0, "bm73");
    bm73->Transparent = true;
    bm73->TransparentColor = clB5G5R5;
    bm73CallingOn = new Graphics::TBitmap;
    bm73CallingOn->LoadFromResourceName(0, "bm73CallingOn");
    bm73CallingOn->Transparent = true;
    bm73CallingOn->TransparentColor = clB5G5R5;
    bm73dblyellow = new Graphics::TBitmap;
    bm73dblyellow->LoadFromResourceName(0, "bm73dblyellow");
    bm73dblyellow->Transparent = true;
    bm73dblyellow->TransparentColor = clB5G5R5;
    bm73grounddblred = new Graphics::TBitmap;
    bm73grounddblred->LoadFromResourceName(0, "bm73grounddblred");
    bm73grounddblred->Transparent = true;
    bm73grounddblred->TransparentColor = clB5G5R5;
    bm73grounddblwhite = new Graphics::TBitmap;
    bm73grounddblwhite->LoadFromResourceName(0, "bm73grounddblwhite");
    bm73grounddblwhite->Transparent = true;
    bm73grounddblwhite->TransparentColor = clB5G5R5;
    bm73green = new Graphics::TBitmap;
    bm73green->LoadFromResourceName(0, "bm73green");
    bm73green->Transparent = true;
    bm73green->TransparentColor = clB5G5R5;
    bm73yellow = new Graphics::TBitmap;
    bm73yellow->LoadFromResourceName(0, "bm73yellow");
    bm73yellow->Transparent = true;
    bm73yellow->TransparentColor = clB5G5R5;
    bm74 = new Graphics::TBitmap;
    bm74->LoadFromResourceName(0, "bm74");
    bm74->Transparent = true;
    bm74->TransparentColor = clB5G5R5;
    bm74CallingOn = new Graphics::TBitmap;
    bm74CallingOn->LoadFromResourceName(0, "bm74CallingOn");
    bm74CallingOn->Transparent = true;
    bm74CallingOn->TransparentColor = clB5G5R5;
    bm74dblyellow = new Graphics::TBitmap;
    bm74dblyellow->LoadFromResourceName(0, "bm74dblyellow");
    bm74dblyellow->Transparent = true;
    bm74dblyellow->TransparentColor = clB5G5R5;
    bm74grounddblred = new Graphics::TBitmap;
    bm74grounddblred->LoadFromResourceName(0, "bm74grounddblred");
    bm74grounddblred->Transparent = true;
    bm74grounddblred->TransparentColor = clB5G5R5;
    bm74grounddblwhite = new Graphics::TBitmap;
    bm74grounddblwhite->LoadFromResourceName(0, "bm74grounddblwhite");
    bm74grounddblwhite->Transparent = true;
    bm74grounddblwhite->TransparentColor = clB5G5R5;
    bm74green = new Graphics::TBitmap;
    bm74green->LoadFromResourceName(0, "bm74green");
    bm74green->Transparent = true;
    bm74green->TransparentColor = clB5G5R5;
    bm74yellow = new Graphics::TBitmap;
    bm74yellow->LoadFromResourceName(0, "bm74yellow");
    bm74yellow->Transparent = true;
    bm74yellow->TransparentColor = clB5G5R5;
    bm75CallingOn = new Graphics::TBitmap;
    bm75CallingOn->LoadFromResourceName(0, "bm75CallingOn");
    bm75CallingOn->Transparent = true;
    bm75CallingOn->TransparentColor = clB5G5R5;
    bm75dblyellow = new Graphics::TBitmap;
    bm75dblyellow->LoadFromResourceName(0, "bm75dblyellow");
    bm75dblyellow->Transparent = true;
    bm75dblyellow->TransparentColor = clB5G5R5;
    bm75grounddblred = new Graphics::TBitmap;
    bm75grounddblred->LoadFromResourceName(0, "bm75grounddblred");
    bm75grounddblred->Transparent = true;
    bm75grounddblred->TransparentColor = clB5G5R5;
    bm75grounddblwhite = new Graphics::TBitmap;
    bm75grounddblwhite->LoadFromResourceName(0, "bm75grounddblwhite");
    bm75grounddblwhite->Transparent = true;
    bm75grounddblwhite->TransparentColor = clB5G5R5;
    bm75green = new Graphics::TBitmap;
    bm75green->LoadFromResourceName(0, "bm75green");
    bm75green->Transparent = true;
    bm75green->TransparentColor = clB5G5R5;
    bm75yellow = new Graphics::TBitmap;
    bm75yellow->LoadFromResourceName(0, "bm75yellow");
    bm75yellow->Transparent = true;
    bm75yellow->TransparentColor = clB5G5R5;
    bm77 = new Graphics::TBitmap;
    bm77->LoadFromResourceName(0, "bm77");
    bm77->Transparent = true;
    bm77->TransparentColor = clB5G5R5;
    bm77Striped = new Graphics::TBitmap;
    bm77Striped->LoadFromResourceName(0, "bm77Striped");
    bm77Striped->Transparent = true;
    bm77Striped->TransparentColor = clB5G5R5;
    bm78 = new Graphics::TBitmap;
    bm78->LoadFromResourceName(0, "bm78");
    bm78->Transparent = true;
    bm78->TransparentColor = clB5G5R5;
    bm78Striped = new Graphics::TBitmap;
    bm78Striped->LoadFromResourceName(0, "bm78Striped");
    bm78Striped->Transparent = true;
    bm78Striped->TransparentColor = clB5G5R5;
    bm7Diverging = new Graphics::TBitmap;
    bm7Diverging->LoadFromResourceName(0, "bm7Diverging");
    bm7Diverging->Transparent = true;
    bm7Diverging->TransparentColor = clB5G5R5;
    bm7Straight = new Graphics::TBitmap;
    bm7Straight->LoadFromResourceName(0, "bm7Straight");
    bm7Straight->Transparent = true;
    bm7Straight->TransparentColor = clB5G5R5;
    bm8 = new Graphics::TBitmap;
    bm8->LoadFromResourceName(0, "bm8");
    bm8->Transparent = true;
    bm8->TransparentColor = clB5G5R5;
    bm85 = new Graphics::TBitmap;
    bm85->LoadFromResourceName(0, "bm85");
    bm85->Transparent = true;
    bm85->TransparentColor = clB5G5R5;
    bm8Diverging = new Graphics::TBitmap;
    bm8Diverging->LoadFromResourceName(0, "bm8Diverging");
    bm8Diverging->Transparent = true;
    bm8Diverging->TransparentColor = clB5G5R5;
    bm8Straight = new Graphics::TBitmap;
    bm8Straight->LoadFromResourceName(0, "bm8Straight");
    bm8Straight->Transparent = true;
    bm8Straight->TransparentColor = clB5G5R5;
    bm9 = new Graphics::TBitmap;
    bm9->LoadFromResourceName(0, "bm9");
    bm9->Transparent = true;
    bm9->TransparentColor = clB5G5R5;
    bm93set = new Graphics::TBitmap;
    bm93set->LoadFromResourceName(0, "bm93set");
    bm93set->Transparent = true;
    bm93set->TransparentColor = clB5G5R5;
    bm93unset = new Graphics::TBitmap;
    bm93unset->LoadFromResourceName(0, "bm93unset");
    bm93unset->Transparent = true;
    bm93unset->TransparentColor = clB5G5R5;
    bm94set = new Graphics::TBitmap;
    bm94set->LoadFromResourceName(0, "bm94set");
    bm94set->Transparent = true;
    bm94set->TransparentColor = clB5G5R5;
    bm94unset = new Graphics::TBitmap;
    bm94unset->LoadFromResourceName(0, "bm94unset");
    bm94unset->Transparent = true;
    bm94unset->TransparentColor = clB5G5R5;
    bm9Diverging = new Graphics::TBitmap;
    bm9Diverging->LoadFromResourceName(0, "bm9Diverging");
    bm9Diverging->Transparent = true;
    bm9Diverging->TransparentColor = clB5G5R5;
    bm9Straight = new Graphics::TBitmap;
    bm9Straight->LoadFromResourceName(0, "bm9Straight");
    bm9Straight->Transparent = true;
    bm9Straight->TransparentColor = clB5G5R5;
    bmGreenEllipse = new Graphics::TBitmap;
    bmGreenEllipse->LoadFromResourceName(0, "bmGreenEllipse");
    bmGreenEllipse->Transparent = true;
    bmGreenEllipse->TransparentColor = clB5G5R5;
    bmGreenRect = new Graphics::TBitmap;
    bmGreenRect->LoadFromResourceName(0, "bmGreenRect");
    bmGreenRect->Transparent = true;
    bmGreenRect->TransparentColor = clB5G5R5;
    bmGrid = new Graphics::TBitmap;
    bmGrid->LoadFromResourceName(0, "bmGrid");
    bmGrid->Transparent = true;
    bmGrid->TransparentColor = clB5G5R5;
    bmLightBlueRect = new Graphics::TBitmap;
    bmLightBlueRect->LoadFromResourceName(0, "bmLightBlueRect");
    bmLightBlueRect->Transparent = true;
    bmLightBlueRect->TransparentColor = clB5G5R5;
    bmName = new Graphics::TBitmap;
    bmName->LoadFromResourceName(0, "bmName");
    bmName->Transparent = true;
    bmName->TransparentColor = clB5G5R5;
    bmNameStriped = new Graphics::TBitmap;
    bmNameStriped->LoadFromResourceName(0, "bmNameStriped");
    bmNameStriped->Transparent = true;
    bmNameStriped->TransparentColor = clB5G5R5;
    bmRedEllipse = new Graphics::TBitmap;
    bmRedEllipse->LoadFromResourceName(0, "bmRedEllipse");
    bmRedEllipse->Transparent = true;
    bmRedEllipse->TransparentColor = clB5G5R5;
    bmRedRect = new Graphics::TBitmap;
    bmRedRect->LoadFromResourceName(0, "bmRedRect");
    bmRedRect->Transparent = true;
    bmRedRect->TransparentColor = clB5G5R5;
    bmRtCancELink1 = new Graphics::TBitmap;
    bmRtCancELink1->LoadFromResourceName(0, "bmRtCancELink1");
    bmRtCancELink1->Transparent = true;
    bmRtCancELink1->TransparentColor = clB5G5R5;
    bmRtCancELink2 = new Graphics::TBitmap;
    bmRtCancELink2->LoadFromResourceName(0, "bmRtCancELink2");
    bmRtCancELink2->Transparent = true;
    bmRtCancELink2->TransparentColor = clB5G5R5;
    bmRtCancELink3 = new Graphics::TBitmap;
    bmRtCancELink3->LoadFromResourceName(0, "bmRtCancELink3");
    bmRtCancELink3->Transparent = true;
    bmRtCancELink3->TransparentColor = clB5G5R5;
    bmRtCancELink4 = new Graphics::TBitmap;
    bmRtCancELink4->LoadFromResourceName(0, "bmRtCancELink4");
    bmRtCancELink4->Transparent = true;
    bmRtCancELink4->TransparentColor = clB5G5R5;
    bmRtCancELink6 = new Graphics::TBitmap;
    bmRtCancELink6->LoadFromResourceName(0, "bmRtCancELink6");
    bmRtCancELink6->Transparent = true;
    bmRtCancELink6->TransparentColor = clB5G5R5;
    bmRtCancELink7 = new Graphics::TBitmap;
    bmRtCancELink7->LoadFromResourceName(0, "bmRtCancELink7");
    bmRtCancELink7->Transparent = true;
    bmRtCancELink7->TransparentColor = clB5G5R5;
    bmRtCancELink8 = new Graphics::TBitmap;
    bmRtCancELink8->LoadFromResourceName(0, "bmRtCancELink8");
    bmRtCancELink8->Transparent = true;
    bmRtCancELink8->TransparentColor = clB5G5R5;
    bmRtCancELink9 = new Graphics::TBitmap;
    bmRtCancELink9->LoadFromResourceName(0, "bmRtCancELink9");
    bmRtCancELink9->Transparent = true;
    bmRtCancELink9->TransparentColor = clB5G5R5;
    br1 = new Graphics::TBitmap;
    br1->LoadFromResourceName(0, "br1");
    br1->Transparent = true;
    br1->TransparentColor = clB5G5R5;
    br10 = new Graphics::TBitmap;
    br10->LoadFromResourceName(0, "br10");
    br10->Transparent = true;
    br10->TransparentColor = clB5G5R5;
    br11 = new Graphics::TBitmap;
    br11->LoadFromResourceName(0, "br11");
    br11->Transparent = true;
    br11->TransparentColor = clB5G5R5;
    br12 = new Graphics::TBitmap;
    br12->LoadFromResourceName(0, "br12");
    br12->Transparent = true;
    br12->TransparentColor = clB5G5R5;
    br2 = new Graphics::TBitmap;
    br2->LoadFromResourceName(0, "br2");
    br2->Transparent = true;
    br2->TransparentColor = clB5G5R5;
    br3 = new Graphics::TBitmap;
    br3->LoadFromResourceName(0, "br3");
    br3->Transparent = true;
    br3->TransparentColor = clB5G5R5;
    br4 = new Graphics::TBitmap;
    br4->LoadFromResourceName(0, "br4");
    br4->Transparent = true;
    br4->TransparentColor = clB5G5R5;
    br5 = new Graphics::TBitmap;
    br5->LoadFromResourceName(0, "br5");
    br5->Transparent = true;
    br5->TransparentColor = clB5G5R5;
    br6 = new Graphics::TBitmap;
    br6->LoadFromResourceName(0, "br6");
    br6->Transparent = true;
    br6->TransparentColor = clB5G5R5;
    br7 = new Graphics::TBitmap;
    br7->LoadFromResourceName(0, "br7");
    br7->Transparent = true;
    br7->TransparentColor = clB5G5R5;
    br8 = new Graphics::TBitmap;
    br8->LoadFromResourceName(0, "br8");
    br8->Transparent = true;
    br8->TransparentColor = clB5G5R5;
    br9 = new Graphics::TBitmap;
    br9->LoadFromResourceName(0, "br9");
    br9->Transparent = true;
    br9->TransparentColor = clB5G5R5;
    Concourse = new Graphics::TBitmap;
    Concourse->LoadFromResourceName(0, "Concourse");
    Concourse->Transparent = true;
    Concourse->TransparentColor = clB5G5R5;
    ConcourseGlyph = new Graphics::TBitmap;
    ConcourseGlyph->LoadFromResourceName(0, "ConcourseGlyph");
    ConcourseGlyph->Transparent = true;
    ConcourseGlyph->TransparentColor = clB5G5R5;
    ConcourseStriped = new Graphics::TBitmap;
    ConcourseStriped->LoadFromResourceName(0, "ConcourseStriped");
    ConcourseStriped->Transparent = true;
    ConcourseStriped->TransparentColor = clB5G5R5;
    ELk1 = new Graphics::TBitmap;
    ELk1->LoadFromResourceName(0, "ELk1");
    ELk1->Transparent = true;
    ELk1->TransparentColor = clB5G5R5;
    ELk2 = new Graphics::TBitmap;
    ELk2->LoadFromResourceName(0, "ELk2");
    ELk2->Transparent = true;
    ELk2->TransparentColor = clB5G5R5;
    ELk3 = new Graphics::TBitmap;
    ELk3->LoadFromResourceName(0, "ELk3");
    ELk3->Transparent = true;
    ELk3->TransparentColor = clB5G5R5;
    ELk4 = new Graphics::TBitmap;
    ELk4->LoadFromResourceName(0, "ELk4");
    ELk4->Transparent = true;
    ELk4->TransparentColor = clB5G5R5;
    ELk6 = new Graphics::TBitmap;
    ELk6->LoadFromResourceName(0, "ELk6");
    ELk6->Transparent = true;
    ELk6->TransparentColor = clB5G5R5;
    ELk7 = new Graphics::TBitmap;
    ELk7->LoadFromResourceName(0, "ELk7");
    ELk7->Transparent = true;
    ELk7->TransparentColor = clB5G5R5;
    ELk8 = new Graphics::TBitmap;
    ELk8->LoadFromResourceName(0, "ELk8");
    ELk8->Transparent = true;
    ELk8->TransparentColor = clB5G5R5;
    ELk9 = new Graphics::TBitmap;
    ELk9->LoadFromResourceName(0, "ELk9");
    ELk9->Transparent = true;
    ELk9->TransparentColor = clB5G5R5;
    gl1 = new Graphics::TBitmap;
    gl1->LoadFromResourceName(0, "gl1");
    gl1->Transparent = true;
    gl1->TransparentColor = clB5G5R5;
    gl10 = new Graphics::TBitmap;
    gl10->LoadFromResourceName(0, "gl10");
    gl10->Transparent = true;
    gl10->TransparentColor = clB5G5R5;
    gl100 = new Graphics::TBitmap;
    gl100->LoadFromResourceName(0, "gl100");
    gl100->Transparent = true;
    gl100->TransparentColor = clB5G5R5;
    gl101 = new Graphics::TBitmap;
    gl101->LoadFromResourceName(0, "gl101");
    gl101->Transparent = true;
    gl101->TransparentColor = clB5G5R5;
    gl102 = new Graphics::TBitmap;
    gl102->LoadFromResourceName(0, "gl102");
    gl102->Transparent = true;
    gl102->TransparentColor = clB5G5R5;
    gl103 = new Graphics::TBitmap;
    gl103->LoadFromResourceName(0, "gl103");
    gl103->Transparent = true;
    gl103->TransparentColor = clB5G5R5;
    gl104 = new Graphics::TBitmap;
    gl104->LoadFromResourceName(0, "gl104");
    gl104->Transparent = true;
    gl104->TransparentColor = clB5G5R5;
    gl105 = new Graphics::TBitmap;
    gl105->LoadFromResourceName(0, "gl105");
    gl105->Transparent = true;
    gl105->TransparentColor = clB5G5R5;
    gl106 = new Graphics::TBitmap;
    gl106->LoadFromResourceName(0, "gl106");
    gl106->Transparent = true;
    gl106->TransparentColor = clB5G5R5;
    gl107 = new Graphics::TBitmap;
    gl107->LoadFromResourceName(0, "gl107");
    gl107->Transparent = true;
    gl107->TransparentColor = clB5G5R5;
    gl108 = new Graphics::TBitmap;
    gl108->LoadFromResourceName(0, "gl108");
    gl108->Transparent = true;
    gl108->TransparentColor = clB5G5R5;
    gl109 = new Graphics::TBitmap;
    gl109->LoadFromResourceName(0, "gl109");
    gl109->Transparent = true;
    gl109->TransparentColor = clB5G5R5;
    gl11 = new Graphics::TBitmap;
    gl11->LoadFromResourceName(0, "gl11");
    gl11->Transparent = true;
    gl11->TransparentColor = clB5G5R5;
    gl110 = new Graphics::TBitmap;
    gl110->LoadFromResourceName(0, "gl110");
    gl110->Transparent = true;
    gl110->TransparentColor = clB5G5R5;
    gl111 = new Graphics::TBitmap;
    gl111->LoadFromResourceName(0, "gl111");
    gl111->Transparent = true;
    gl111->TransparentColor = clB5G5R5;
    gl112 = new Graphics::TBitmap;
    gl112->LoadFromResourceName(0, "gl112");
    gl112->Transparent = true;
    gl112->TransparentColor = clB5G5R5;
    gl113 = new Graphics::TBitmap;
    gl113->LoadFromResourceName(0, "gl113");
    gl113->Transparent = true;
    gl113->TransparentColor = clB5G5R5;
    gl114 = new Graphics::TBitmap;
    gl114->LoadFromResourceName(0, "gl114");
    gl114->Transparent = true;
    gl114->TransparentColor = clB5G5R5;
    gl115 = new Graphics::TBitmap;
    gl115->LoadFromResourceName(0, "gl115");
    gl115->Transparent = true;
    gl115->TransparentColor = clB5G5R5;
    gl116 = new Graphics::TBitmap;
    gl116->LoadFromResourceName(0, "gl116");
    gl116->Transparent = true;
    gl116->TransparentColor = clB5G5R5;
    gl117 = new Graphics::TBitmap;
    gl117->LoadFromResourceName(0, "gl117");
    gl117->Transparent = true;
    gl117->TransparentColor = clB5G5R5;
    gl118 = new Graphics::TBitmap;
    gl118->LoadFromResourceName(0, "gl118");
    gl118->Transparent = true;
    gl118->TransparentColor = clB5G5R5;
    gl119 = new Graphics::TBitmap;
    gl119->LoadFromResourceName(0, "gl119");
    gl119->Transparent = true;
    gl119->TransparentColor = clB5G5R5;
    gl12 = new Graphics::TBitmap;
    gl12->LoadFromResourceName(0, "gl12");
    gl12->Transparent = true;
    gl12->TransparentColor = clB5G5R5;
    gl120 = new Graphics::TBitmap;
    gl120->LoadFromResourceName(0, "gl120");
    gl120->Transparent = true;
    gl120->TransparentColor = clB5G5R5;
    gl121 = new Graphics::TBitmap;
    gl121->LoadFromResourceName(0, "gl121");
    gl121->Transparent = true;
    gl121->TransparentColor = clB5G5R5;
    gl122 = new Graphics::TBitmap;
    gl122->LoadFromResourceName(0, "gl122");
    gl122->Transparent = true;
    gl122->TransparentColor = clB5G5R5;
    gl123 = new Graphics::TBitmap;
    gl123->LoadFromResourceName(0, "gl123");
    gl123->Transparent = true;
    gl123->TransparentColor = clB5G5R5;
    gl124 = new Graphics::TBitmap;
    gl124->LoadFromResourceName(0, "gl124");
    gl124->Transparent = true;
    gl124->TransparentColor = clB5G5R5;
    gl125 = new Graphics::TBitmap;
    gl125->LoadFromResourceName(0, "gl125");
    gl125->Transparent = true;
    gl125->TransparentColor = clB5G5R5;
    gl126 = new Graphics::TBitmap;
    gl126->LoadFromResourceName(0, "gl126");
    gl126->Transparent = true;
    gl126->TransparentColor = clB5G5R5;
    gl127 = new Graphics::TBitmap;
    gl127->LoadFromResourceName(0, "gl127");
    gl127->Transparent = true;
    gl127->TransparentColor = clB5G5R5;
    gl128 = new Graphics::TBitmap;
    gl128->LoadFromResourceName(0, "gl128");
    gl128->Transparent = true;
    gl128->TransparentColor = clB5G5R5;
    gl129 = new Graphics::TBitmap;
    gl129->LoadFromResourceName(0, "gl129");
    gl129->Transparent = true;
    gl129->TransparentColor = clB5G5R5;
    gl129Striped = new Graphics::TBitmap;
    gl129Striped->LoadFromResourceName(0, "gl129Striped");
    gl129Striped->Transparent = true;
    gl129Striped->TransparentColor = clB5G5R5;
    gl13 = new Graphics::TBitmap;
    gl13->LoadFromResourceName(0, "gl13");
    gl13->Transparent = true;
    gl13->TransparentColor = clB5G5R5;
    gl130 = new Graphics::TBitmap;
    gl130->LoadFromResourceName(0, "gl130");
    gl130->Transparent = true;
    gl130->TransparentColor = clB5G5R5;
    gl130Striped = new Graphics::TBitmap;
    gl130Striped->LoadFromResourceName(0, "gl130Striped");
    gl130Striped->Transparent = true;
    gl130Striped->TransparentColor = clB5G5R5;
    gl131 = new Graphics::TBitmap;
    gl131->LoadFromResourceName(0, "gl131");
    gl131->Transparent = true;
    gl131->TransparentColor = clB5G5R5;
    gl132 = new Graphics::TBitmap;
    gl132->LoadFromResourceName(0, "gl132");
    gl132->Transparent = true;
    gl132->TransparentColor = clB5G5R5;
    gl133 = new Graphics::TBitmap;
    gl133->LoadFromResourceName(0, "gl133");
    gl133->Transparent = true;
    gl133->TransparentColor = clB5G5R5;
    gl134 = new Graphics::TBitmap;
    gl134->LoadFromResourceName(0, "gl134");
    gl134->Transparent = true;
    gl134->TransparentColor = clB5G5R5;
    gl135 = new Graphics::TBitmap;
    gl135->LoadFromResourceName(0, "gl135");
    gl135->Transparent = true;
    gl135->TransparentColor = clB5G5R5;
    gl136 = new Graphics::TBitmap;
    gl136->LoadFromResourceName(0, "gl136");
    gl136->Transparent = true;
    gl136->TransparentColor = clB5G5R5;
    gl137 = new Graphics::TBitmap;
    gl137->LoadFromResourceName(0, "gl137");
    gl137->Transparent = true;
    gl137->TransparentColor = clB5G5R5;
    gl138 = new Graphics::TBitmap;
    gl138->LoadFromResourceName(0, "gl138");
    gl138->Transparent = true;
    gl138->TransparentColor = clB5G5R5;
    gl139 = new Graphics::TBitmap;
    gl139->LoadFromResourceName(0, "gl139");
    gl139->Transparent = true;
    gl139->TransparentColor = clB5G5R5;
    gl14 = new Graphics::TBitmap;
    gl14->LoadFromResourceName(0, "gl14");
    gl14->Transparent = true;
    gl14->TransparentColor = clB5G5R5;
    gl140 = new Graphics::TBitmap;
    gl140->LoadFromResourceName(0, "gl140");
    gl140->Transparent = true;
    gl140->TransparentColor = clB5G5R5;
    gl141 = new Graphics::TBitmap;
    gl141->LoadFromResourceName(0, "gl141");
    gl141->Transparent = true;
    gl141->TransparentColor = clB5G5R5;
    gl142 = new Graphics::TBitmap;
    gl142->LoadFromResourceName(0, "gl142");
    gl142->Transparent = true;
    gl142->TransparentColor = clB5G5R5;
    gl143 = new Graphics::TBitmap;
    gl143->LoadFromResourceName(0, "gl143");
    gl143->Transparent = true;
    gl143->TransparentColor = clB5G5R5;
    gl145 = new Graphics::TBitmap;
    gl145->LoadFromResourceName(0, "gl145");
    gl145->Transparent = true;
    gl145->TransparentColor = clB5G5R5;
    gl145Striped = new Graphics::TBitmap;
    gl145Striped->LoadFromResourceName(0, "gl145Striped");
    gl145Striped->Transparent = true;
    gl145Striped->TransparentColor = clB5G5R5;
    gl146 = new Graphics::TBitmap;
    gl146->LoadFromResourceName(0, "gl146");
    gl146->Transparent = true;
    gl146->TransparentColor = clB5G5R5;
    gl146Striped = new Graphics::TBitmap;
    gl146Striped->LoadFromResourceName(0, "gl146Striped");
    gl146Striped->Transparent = true;
    gl146Striped->TransparentColor = clB5G5R5;
    gl15 = new Graphics::TBitmap;
    gl15->LoadFromResourceName(0, "gl15");
    gl15->Transparent = true;
    gl15->TransparentColor = clB5G5R5;
    gl16 = new Graphics::TBitmap;
    gl16->LoadFromResourceName(0, "gl16");
    gl16->Transparent = true;
    gl16->TransparentColor = clB5G5R5;
    gl18 = new Graphics::TBitmap;
    gl18->LoadFromResourceName(0, "gl18");
    gl18->Transparent = true;
    gl18->TransparentColor = clB5G5R5;
    gl19 = new Graphics::TBitmap;
    gl19->LoadFromResourceName(0, "gl19");
    gl19->Transparent = true;
    gl19->TransparentColor = clB5G5R5;
    gl2 = new Graphics::TBitmap;
    gl2->LoadFromResourceName(0, "gl2");
    gl2->Transparent = true;
    gl2->TransparentColor = clB5G5R5;
    gl20 = new Graphics::TBitmap;
    gl20->LoadFromResourceName(0, "gl20");
    gl20->Transparent = true;
    gl20->TransparentColor = clB5G5R5;
    gl21 = new Graphics::TBitmap;
    gl21->LoadFromResourceName(0, "gl21");
    gl21->Transparent = true;
    gl21->TransparentColor = clB5G5R5;
    gl22 = new Graphics::TBitmap;
    gl22->LoadFromResourceName(0, "gl22");
    gl22->Transparent = true;
    gl22->TransparentColor = clB5G5R5;
    gl23 = new Graphics::TBitmap;
    gl23->LoadFromResourceName(0, "gl23");
    gl23->Transparent = true;
    gl23->TransparentColor = clB5G5R5;
    gl24 = new Graphics::TBitmap;
    gl24->LoadFromResourceName(0, "gl24");
    gl24->Transparent = true;
    gl24->TransparentColor = clB5G5R5;
    gl25 = new Graphics::TBitmap;
    gl25->LoadFromResourceName(0, "gl25");
    gl25->Transparent = true;
    gl25->TransparentColor = clB5G5R5;
    gl26 = new Graphics::TBitmap;
    gl26->LoadFromResourceName(0, "gl26");
    gl26->Transparent = true;
    gl26->TransparentColor = clB5G5R5;
    gl27 = new Graphics::TBitmap;
    gl27->LoadFromResourceName(0, "gl27");
    gl27->Transparent = true;
    gl27->TransparentColor = clB5G5R5;
    gl28 = new Graphics::TBitmap;
    gl28->LoadFromResourceName(0, "gl28");
    gl28->Transparent = true;
    gl28->TransparentColor = clB5G5R5;
    gl29 = new Graphics::TBitmap;
    gl29->LoadFromResourceName(0, "gl29");
    gl29->Transparent = true;
    gl29->TransparentColor = clB5G5R5;
    gl3 = new Graphics::TBitmap;
    gl3->LoadFromResourceName(0, "gl3");
    gl3->Transparent = true;
    gl3->TransparentColor = clB5G5R5;
    gl30 = new Graphics::TBitmap;
    gl30->LoadFromResourceName(0, "gl30");
    gl30->Transparent = true;
    gl30->TransparentColor = clB5G5R5;
    gl31 = new Graphics::TBitmap;
    gl31->LoadFromResourceName(0, "gl31");
    gl31->Transparent = true;
    gl31->TransparentColor = clB5G5R5;
    gl32 = new Graphics::TBitmap;
    gl32->LoadFromResourceName(0, "gl32");
    gl32->Transparent = true;
    gl32->TransparentColor = clB5G5R5;
    gl33 = new Graphics::TBitmap;
    gl33->LoadFromResourceName(0, "gl33");
    gl33->Transparent = true;
    gl33->TransparentColor = clB5G5R5;
    gl34 = new Graphics::TBitmap;
    gl34->LoadFromResourceName(0, "gl34");
    gl34->Transparent = true;
    gl34->TransparentColor = clB5G5R5;
    gl35 = new Graphics::TBitmap;
    gl35->LoadFromResourceName(0, "gl35");
    gl35->Transparent = true;
    gl35->TransparentColor = clB5G5R5;
    gl36 = new Graphics::TBitmap;
    gl36->LoadFromResourceName(0, "gl36");
    gl36->Transparent = true;
    gl36->TransparentColor = clB5G5R5;
    gl37 = new Graphics::TBitmap;
    gl37->LoadFromResourceName(0, "gl37");
    gl37->Transparent = true;
    gl37->TransparentColor = clB5G5R5;
    gl38 = new Graphics::TBitmap;
    gl38->LoadFromResourceName(0, "gl38");
    gl38->Transparent = true;
    gl38->TransparentColor = clB5G5R5;
    gl39 = new Graphics::TBitmap;
    gl39->LoadFromResourceName(0, "gl39");
    gl39->Transparent = true;
    gl39->TransparentColor = clB5G5R5;
    gl4 = new Graphics::TBitmap;
    gl4->LoadFromResourceName(0, "gl4");
    gl4->Transparent = true;
    gl4->TransparentColor = clB5G5R5;
    gl40 = new Graphics::TBitmap;
    gl40->LoadFromResourceName(0, "gl40");
    gl40->Transparent = true;
    gl40->TransparentColor = clB5G5R5;
    gl41 = new Graphics::TBitmap;
    gl41->LoadFromResourceName(0, "gl41");
    gl41->Transparent = true;
    gl41->TransparentColor = clB5G5R5;
    gl42 = new Graphics::TBitmap;
    gl42->LoadFromResourceName(0, "gl42");
    gl42->Transparent = true;
    gl42->TransparentColor = clB5G5R5;
    gl43 = new Graphics::TBitmap;
    gl43->LoadFromResourceName(0, "gl43");
    gl43->Transparent = true;
    gl43->TransparentColor = clB5G5R5;
    gl44 = new Graphics::TBitmap;
    gl44->LoadFromResourceName(0, "gl44");
    gl44->Transparent = true;
    gl44->TransparentColor = clB5G5R5;
    gl45 = new Graphics::TBitmap;
    gl45->LoadFromResourceName(0, "gl45");
    gl45->Transparent = true;
    gl45->TransparentColor = clB5G5R5;
    gl46 = new Graphics::TBitmap;
    gl46->LoadFromResourceName(0, "gl46");
    gl46->Transparent = true;
    gl46->TransparentColor = clB5G5R5;
    gl47 = new Graphics::TBitmap;
    gl47->LoadFromResourceName(0, "gl47");
    gl47->Transparent = true;
    gl47->TransparentColor = clB5G5R5;
    gl48 = new Graphics::TBitmap;
    gl48->LoadFromResourceName(0, "gl48");
    gl48->Transparent = true;
    gl48->TransparentColor = clB5G5R5;
    gl49 = new Graphics::TBitmap;
    gl49->LoadFromResourceName(0, "gl49");
    gl49->Transparent = true;
    gl49->TransparentColor = clB5G5R5;
    gl5 = new Graphics::TBitmap;
    gl5->LoadFromResourceName(0, "gl5");
    gl5->Transparent = true;
    gl5->TransparentColor = clB5G5R5;
    gl50 = new Graphics::TBitmap;
    gl50->LoadFromResourceName(0, "gl50");
    gl50->Transparent = true;
    gl50->TransparentColor = clB5G5R5;
    gl51 = new Graphics::TBitmap;
    gl51->LoadFromResourceName(0, "gl51");
    gl51->Transparent = true;
    gl51->TransparentColor = clB5G5R5;
    gl52 = new Graphics::TBitmap;
    gl52->LoadFromResourceName(0, "gl52");
    gl52->Transparent = true;
    gl52->TransparentColor = clB5G5R5;
    gl53 = new Graphics::TBitmap;
    gl53->LoadFromResourceName(0, "gl53");
    gl53->Transparent = true;
    gl53->TransparentColor = clB5G5R5;
    gl54 = new Graphics::TBitmap;
    gl54->LoadFromResourceName(0, "gl54");
    gl54->Transparent = true;
    gl54->TransparentColor = clB5G5R5;
    gl55 = new Graphics::TBitmap;
    gl55->LoadFromResourceName(0, "gl55");
    gl55->Transparent = true;
    gl55->TransparentColor = clB5G5R5;
    gl56 = new Graphics::TBitmap;
    gl56->LoadFromResourceName(0, "gl56");
    gl56->Transparent = true;
    gl56->TransparentColor = clB5G5R5;
    gl57 = new Graphics::TBitmap;
    gl57->LoadFromResourceName(0, "gl57");
    gl57->Transparent = true;
    gl57->TransparentColor = clB5G5R5;
    gl58 = new Graphics::TBitmap;
    gl58->LoadFromResourceName(0, "gl58");
    gl58->Transparent = true;
    gl58->TransparentColor = clB5G5R5;
    gl59 = new Graphics::TBitmap;
    gl59->LoadFromResourceName(0, "gl59");
    gl59->Transparent = true;
    gl59->TransparentColor = clB5G5R5;
    gl6 = new Graphics::TBitmap;
    gl6->LoadFromResourceName(0, "gl6");
    gl6->Transparent = true;
    gl6->TransparentColor = clB5G5R5;
    gl60 = new Graphics::TBitmap;
    gl60->LoadFromResourceName(0, "gl60");
    gl60->Transparent = true;
    gl60->TransparentColor = clB5G5R5;
    gl61 = new Graphics::TBitmap;
    gl61->LoadFromResourceName(0, "gl61");
    gl61->Transparent = true;
    gl61->TransparentColor = clB5G5R5;
    gl62 = new Graphics::TBitmap;
    gl62->LoadFromResourceName(0, "gl62");
    gl62->Transparent = true;
    gl62->TransparentColor = clB5G5R5;
    gl63 = new Graphics::TBitmap;
    gl63->LoadFromResourceName(0, "gl63");
    gl63->Transparent = true;
    gl63->TransparentColor = clB5G5R5;
    gl64 = new Graphics::TBitmap;
    gl64->LoadFromResourceName(0, "gl64");
    gl64->Transparent = true;
    gl64->TransparentColor = clB5G5R5;
    gl65 = new Graphics::TBitmap;
    gl65->LoadFromResourceName(0, "gl65");
    gl65->Transparent = true;
    gl65->TransparentColor = clB5G5R5;
    gl66 = new Graphics::TBitmap;
    gl66->LoadFromResourceName(0, "gl66");
    gl66->Transparent = true;
    gl66->TransparentColor = clB5G5R5;
    gl67 = new Graphics::TBitmap;
    gl67->LoadFromResourceName(0, "gl67");
    gl67->Transparent = true;
    gl67->TransparentColor = clB5G5R5;
    gl68 = new Graphics::TBitmap;
    gl68->LoadFromResourceName(0, "gl68");
    gl68->Transparent = true;
    gl68->TransparentColor = clB5G5R5;
    gl69 = new Graphics::TBitmap;
    gl69->LoadFromResourceName(0, "gl69");
    gl69->Transparent = true;
    gl69->TransparentColor = clB5G5R5;
    gl7 = new Graphics::TBitmap;
    gl7->LoadFromResourceName(0, "gl7");
    gl7->Transparent = true;
    gl7->TransparentColor = clB5G5R5;
    gl70 = new Graphics::TBitmap;
    gl70->LoadFromResourceName(0, "gl70");
    gl70->Transparent = true;
    gl70->TransparentColor = clB5G5R5;
    gl71 = new Graphics::TBitmap;
    gl71->LoadFromResourceName(0, "gl71");
    gl71->Transparent = true;
    gl71->TransparentColor = clB5G5R5;
    gl72 = new Graphics::TBitmap;
    gl72->LoadFromResourceName(0, "gl72");
    gl72->Transparent = true;
    gl72->TransparentColor = clB5G5R5;
    gl73 = new Graphics::TBitmap;
    gl73->LoadFromResourceName(0, "gl73");
    gl73->Transparent = true;
    gl73->TransparentColor = clB5G5R5;
    gl73grounddblred = new Graphics::TBitmap;
    gl73grounddblred->LoadFromResourceName(0, "gl73grounddblred");
    gl73grounddblred->Transparent = true;
    gl73grounddblred->TransparentColor = clB5G5R5;
    gl74 = new Graphics::TBitmap;
    gl74->LoadFromResourceName(0, "gl74");
    gl74->Transparent = true;
    gl74->TransparentColor = clB5G5R5;
    gl74grounddblred = new Graphics::TBitmap;
    gl74grounddblred->LoadFromResourceName(0, "gl74grounddblred");
    gl74grounddblred->Transparent = true;
    gl74grounddblred->TransparentColor = clB5G5R5;
    gl75 = new Graphics::TBitmap;
    gl75->LoadFromResourceName(0, "gl75");
    gl75->Transparent = true;
    gl75->TransparentColor = clB5G5R5;
    gl76 = new Graphics::TBitmap;
    gl76->LoadFromResourceName(0, "gl76");
    gl76->Transparent = true;
    gl76->TransparentColor = clB5G5R5;
    gl76Striped = new Graphics::TBitmap;
    gl76Striped->LoadFromResourceName(0, "gl76Striped");
    gl76Striped->Transparent = true;
    gl76Striped->TransparentColor = clB5G5R5;
    gl77 = new Graphics::TBitmap;
    gl77->LoadFromResourceName(0, "gl77");
    gl77->Transparent = true;
    gl77->TransparentColor = clB5G5R5;
    gl78 = new Graphics::TBitmap;
    gl78->LoadFromResourceName(0, "gl78");
    gl78->Transparent = true;
    gl78->TransparentColor = clB5G5R5;
    gl79 = new Graphics::TBitmap;
    gl79->LoadFromResourceName(0, "gl79");
    gl79->Transparent = true;
    gl79->TransparentColor = clB5G5R5;
    gl79Striped = new Graphics::TBitmap;
    gl79Striped->LoadFromResourceName(0, "gl79Striped");
    gl79Striped->Transparent = true;
    gl79Striped->TransparentColor = clB5G5R5;
    gl8 = new Graphics::TBitmap;
    gl8->LoadFromResourceName(0, "gl8");
    gl8->Transparent = true;
    gl8->TransparentColor = clB5G5R5;
    gl80 = new Graphics::TBitmap;
    gl80->LoadFromResourceName(0, "gl80");
    gl80->Transparent = true;
    gl80->TransparentColor = clB5G5R5;
    gl81 = new Graphics::TBitmap;
    gl81->LoadFromResourceName(0, "gl81");
    gl81->Transparent = true;
    gl81->TransparentColor = clB5G5R5;
    gl82 = new Graphics::TBitmap;
    gl82->LoadFromResourceName(0, "gl82");
    gl82->Transparent = true;
    gl82->TransparentColor = clB5G5R5;
    gl83 = new Graphics::TBitmap;
    gl83->LoadFromResourceName(0, "gl83");
    gl83->Transparent = true;
    gl83->TransparentColor = clB5G5R5;
    gl84 = new Graphics::TBitmap;
    gl84->LoadFromResourceName(0, "gl84");
    gl84->Transparent = true;
    gl84->TransparentColor = clB5G5R5;
    gl85 = new Graphics::TBitmap;
    gl85->LoadFromResourceName(0, "gl85");
    gl85->Transparent = true;
    gl85->TransparentColor = clB5G5R5;
    gl86 = new Graphics::TBitmap;
    gl86->LoadFromResourceName(0, "gl86");
    gl86->Transparent = true;
    gl86->TransparentColor = clB5G5R5;
    gl87 = new Graphics::TBitmap;
    gl87->LoadFromResourceName(0, "gl87");
    gl87->Transparent = true;
    gl87->TransparentColor = clB5G5R5;
    gl88set = new Graphics::TBitmap;
    gl88set->LoadFromResourceName(0, "gl88set");
    gl88set->Transparent = true;
    gl88set->TransparentColor = clB5G5R5;
    gl88unset = new Graphics::TBitmap;
    gl88unset->LoadFromResourceName(0, "gl88unset");
    gl88unset->Transparent = true;
    gl88unset->TransparentColor = clB5G5R5;
    gl89set = new Graphics::TBitmap;
    gl89set->LoadFromResourceName(0, "gl89set");
    gl89set->Transparent = true;
    gl89set->TransparentColor = clB5G5R5;
    gl89unset = new Graphics::TBitmap;
    gl89unset->LoadFromResourceName(0, "gl89unset");
    gl89unset->Transparent = true;
    gl89unset->TransparentColor = clB5G5R5;
    gl9 = new Graphics::TBitmap;
    gl9->LoadFromResourceName(0, "gl9");
    gl9->Transparent = true;
    gl9->TransparentColor = clB5G5R5;
    gl90set = new Graphics::TBitmap;
    gl90set->LoadFromResourceName(0, "gl90set");
    gl90set->Transparent = true;
    gl90set->TransparentColor = clB5G5R5;
    gl90unset = new Graphics::TBitmap;
    gl90unset->LoadFromResourceName(0, "gl90unset");
    gl90unset->Transparent = true;
    gl90unset->TransparentColor = clB5G5R5;
    gl91set = new Graphics::TBitmap;
    gl91set->LoadFromResourceName(0, "gl91set");
    gl91set->Transparent = true;
    gl91set->TransparentColor = clB5G5R5;
    gl91unset = new Graphics::TBitmap;
    gl91unset->LoadFromResourceName(0, "gl91unset");
    gl91unset->Transparent = true;
    gl91unset->TransparentColor = clB5G5R5;
    gl92set = new Graphics::TBitmap;
    gl92set->LoadFromResourceName(0, "gl92set");
    gl92set->Transparent = true;
    gl92set->TransparentColor = clB5G5R5;
    gl92unset = new Graphics::TBitmap;
    gl92unset->LoadFromResourceName(0, "gl92unset");
    gl92unset->Transparent = true;
    gl92unset->TransparentColor = clB5G5R5;
    gl93set = new Graphics::TBitmap;
    gl93set->LoadFromResourceName(0, "gl93set");
    gl93set->Transparent = true;
    gl93set->TransparentColor = clB5G5R5;
    gl94set = new Graphics::TBitmap;
    gl94set->LoadFromResourceName(0, "gl94set");
    gl94set->Transparent = true;
    gl94set->TransparentColor = clB5G5R5;
    gl95set = new Graphics::TBitmap;
    gl95set->LoadFromResourceName(0, "gl95set");
    gl95set->Transparent = true;
    gl95set->TransparentColor = clB5G5R5;
    gl95unset = new Graphics::TBitmap;
    gl95unset->LoadFromResourceName(0, "gl95unset");
    gl95unset->Transparent = true;
    gl95unset->TransparentColor = clB5G5R5;
    gl97 = new Graphics::TBitmap;
    gl97->LoadFromResourceName(0, "gl97");
    gl97->Transparent = true;
    gl97->TransparentColor = clB5G5R5;
    gl98 = new Graphics::TBitmap;
    gl98->LoadFromResourceName(0, "gl98");
    gl98->Transparent = true;
    gl98->TransparentColor = clB5G5R5;
    gl99 = new Graphics::TBitmap;
    gl99->LoadFromResourceName(0, "gl99");
    gl99->Transparent = true;
    gl99->TransparentColor = clB5G5R5;
    Plat68 = new Graphics::TBitmap;
    Plat68->LoadFromResourceName(0, "Plat68");
    Plat68->Transparent = true;
    Plat68->TransparentColor = clB5G5R5;
    Plat68Striped = new Graphics::TBitmap;
    Plat68Striped->LoadFromResourceName(0, "Plat68Striped");
    Plat68Striped->Transparent = true;
    Plat68Striped->TransparentColor = clB5G5R5;
    Plat69 = new Graphics::TBitmap;
    Plat69->LoadFromResourceName(0, "Plat69");
    Plat69->Transparent = true;
    Plat69->TransparentColor = clB5G5R5;
    Plat69Striped = new Graphics::TBitmap;
    Plat69Striped->LoadFromResourceName(0, "Plat69Striped");
    Plat69Striped->Transparent = true;
    Plat69Striped->TransparentColor = clB5G5R5;
    Plat70 = new Graphics::TBitmap;
    Plat70->LoadFromResourceName(0, "Plat70");
    Plat70->Transparent = true;
    Plat70->TransparentColor = clB5G5R5;
    Plat70Striped = new Graphics::TBitmap;
    Plat70Striped->LoadFromResourceName(0, "Plat70Striped");
    Plat70Striped->Transparent = true;
    Plat70Striped->TransparentColor = clB5G5R5;
    Plat71 = new Graphics::TBitmap;
    Plat71->LoadFromResourceName(0, "Plat71");
    Plat71->Transparent = true;
    Plat71->TransparentColor = clB5G5R5;
    Plat71Striped = new Graphics::TBitmap;
    Plat71Striped->LoadFromResourceName(0, "Plat71Striped");
    Plat71Striped->Transparent = true;
    Plat71Striped->TransparentColor = clB5G5R5;
    sm1 = new Graphics::TBitmap;
    sm1->LoadFromResourceName(0, "sm1");
    sm1->Transparent = true;
    sm1->TransparentColor = clB5G5R5;
    sm10 = new Graphics::TBitmap;
    sm10->LoadFromResourceName(0, "sm10");
    sm10->Transparent = true;
    sm10->TransparentColor = clB5G5R5;
    sm100 = new Graphics::TBitmap;
    sm100->LoadFromResourceName(0, "sm100");
    sm100->Transparent = true;
    sm100->TransparentColor = clB5G5R5;
    sm101 = new Graphics::TBitmap;
    sm101->LoadFromResourceName(0, "sm101");
    sm101->Transparent = true;
    sm101->TransparentColor = clB5G5R5;
    sm102 = new Graphics::TBitmap;
    sm102->LoadFromResourceName(0, "sm102");
    sm102->Transparent = true;
    sm102->TransparentColor = clB5G5R5;
    sm103 = new Graphics::TBitmap;
    sm103->LoadFromResourceName(0, "sm103");
    sm103->Transparent = true;
    sm103->TransparentColor = clB5G5R5;
    sm104 = new Graphics::TBitmap;
    sm104->LoadFromResourceName(0, "sm104");
    sm104->Transparent = true;
    sm104->TransparentColor = clB5G5R5;
    sm105 = new Graphics::TBitmap;
    sm105->LoadFromResourceName(0, "sm105");
    sm105->Transparent = true;
    sm105->TransparentColor = clB5G5R5;
    sm106 = new Graphics::TBitmap;
    sm106->LoadFromResourceName(0, "sm106");
    sm106->Transparent = true;
    sm106->TransparentColor = clB5G5R5;
    sm107 = new Graphics::TBitmap;
    sm107->LoadFromResourceName(0, "sm107");
    sm107->Transparent = true;
    sm107->TransparentColor = clB5G5R5;
    sm108 = new Graphics::TBitmap;
    sm108->LoadFromResourceName(0, "sm108");
    sm108->Transparent = true;
    sm108->TransparentColor = clB5G5R5;
    sm109 = new Graphics::TBitmap;
    sm109->LoadFromResourceName(0, "sm109");
    sm109->Transparent = true;
    sm109->TransparentColor = clB5G5R5;
    sm11 = new Graphics::TBitmap;
    sm11->LoadFromResourceName(0, "sm11");
    sm11->Transparent = true;
    sm11->TransparentColor = clB5G5R5;
    sm110 = new Graphics::TBitmap;
    sm110->LoadFromResourceName(0, "sm110");
    sm110->Transparent = true;
    sm110->TransparentColor = clB5G5R5;
    sm111 = new Graphics::TBitmap;
    sm111->LoadFromResourceName(0, "sm111");
    sm111->Transparent = true;
    sm111->TransparentColor = clB5G5R5;
    sm112 = new Graphics::TBitmap;
    sm112->LoadFromResourceName(0, "sm112");
    sm112->Transparent = true;
    sm112->TransparentColor = clB5G5R5;
    sm115 = new Graphics::TBitmap;
    sm115->LoadFromResourceName(0, "sm115");
    sm115->Transparent = true;
    sm115->TransparentColor = clB5G5R5;
    sm117 = new Graphics::TBitmap;
    sm117->LoadFromResourceName(0, "sm117");
    sm117->Transparent = true;
    sm117->TransparentColor = clB5G5R5;
    sm12 = new Graphics::TBitmap;
    sm12->LoadFromResourceName(0, "sm12");
    sm12->Transparent = true;
    sm12->TransparentColor = clB5G5R5;
    sm129 = new Graphics::TBitmap;
    sm129->LoadFromResourceName(0, "sm129");
    sm129->Transparent = true;
    sm129->TransparentColor = clB5G5R5;
    sm129striped = new Graphics::TBitmap;
    sm129striped->LoadFromResourceName(0, "sm129striped");
    sm129striped->Transparent = true;
    sm129striped->TransparentColor = clB5G5R5;
    sm13 = new Graphics::TBitmap;
    sm13->LoadFromResourceName(0, "sm13");
    sm13->Transparent = true;
    sm13->TransparentColor = clB5G5R5;
    sm130 = new Graphics::TBitmap;
    sm130->LoadFromResourceName(0, "sm130");
    sm130->Transparent = true;
    sm130->TransparentColor = clB5G5R5;
    sm130striped = new Graphics::TBitmap;
    sm130striped->LoadFromResourceName(0, "sm130striped");
    sm130striped->Transparent = true;
    sm130striped->TransparentColor = clB5G5R5;
    sm131striped = new Graphics::TBitmap;
    sm131striped->LoadFromResourceName(0, "sm131striped");
    sm131striped->Transparent = true;
    sm131striped->TransparentColor = clB5G5R5;
    sm132 = new Graphics::TBitmap;
    sm132->LoadFromResourceName(0, "sm132");
    sm132->Transparent = true;
    sm132->TransparentColor = clB5G5R5;
    sm133 = new Graphics::TBitmap;
    sm133->LoadFromResourceName(0, "sm133");
    sm133->Transparent = true;
    sm133->TransparentColor = clB5G5R5;
    sm134 = new Graphics::TBitmap;
    sm134->LoadFromResourceName(0, "sm134");
    sm134->Transparent = true;
    sm134->TransparentColor = clB5G5R5;
    sm135 = new Graphics::TBitmap;
    sm135->LoadFromResourceName(0, "sm135");
    sm135->Transparent = true;
    sm135->TransparentColor = clB5G5R5;
    sm136 = new Graphics::TBitmap;
    sm136->LoadFromResourceName(0, "sm136");
    sm136->Transparent = true;
    sm136->TransparentColor = clB5G5R5;
    sm137 = new Graphics::TBitmap;
    sm137->LoadFromResourceName(0, "sm137");
    sm137->Transparent = true;
    sm137->TransparentColor = clB5G5R5;
    sm138 = new Graphics::TBitmap;
    sm138->LoadFromResourceName(0, "sm138");
    sm138->Transparent = true;
    sm138->TransparentColor = clB5G5R5;
    sm139 = new Graphics::TBitmap;
    sm139->LoadFromResourceName(0, "sm139");
    sm139->Transparent = true;
    sm139->TransparentColor = clB5G5R5;
    sm14 = new Graphics::TBitmap;
    sm14->LoadFromResourceName(0, "sm14");
    sm14->Transparent = true;
    sm14->TransparentColor = clB5G5R5;
    sm15 = new Graphics::TBitmap;
    sm15->LoadFromResourceName(0, "sm15");
    sm15->Transparent = true;
    sm15->TransparentColor = clB5G5R5;
    sm16 = new Graphics::TBitmap;
    sm16->LoadFromResourceName(0, "sm16");
    sm16->Transparent = true;
    sm16->TransparentColor = clB5G5R5;
    sm18 = new Graphics::TBitmap;
    sm18->LoadFromResourceName(0, "sm18");
    sm18->Transparent = true;
    sm18->TransparentColor = clB5G5R5;
    sm19 = new Graphics::TBitmap;
    sm19->LoadFromResourceName(0, "sm19");
    sm19->Transparent = true;
    sm19->TransparentColor = clB5G5R5;
    sm2 = new Graphics::TBitmap;
    sm2->LoadFromResourceName(0, "sm2");
    sm2->Transparent = true;
    sm2->TransparentColor = clB5G5R5;
    sm20 = new Graphics::TBitmap;
    sm20->LoadFromResourceName(0, "sm20");
    sm20->Transparent = true;
    sm20->TransparentColor = clB5G5R5;
    sm21 = new Graphics::TBitmap;
    sm21->LoadFromResourceName(0, "sm21");
    sm21->Transparent = true;
    sm21->TransparentColor = clB5G5R5;
    sm22 = new Graphics::TBitmap;
    sm22->LoadFromResourceName(0, "sm22");
    sm22->Transparent = true;
    sm22->TransparentColor = clB5G5R5;
    sm23 = new Graphics::TBitmap;
    sm23->LoadFromResourceName(0, "sm23");
    sm23->Transparent = true;
    sm23->TransparentColor = clB5G5R5;
    sm24 = new Graphics::TBitmap;
    sm24->LoadFromResourceName(0, "sm24");
    sm24->Transparent = true;
    sm24->TransparentColor = clB5G5R5;
    sm25 = new Graphics::TBitmap;
    sm25->LoadFromResourceName(0, "sm25");
    sm25->Transparent = true;
    sm25->TransparentColor = clB5G5R5;
    sm26 = new Graphics::TBitmap;
    sm26->LoadFromResourceName(0, "sm26");
    sm26->Transparent = true;
    sm26->TransparentColor = clB5G5R5;
    sm27 = new Graphics::TBitmap;
    sm27->LoadFromResourceName(0, "sm27");
    sm27->Transparent = true;
    sm27->TransparentColor = clB5G5R5;
    sm28 = new Graphics::TBitmap;
    sm28->LoadFromResourceName(0, "sm28");
    sm28->Transparent = true;
    sm28->TransparentColor = clB5G5R5;
    sm29 = new Graphics::TBitmap;
    sm29->LoadFromResourceName(0, "sm29");
    sm29->Transparent = true;
    sm29->TransparentColor = clB5G5R5;
    sm3 = new Graphics::TBitmap;
    sm3->LoadFromResourceName(0, "sm3");
    sm3->Transparent = true;
    sm3->TransparentColor = clB5G5R5;
    sm30 = new Graphics::TBitmap;
    sm30->LoadFromResourceName(0, "sm30");
    sm30->Transparent = true;
    sm30->TransparentColor = clB5G5R5;
    sm31 = new Graphics::TBitmap;
    sm31->LoadFromResourceName(0, "sm31");
    sm31->Transparent = true;
    sm31->TransparentColor = clB5G5R5;
    sm32 = new Graphics::TBitmap;
    sm32->LoadFromResourceName(0, "sm32");
    sm32->Transparent = true;
    sm32->TransparentColor = clB5G5R5;
    sm33 = new Graphics::TBitmap;
    sm33->LoadFromResourceName(0, "sm33");
    sm33->Transparent = true;
    sm33->TransparentColor = clB5G5R5;
    sm34 = new Graphics::TBitmap;
    sm34->LoadFromResourceName(0, "sm34");
    sm34->Transparent = true;
    sm34->TransparentColor = clB5G5R5;
    sm35 = new Graphics::TBitmap;
    sm35->LoadFromResourceName(0, "sm35");
    sm35->Transparent = true;
    sm35->TransparentColor = clB5G5R5;
    sm36 = new Graphics::TBitmap;
    sm36->LoadFromResourceName(0, "sm36");
    sm36->Transparent = true;
    sm36->TransparentColor = clB5G5R5;
    sm37 = new Graphics::TBitmap;
    sm37->LoadFromResourceName(0, "sm37");
    sm37->Transparent = true;
    sm37->TransparentColor = clB5G5R5;
    sm38 = new Graphics::TBitmap;
    sm38->LoadFromResourceName(0, "sm38");
    sm38->Transparent = true;
    sm38->TransparentColor = clB5G5R5;
    sm39 = new Graphics::TBitmap;
    sm39->LoadFromResourceName(0, "sm39");
    sm39->Transparent = true;
    sm39->TransparentColor = clB5G5R5;
    sm4 = new Graphics::TBitmap;
    sm4->LoadFromResourceName(0, "sm4");
    sm4->Transparent = true;
    sm4->TransparentColor = clB5G5R5;
    sm40 = new Graphics::TBitmap;
    sm40->LoadFromResourceName(0, "sm40");
    sm40->Transparent = true;
    sm40->TransparentColor = clB5G5R5;
    sm41 = new Graphics::TBitmap;
    sm41->LoadFromResourceName(0, "sm41");
    sm41->Transparent = true;
    sm41->TransparentColor = clB5G5R5;
    sm42 = new Graphics::TBitmap;
    sm42->LoadFromResourceName(0, "sm42");
    sm42->Transparent = true;
    sm42->TransparentColor = clB5G5R5;
    sm43 = new Graphics::TBitmap;
    sm43->LoadFromResourceName(0, "sm43");
    sm43->Transparent = true;
    sm43->TransparentColor = clB5G5R5;
    sm44 = new Graphics::TBitmap;
    sm44->LoadFromResourceName(0, "sm44");
    sm44->Transparent = true;
    sm44->TransparentColor = clB5G5R5;
    sm45 = new Graphics::TBitmap;
    sm45->LoadFromResourceName(0, "sm45");
    sm45->Transparent = true;
    sm45->TransparentColor = clB5G5R5;
    sm46 = new Graphics::TBitmap;
    sm46->LoadFromResourceName(0, "sm46");
    sm46->Transparent = true;
    sm46->TransparentColor = clB5G5R5;
    sm47 = new Graphics::TBitmap;
    sm47->LoadFromResourceName(0, "sm47");
    sm47->Transparent = true;
    sm47->TransparentColor = clB5G5R5;
    sm48 = new Graphics::TBitmap;
    sm48->LoadFromResourceName(0, "sm48");
    sm48->Transparent = true;
    sm48->TransparentColor = clB5G5R5;
    sm49 = new Graphics::TBitmap;
    sm49->LoadFromResourceName(0, "sm49");
    sm49->Transparent = true;
    sm49->TransparentColor = clB5G5R5;
    sm5 = new Graphics::TBitmap;
    sm5->LoadFromResourceName(0, "sm5");
    sm5->Transparent = true;
    sm5->TransparentColor = clB5G5R5;
    sm50 = new Graphics::TBitmap;
    sm50->LoadFromResourceName(0, "sm50");
    sm50->Transparent = true;
    sm50->TransparentColor = clB5G5R5;
    sm51 = new Graphics::TBitmap;
    sm51->LoadFromResourceName(0, "sm51");
    sm51->Transparent = true;
    sm51->TransparentColor = clB5G5R5;
    sm52 = new Graphics::TBitmap;
    sm52->LoadFromResourceName(0, "sm52");
    sm52->Transparent = true;
    sm52->TransparentColor = clB5G5R5;
    sm53 = new Graphics::TBitmap;
    sm53->LoadFromResourceName(0, "sm53");
    sm53->Transparent = true;
    sm53->TransparentColor = clB5G5R5;
    sm54 = new Graphics::TBitmap;
    sm54->LoadFromResourceName(0, "sm54");
    sm54->Transparent = true;
    sm54->TransparentColor = clB5G5R5;
    sm55 = new Graphics::TBitmap;
    sm55->LoadFromResourceName(0, "sm55");
    sm55->Transparent = true;
    sm55->TransparentColor = clB5G5R5;
    sm56 = new Graphics::TBitmap;
    sm56->LoadFromResourceName(0, "sm56");
    sm56->Transparent = true;
    sm56->TransparentColor = clB5G5R5;
    sm57 = new Graphics::TBitmap;
    sm57->LoadFromResourceName(0, "sm57");
    sm57->Transparent = true;
    sm57->TransparentColor = clB5G5R5;
    sm58 = new Graphics::TBitmap;
    sm58->LoadFromResourceName(0, "sm58");
    sm58->Transparent = true;
    sm58->TransparentColor = clB5G5R5;
    sm59 = new Graphics::TBitmap;
    sm59->LoadFromResourceName(0, "sm59");
    sm59->Transparent = true;
    sm59->TransparentColor = clB5G5R5;
    sm6 = new Graphics::TBitmap;
    sm6->LoadFromResourceName(0, "sm6");
    sm6->Transparent = true;
    sm6->TransparentColor = clB5G5R5;
    sm60 = new Graphics::TBitmap;
    sm60->LoadFromResourceName(0, "sm60");
    sm60->Transparent = true;
    sm60->TransparentColor = clB5G5R5;
    sm61 = new Graphics::TBitmap;
    sm61->LoadFromResourceName(0, "sm61");
    sm61->Transparent = true;
    sm61->TransparentColor = clB5G5R5;
    sm62 = new Graphics::TBitmap;
    sm62->LoadFromResourceName(0, "sm62");
    sm62->Transparent = true;
    sm62->TransparentColor = clB5G5R5;
    sm63 = new Graphics::TBitmap;
    sm63->LoadFromResourceName(0, "sm63");
    sm63->Transparent = true;
    sm63->TransparentColor = clB5G5R5;
    sm64 = new Graphics::TBitmap;
    sm64->LoadFromResourceName(0, "sm64");
    sm64->Transparent = true;
    sm64->TransparentColor = clB5G5R5;
    sm65 = new Graphics::TBitmap;
    sm65->LoadFromResourceName(0, "sm65");
    sm65->Transparent = true;
    sm65->TransparentColor = clB5G5R5;
    sm66 = new Graphics::TBitmap;
    sm66->LoadFromResourceName(0, "sm66");
    sm66->Transparent = true;
    sm66->TransparentColor = clB5G5R5;
    sm67 = new Graphics::TBitmap;
    sm67->LoadFromResourceName(0, "sm67");
    sm67->Transparent = true;
    sm67->TransparentColor = clB5G5R5;
    sm7 = new Graphics::TBitmap;
    sm7->LoadFromResourceName(0, "sm7");
    sm7->Transparent = true;
    sm7->TransparentColor = clB5G5R5;
    sm76 = new Graphics::TBitmap;
    sm76->LoadFromResourceName(0, "sm76");
    sm76->Transparent = true;
    sm76->TransparentColor = clB5G5R5;
    sm76striped = new Graphics::TBitmap;
    sm76striped->LoadFromResourceName(0, "sm76striped");
    sm76striped->Transparent = true;
    sm76striped->TransparentColor = clB5G5R5;
    sm77 = new Graphics::TBitmap;
    sm77->LoadFromResourceName(0, "sm77");
    sm77->Transparent = true;
    sm77->TransparentColor = clB5G5R5;
    sm77striped = new Graphics::TBitmap;
    sm77striped->LoadFromResourceName(0, "sm77striped");
    sm77striped->Transparent = true;
    sm77striped->TransparentColor = clB5G5R5;
    sm78 = new Graphics::TBitmap;
    sm78->LoadFromResourceName(0, "sm78");
    sm78->Transparent = true;
    sm78->TransparentColor = clB5G5R5;
    sm78striped = new Graphics::TBitmap;
    sm78striped->LoadFromResourceName(0, "sm78striped");
    sm78striped->Transparent = true;
    sm78striped->TransparentColor = clB5G5R5;
    sm79 = new Graphics::TBitmap;
    sm79->LoadFromResourceName(0, "sm79");
    sm79->Transparent = true;
    sm79->TransparentColor = clB5G5R5;
    sm79striped = new Graphics::TBitmap;
    sm79striped->LoadFromResourceName(0, "sm79striped");
    sm79striped->Transparent = true;
    sm79striped->TransparentColor = clB5G5R5;
    sm8 = new Graphics::TBitmap;
    sm8->LoadFromResourceName(0, "sm8");
    sm8->Transparent = true;
    sm8->TransparentColor = clB5G5R5;
    sm80 = new Graphics::TBitmap;
    sm80->LoadFromResourceName(0, "sm80");
    sm80->Transparent = true;
    sm80->TransparentColor = clB5G5R5;
    sm81 = new Graphics::TBitmap;
    sm81->LoadFromResourceName(0, "sm81");
    sm81->Transparent = true;
    sm81->TransparentColor = clB5G5R5;
    sm82 = new Graphics::TBitmap;
    sm82->LoadFromResourceName(0, "sm82");
    sm82->Transparent = true;
    sm82->TransparentColor = clB5G5R5;
    sm83 = new Graphics::TBitmap;
    sm83->LoadFromResourceName(0, "sm83");
    sm83->Transparent = true;
    sm83->TransparentColor = clB5G5R5;
    sm84 = new Graphics::TBitmap;
    sm84->LoadFromResourceName(0, "sm84");
    sm84->Transparent = true;
    sm84->TransparentColor = clB5G5R5;
    sm85 = new Graphics::TBitmap;
    sm85->LoadFromResourceName(0, "sm85");
    sm85->Transparent = true;
    sm85->TransparentColor = clB5G5R5;
    sm86 = new Graphics::TBitmap;
    sm86->LoadFromResourceName(0, "sm86");
    sm86->Transparent = true;
    sm86->TransparentColor = clB5G5R5;
    sm87 = new Graphics::TBitmap;
    sm87->LoadFromResourceName(0, "sm87");
    sm87->Transparent = true;
    sm87->TransparentColor = clB5G5R5;
    sm88 = new Graphics::TBitmap;
    sm88->LoadFromResourceName(0, "sm88");
    sm88->Transparent = true;
    sm88->TransparentColor = clB5G5R5;
    sm89 = new Graphics::TBitmap;
    sm89->LoadFromResourceName(0, "sm89");
    sm89->Transparent = true;
    sm89->TransparentColor = clB5G5R5;
    sm9 = new Graphics::TBitmap;
    sm9->LoadFromResourceName(0, "sm9");
    sm9->Transparent = true;
    sm9->TransparentColor = clB5G5R5;
    sm90 = new Graphics::TBitmap;
    sm90->LoadFromResourceName(0, "sm90");
    sm90->Transparent = true;
    sm90->TransparentColor = clB5G5R5;
    sm91 = new Graphics::TBitmap;
    sm91->LoadFromResourceName(0, "sm91");
    sm91->Transparent = true;
    sm91->TransparentColor = clB5G5R5;
    sm92 = new Graphics::TBitmap;
    sm92->LoadFromResourceName(0, "sm92");
    sm92->Transparent = true;
    sm92->TransparentColor = clB5G5R5;
    sm93 = new Graphics::TBitmap;
    sm93->LoadFromResourceName(0, "sm93");
    sm93->Transparent = true;
    sm93->TransparentColor = clB5G5R5;
    sm94 = new Graphics::TBitmap;
    sm94->LoadFromResourceName(0, "sm94");
    sm94->Transparent = true;
    sm94->TransparentColor = clB5G5R5;
    sm95 = new Graphics::TBitmap;
    sm95->LoadFromResourceName(0, "sm95");
    sm95->Transparent = true;
    sm95->TransparentColor = clB5G5R5;
    sm96 = new Graphics::TBitmap;
    sm96->LoadFromResourceName(0, "sm96");
    sm96->Transparent = true;
    sm96->TransparentColor = clB5G5R5;
    sm96striped = new Graphics::TBitmap;
    sm96striped->LoadFromResourceName(0, "sm96striped");
    sm96striped->Transparent = true;
    sm96striped->TransparentColor = clB5G5R5;
    sm97 = new Graphics::TBitmap;
    sm97->LoadFromResourceName(0, "sm97");
    sm97->Transparent = true;
    sm97->TransparentColor = clB5G5R5;
    sm98 = new Graphics::TBitmap;
    sm98->LoadFromResourceName(0, "sm98");
    sm98->Transparent = true;
    sm98->TransparentColor = clB5G5R5;
    sm99 = new Graphics::TBitmap;
    sm99->LoadFromResourceName(0, "sm99");
    sm99->Transparent = true;
    sm99->TransparentColor = clB5G5R5;
    smBlack = new Graphics::TBitmap;
    smBlack->LoadFromResourceName(0, "smBlack");
    smBlack->Transparent = true;
    smBlack->TransparentColor = clB5G5R5;
    smBlue = new Graphics::TBitmap;
    smBlue->LoadFromResourceName(0, "smBlue");
    smBlue->Transparent = true;
    smBlue->TransparentColor = clB5G5R5;
    smBrightGreen = new Graphics::TBitmap;
    smBrightGreen->LoadFromResourceName(0, "smBrightGreen");
    smBrightGreen->Transparent = true;
    smBrightGreen->TransparentColor = clB5G5R5;
    smCaramel = new Graphics::TBitmap;
    smCaramel->LoadFromResourceName(0, "smCaramel");
    smCaramel->Transparent = true;
    smCaramel->TransparentColor = clB5G5R5;
    smCyan = new Graphics::TBitmap;
    smCyan->LoadFromResourceName(0, "smCyan");
    smCyan->Transparent = true;
    smCyan->TransparentColor = clB5G5R5;
    smLC = new Graphics::TBitmap;      //added v2.9.0 to show in zoom out mode
    smLC->LoadFromResourceName(0, "smLC");
    smLC->Transparent = true;
    smLC->TransparentColor = clB5G5R5;
    smLightBlue = new Graphics::TBitmap;
    smLightBlue->LoadFromResourceName(0, "smLightBlue");
    smLightBlue->Transparent = true;
    smLightBlue->TransparentColor = clB5G5R5;
    smMagenta = new Graphics::TBitmap;
    smMagenta->LoadFromResourceName(0, "smMagenta");
    smMagenta->Transparent = true;
    smMagenta->TransparentColor = clB5G5R5;
    smName = new Graphics::TBitmap;
    smName->LoadFromResourceName(0, "smName");
    smName->Transparent = true;
    smName->TransparentColor = clB5G5R5;
    smOrange = new Graphics::TBitmap;
    smOrange->LoadFromResourceName(0, "smOrange");
    smOrange->Transparent = true;
    smOrange->TransparentColor = clB5G5R5;
    smPaleGreen = new Graphics::TBitmap;
    smPaleGreen->LoadFromResourceName(0, "smPaleGreen");
    smPaleGreen->Transparent = true;
    smPaleGreen->TransparentColor = clB5G5R5;
    smRed = new Graphics::TBitmap;
    smRed->LoadFromResourceName(0, "smRed");
    smRed->Transparent = true;
    smRed->TransparentColor = clB5G5R5;
    smYellow = new Graphics::TBitmap;
    smYellow->LoadFromResourceName(0, "smYellow");
    smYellow->Transparent = true;
    smYellow->TransparentColor = clB5G5R5;
    smTransparent = new Graphics::TBitmap;
    smTransparent->LoadFromResourceName(0, "smTransparent");
    smTransparent->Transparent = true;
    smTransparent->TransparentColor = clB5G5R5;
    TempBackground = new Graphics::TBitmap;
    TempBackground->LoadFromResourceName(0, "TempBackground");
    TempBackground->Transparent = true;
    TempBackground->TransparentColor = clB5G5R5;
    TempHeadCode = new Graphics::TBitmap;
    TempHeadCode->LoadFromResourceName(0, "TempHeadCode");
    TempHeadCode->Transparent = true;
    TempHeadCode->TransparentColor = clB5G5R5;
    UnderHFootbridge = new Graphics::TBitmap;
    UnderHFootbridge->LoadFromResourceName(0, "UnderHFootbridge");
    UnderHFootbridge->Transparent = true;
    UnderHFootbridge->TransparentColor = clB5G5R5;
    UnderVFootbridge = new Graphics::TBitmap;
    UnderVFootbridge->LoadFromResourceName(0, "UnderVFootbridge");
    UnderVFootbridge->Transparent = true;
    UnderVFootbridge->TransparentColor = clB5G5R5;

    // extra from bmSolidBgnd bitmap file but transparent
    bmTransparentBgnd = new Graphics::TBitmap;
    bmTransparentBgnd->LoadFromResourceName(0, "bmSolidBgnd");
    bmTransparentBgnd->Transparent = true;
    bmTransparentBgnd->TransparentColor = clB5G5R5;

    // level crossing graphics
    LCBothHor = new Graphics::TBitmap;
    LCBothHor->LoadFromResourceName(0, "LCBothHor");
    LCBothHor->Transparent = true;
    LCBothHor->TransparentColor = clB5G5R5;
    LCBothHorMan = new Graphics::TBitmap;
    LCBothHorMan->LoadFromResourceName(0, "LCBothHorMan");
    LCBothHorMan->Transparent = true;
    LCBothHorMan->TransparentColor = clB5G5R5;
    LCBotHor = new Graphics::TBitmap;
    LCBotHor->LoadFromResourceName(0, "LCBotHor");
    LCBotHor->Transparent = true;
    LCBotHor->TransparentColor = clB5G5R5;
    LCBotHorMan = new Graphics::TBitmap;
    LCBotHorMan->LoadFromResourceName(0, "LCBotHorMan");
    LCBotHorMan->Transparent = true;
    LCBotHorMan->TransparentColor = clB5G5R5;
    LCBothVer = new Graphics::TBitmap;
    LCBothVer->LoadFromResourceName(0, "LCBothVer");
    LCBothVer->Transparent = true;
    LCBothVer->TransparentColor = clB5G5R5;
    LCBothVerMan = new Graphics::TBitmap;
    LCBothVerMan->LoadFromResourceName(0, "LCBothVerMan");
    LCBothVerMan->Transparent = true;
    LCBothVerMan->TransparentColor = clB5G5R5;
    LCLHSVer = new Graphics::TBitmap;
    LCLHSVer->LoadFromResourceName(0, "LCLHSVer");
    LCLHSVer->Transparent = true;
    LCLHSVer->TransparentColor = clB5G5R5;
    LCLHSVerMan = new Graphics::TBitmap;
    LCLHSVerMan->LoadFromResourceName(0, "LCLHSVerMan");
    LCLHSVerMan->Transparent = true;
    LCLHSVerMan->TransparentColor = clB5G5R5;
    LCPlain = new Graphics::TBitmap;
    LCPlain->LoadFromResourceName(0, "LCPlain");
    LCPlain->Transparent = true;
    LCPlain->TransparentColor = clB5G5R5;
    LCPlainMan = new Graphics::TBitmap;
    LCPlainMan->LoadFromResourceName(0, "LCPlainMan");
    LCPlainMan->Transparent = true;
    LCPlainMan->TransparentColor = clB5G5R5;
    LCRHSVer = new Graphics::TBitmap;
    LCRHSVer->LoadFromResourceName(0, "LCRHSVer");
    LCRHSVer->Transparent = true;
    LCRHSVer->TransparentColor = clB5G5R5;
    LCRHSVerMan = new Graphics::TBitmap;
    LCRHSVerMan->LoadFromResourceName(0, "LCRHSVerMan");
    LCRHSVerMan->Transparent = true;
    LCRHSVerMan->TransparentColor = clB5G5R5;
    LCTopHor = new Graphics::TBitmap;
    LCTopHor->LoadFromResourceName(0, "LCTopHor");
    LCTopHor->Transparent = true;
    LCTopHor->TransparentColor = clB5G5R5;
    LCTopHorMan = new Graphics::TBitmap;
    LCTopHorMan->LoadFromResourceName(0, "LCTopHorMan");
    LCTopHorMan->Transparent = true;
    LCTopHorMan->TransparentColor = clB5G5R5;

    // additional pointers copied from existing pointers
    sm68 = sm1;
    sm69 = sm1;
    sm70 = sm2;
    sm71 = sm2;
    sm72 = sm19;
    sm73 = sm18;
    sm74 = sm18;
    sm75 = sm19;
    sm113 = sm111;
    sm114 = sm112;
    sm116 = sm115;
    sm118 = sm117;
    sm119 = sm114;
    sm120 = sm117;
    sm121 = sm115;
    sm122 = sm111;
    sm123 = sm111;
    sm124 = sm115;
    sm125 = sm1;
    sm126 = sm1;
    sm127 = sm2;
    sm128 = sm2;

    // non-transparent graphics
    Code_a = new Graphics::TBitmap;
    Code_a->LoadFromResourceName(0, "Code_a");
    Code_a->Transparent = false;
    Code_b = new Graphics::TBitmap;
    Code_b->LoadFromResourceName(0, "Code_b");
    Code_b->Transparent = false;
    Code_c = new Graphics::TBitmap;
    Code_c->LoadFromResourceName(0, "Code_c");
    Code_c->Transparent = false;
    Code_d = new Graphics::TBitmap;
    Code_d->LoadFromResourceName(0, "Code_d");
    Code_d->Transparent = false;
    Code_e = new Graphics::TBitmap;
    Code_e->LoadFromResourceName(0, "Code_e");
    Code_e->Transparent = false;
    Code_f = new Graphics::TBitmap;
    Code_f->LoadFromResourceName(0, "Code_f");
    Code_f->Transparent = false;
    Code_g = new Graphics::TBitmap;
    Code_g->LoadFromResourceName(0, "Code_g");
    Code_g->Transparent = false;
    Code_h = new Graphics::TBitmap;
    Code_h->LoadFromResourceName(0, "Code_h");
    Code_h->Transparent = false;
    Code_i = new Graphics::TBitmap;
    Code_i->LoadFromResourceName(0, "Code_i");
    Code_i->Transparent = false;
    Code_j = new Graphics::TBitmap;
    Code_j->LoadFromResourceName(0, "Code_j");
    Code_j->Transparent = false;
    Code_k = new Graphics::TBitmap;
    Code_k->LoadFromResourceName(0, "Code_k");
    Code_k->Transparent = false;
    Code_l = new Graphics::TBitmap;
    Code_l->LoadFromResourceName(0, "Code_l");
    Code_l->Transparent = false;
    Code_m = new Graphics::TBitmap;
    Code_m->LoadFromResourceName(0, "Code_m");
    Code_m->Transparent = false;
    Code_n = new Graphics::TBitmap;
    Code_n->LoadFromResourceName(0, "Code_n");
    Code_n->Transparent = false;
    Code_o = new Graphics::TBitmap;
    Code_o->LoadFromResourceName(0, "Code_o");
    Code_o->Transparent = false;
    Code_p = new Graphics::TBitmap;
    Code_p->LoadFromResourceName(0, "Code_p");
    Code_p->Transparent = false;
    Code_q = new Graphics::TBitmap;
    Code_q->LoadFromResourceName(0, "Code_q");
    Code_q->Transparent = false;
    Code_r = new Graphics::TBitmap;
    Code_r->LoadFromResourceName(0, "Code_r");
    Code_r->Transparent = false;
    Code_s = new Graphics::TBitmap;
    Code_s->LoadFromResourceName(0, "Code_s");
    Code_s->Transparent = false;
    Code_t = new Graphics::TBitmap;
    Code_t->LoadFromResourceName(0, "Code_t");
    Code_t->Transparent = false;
    Code_u = new Graphics::TBitmap;
    Code_u->LoadFromResourceName(0, "Code_u");
    Code_u->Transparent = false;
    Code_v = new Graphics::TBitmap;
    Code_v->LoadFromResourceName(0, "Code_v");
    Code_v->Transparent = false;
    Code_w = new Graphics::TBitmap;
    Code_w->LoadFromResourceName(0, "Code_w");
    Code_w->Transparent = false;
    Code_x = new Graphics::TBitmap;
    Code_x->LoadFromResourceName(0, "Code_x");
    Code_x->Transparent = false;
    Code_y = new Graphics::TBitmap;
    Code_y->LoadFromResourceName(0, "Code_y");
    Code_y->Transparent = false;
    Code_z = new Graphics::TBitmap;
    Code_z->LoadFromResourceName(0, "Code_z");
    Code_z->Transparent = false;
    Code0 = new Graphics::TBitmap;
    Code0->LoadFromResourceName(0, "Code0");
    Code0->Transparent = false;
    Code1 = new Graphics::TBitmap;
    Code1->LoadFromResourceName(0, "Code1");
    Code1->Transparent = false;
    Code2 = new Graphics::TBitmap;
    Code2->LoadFromResourceName(0, "Code2");
    Code2->Transparent = false;
    Code3 = new Graphics::TBitmap;
    Code3->LoadFromResourceName(0, "Code3");
    Code3->Transparent = false;
    Code4 = new Graphics::TBitmap;
    Code4->LoadFromResourceName(0, "Code4");
    Code4->Transparent = false;
    Code5 = new Graphics::TBitmap;
    Code5->LoadFromResourceName(0, "Code5");
    Code5->Transparent = false;
    Code6 = new Graphics::TBitmap;
    Code6->LoadFromResourceName(0, "Code6");
    Code6->Transparent = false;
    Code7 = new Graphics::TBitmap;
    Code7->LoadFromResourceName(0, "Code7");
    Code7->Transparent = false;
    Code8 = new Graphics::TBitmap;
    Code8->LoadFromResourceName(0, "Code8");
    Code8->Transparent = false;
    Code9 = new Graphics::TBitmap;
    Code9->LoadFromResourceName(0, "Code9");
    Code9->Transparent = false;
    CodeA = new Graphics::TBitmap;
    CodeA->LoadFromResourceName(0, "CodeA");
    CodeA->Transparent = false;
    CodeB = new Graphics::TBitmap;
    CodeB->LoadFromResourceName(0, "CodeB");
    CodeB->Transparent = false;
    CodeC = new Graphics::TBitmap;
    CodeC->LoadFromResourceName(0, "CodeC");
    CodeC->Transparent = false;
    CodeD = new Graphics::TBitmap;
    CodeD->LoadFromResourceName(0, "CodeD");
    CodeD->Transparent = false;
    CodeE = new Graphics::TBitmap;
    CodeE->LoadFromResourceName(0, "CodeE");
    CodeE->Transparent = false;
    CodeF = new Graphics::TBitmap;
    CodeF->LoadFromResourceName(0, "CodeF");
    CodeF->Transparent = false;
    CodeG = new Graphics::TBitmap;
    CodeG->LoadFromResourceName(0, "CodeG");
    CodeG->Transparent = false;
    CodeH = new Graphics::TBitmap;
    CodeH->LoadFromResourceName(0, "CodeH");
    CodeH->Transparent = false;
    CodeI = new Graphics::TBitmap;
    CodeI->LoadFromResourceName(0, "CodeI");
    CodeI->Transparent = false;
    CodeJ = new Graphics::TBitmap;
    CodeJ->LoadFromResourceName(0, "CodeJ");
    CodeJ->Transparent = false;
    CodeK = new Graphics::TBitmap;
    CodeK->LoadFromResourceName(0, "CodeK");
    CodeK->Transparent = false;
    CodeL = new Graphics::TBitmap;
    CodeL->LoadFromResourceName(0, "CodeL");
    CodeL->Transparent = false;
    CodeM = new Graphics::TBitmap;
    CodeM->LoadFromResourceName(0, "CodeM");
    CodeM->Transparent = false;
    CodeN = new Graphics::TBitmap;
    CodeN->LoadFromResourceName(0, "CodeN");
    CodeN->Transparent = false;
    CodeO = new Graphics::TBitmap;
    CodeO->LoadFromResourceName(0, "CodeO");
    CodeO->Transparent = false;
    CodeP = new Graphics::TBitmap;
    CodeP->LoadFromResourceName(0, "CodeP");
    CodeP->Transparent = false;
    CodeQ = new Graphics::TBitmap;
    CodeQ->LoadFromResourceName(0, "CodeQ");
    CodeQ->Transparent = false;
    CodeR = new Graphics::TBitmap;
    CodeR->LoadFromResourceName(0, "CodeR");
    CodeR->Transparent = false;
    CodeS = new Graphics::TBitmap;
    CodeS->LoadFromResourceName(0, "CodeS");
    CodeS->Transparent = false;
    CodeT = new Graphics::TBitmap;
    CodeT->LoadFromResourceName(0, "CodeT");
    CodeT->Transparent = false;
    CodeU = new Graphics::TBitmap;
    CodeU->LoadFromResourceName(0, "CodeU");
    CodeU->Transparent = false;
    CodeV = new Graphics::TBitmap;
    CodeV->LoadFromResourceName(0, "CodeV");
    CodeV->Transparent = false;
    CodeW = new Graphics::TBitmap;
    CodeW->LoadFromResourceName(0, "CodeW");
    CodeW->Transparent = false;
    CodeX = new Graphics::TBitmap;
    CodeX->LoadFromResourceName(0, "CodeX");
    CodeX->Transparent = false;
    CodeY = new Graphics::TBitmap;
    CodeY->LoadFromResourceName(0, "CodeY");
    CodeY->Transparent = false;
    CodeZ = new Graphics::TBitmap;
    CodeZ->LoadFromResourceName(0, "CodeZ");
    CodeZ->Transparent = false;
    bmSolidBgnd = new Graphics::TBitmap;
    bmSolidBgnd->LoadFromResourceName(0, "bmSolidBgnd");
    bmSolidBgnd->Transparent = false;
    smSolidBgnd = new Graphics::TBitmap;
    smSolidBgnd->LoadFromResourceName(0, "smSolidBgnd");
    smSolidBgnd->Transparent = false;
    bmDiagonalSignalBlank = new Graphics::TBitmap;
    bmDiagonalSignalBlank->LoadFromResourceName(0, "bmDiagonalSignalBlank");
    bmDiagonalSignalBlank->Transparent = false;
    bmPointBlank = new Graphics::TBitmap;
    bmPointBlank->LoadFromResourceName(0, "bmPointBlank");
    bmPointBlank->Transparent = false;
    bmStraightEWSignalBlank = new Graphics::TBitmap;
    bmStraightEWSignalBlank->LoadFromResourceName(0, "bmStraightEWSignalBlank");
    bmStraightEWSignalBlank->Transparent = false;
    bmStraightNSSignalBlank = new Graphics::TBitmap;
    bmStraightNSSignalBlank->LoadFromResourceName(0, "bmStraightNSSignalBlank");
    bmStraightNSSignalBlank->Transparent = false;

// These are the new glyphs for v2.3.0 that stay black, they are transparent, using the bottom LH corner pixel as the transparent colour
    SpeedBut68NormBlackGlyph = new Graphics::TBitmap;
    SpeedBut68NormBlackGlyph->Assign(gl68); // changed at v2.3.1 from 'LoadFromResourceName' for consistency
    SpeedBut69NormBlackGlyph = new Graphics::TBitmap;
    SpeedBut69NormBlackGlyph->Assign(gl69);
    SpeedBut70NormBlackGlyph = new Graphics::TBitmap;
    SpeedBut70NormBlackGlyph->Assign(gl70);
    SpeedBut71NormBlackGlyph = new Graphics::TBitmap;
    SpeedBut71NormBlackGlyph->Assign(gl71);
    SpeedBut72NormBlackGlyph = new Graphics::TBitmap;
    SpeedBut72NormBlackGlyph->Assign(gl72);
    SpeedBut73NormBlackGlyph = new Graphics::TBitmap;
    SpeedBut73NormBlackGlyph->Assign(gl73);
    SpeedBut74NormBlackGlyph = new Graphics::TBitmap;
    SpeedBut74NormBlackGlyph->Assign(gl74);
    SpeedBut75NormBlackGlyph = new Graphics::TBitmap;
    SpeedBut75NormBlackGlyph->Assign(gl75);

    SpeedBut68GrndBlackGlyph = new Graphics::TBitmap;
    SpeedBut68GrndBlackGlyph->Assign(bm68grounddblred);
    SpeedBut69GrndBlackGlyph = new Graphics::TBitmap;
    SpeedBut69GrndBlackGlyph->Assign(bm69grounddblred);
    SpeedBut70GrndBlackGlyph = new Graphics::TBitmap;
    SpeedBut70GrndBlackGlyph->Assign(bm70grounddblred);
    SpeedBut71GrndBlackGlyph = new Graphics::TBitmap;
    SpeedBut71GrndBlackGlyph->Assign(bm71grounddblred);
    SpeedBut72GrndBlackGlyph = new Graphics::TBitmap;
    SpeedBut72GrndBlackGlyph->Assign(bm72grounddblred);
    SpeedBut73GrndBlackGlyph = new Graphics::TBitmap;
    SpeedBut73GrndBlackGlyph->Assign(gl73grounddblred); // these have to use 'gl' graphics as bot LH corner = transparent
    SpeedBut74GrndBlackGlyph = new Graphics::TBitmap;
    SpeedBut74GrndBlackGlyph->Assign(gl74grounddblred); // colour, & 'bm' graphics have black at that position
    SpeedBut75GrndBlackGlyph = new Graphics::TBitmap;
    SpeedBut75GrndBlackGlyph->Assign(bm75grounddblred);

    // GridBitmap is a 10 x 9 grid image, quicker to plot these for whole screen than small ones
    GridBitmap = new Graphics::TBitmap;
    GridBitmap->PixelFormat = pf8bit;
    GridBitmap->Width = 160;
    GridBitmap->Height = 144;
    TRect Source(0, 0, 16, 16);

    for(int x = 0; x < 10; x++)
    {
        for(int y = 0; y < 9; y++)
        {
            TRect Dest(x * 16, y * 16, (x * 16) + 16, (y * 16) + 16);
            GridBitmap->Canvas->CopyRect(Dest, bmGrid->Canvas, Source);
        }
    }
    GridBitmap->Transparent = true;
    GridBitmap->TransparentColor = clB5G5R5;

    for(int x = 0; x < 30; x++)
    {
        LinkPrefDirGraphicsPtr[x] = new Graphics::TBitmap;
        LinkPrefDirGraphicsPtr[x]->PixelFormat = pf8bit;
        LinkNonSigRouteGraphicsPtr[x] = new Graphics::TBitmap;
        LinkNonSigRouteGraphicsPtr[x]->PixelFormat = pf8bit;
        LinkSigRouteGraphicsPtr[x] = new Graphics::TBitmap;
        LinkSigRouteGraphicsPtr[x]->PixelFormat = pf8bit;
        LinkRouteAutoSigsGraphicsPtr[x] = new Graphics::TBitmap;
        LinkRouteAutoSigsGraphicsPtr[x]->PixelFormat = pf8bit;
        LinkPrefDirGraphicsPtr[x]->Width = 16;
        LinkPrefDirGraphicsPtr[x]->Height = 16;
        LinkNonSigRouteGraphicsPtr[x]->Width = 16;
        LinkNonSigRouteGraphicsPtr[x]->Height = 16;
        LinkSigRouteGraphicsPtr[x]->Width = 16;
        LinkSigRouteGraphicsPtr[x]->Height = 16;
        LinkRouteAutoSigsGraphicsPtr[x]->Width = 16;
        LinkRouteAutoSigsGraphicsPtr[x]->Height = 16;
        LinkPrefDirGraphicsPtr[x]->Transparent = true;
        LinkNonSigRouteGraphicsPtr[x]->Transparent = true;
        LinkSigRouteGraphicsPtr[x]->Transparent = true;
        LinkRouteAutoSigsGraphicsPtr[x]->Transparent = true;
    }
    for(int x = 0; x < 12; x++)
    {
        BridgePrefDirGraphicsPtr[x] = new Graphics::TBitmap;
        BridgePrefDirGraphicsPtr[x]->PixelFormat = pf8bit;
        BridgeNonSigRouteGraphicsPtr[x] = new Graphics::TBitmap;
        BridgeNonSigRouteGraphicsPtr[x]->PixelFormat = pf8bit;
        BridgeSigRouteGraphicsPtr[x] = new Graphics::TBitmap;
        BridgeSigRouteGraphicsPtr[x]->PixelFormat = pf8bit;
        BridgeRouteAutoSigsGraphicsPtr[x] = new Graphics::TBitmap;
        BridgeRouteAutoSigsGraphicsPtr[x]->PixelFormat = pf8bit;
        BridgePrefDirGraphicsPtr[x]->Width = 16;
        BridgePrefDirGraphicsPtr[x]->Height = 16;
        BridgeNonSigRouteGraphicsPtr[x]->Width = 16;
        BridgeNonSigRouteGraphicsPtr[x]->Height = 16;
        BridgeSigRouteGraphicsPtr[x]->Width = 16;
        BridgeSigRouteGraphicsPtr[x]->Height = 16;
        BridgeRouteAutoSigsGraphicsPtr[x]->Width = 16;
        BridgeRouteAutoSigsGraphicsPtr[x]->Height = 16;
        BridgePrefDirGraphicsPtr[x]->Transparent = true;
        BridgeNonSigRouteGraphicsPtr[x]->Transparent = true;
        BridgeSigRouteGraphicsPtr[x]->Transparent = true;
        BridgeRouteAutoSigsGraphicsPtr[x]->Transparent = true;
    }

    for(int x = 0; x < 10; x++)
    {
        DirectionPrefDirGraphicsPtr[x] = new Graphics::TBitmap;
        DirectionPrefDirGraphicsPtr[x]->PixelFormat = pf8bit;
        DirectionNonSigRouteGraphicsPtr[x] = new Graphics::TBitmap;
        DirectionNonSigRouteGraphicsPtr[x]->PixelFormat = pf8bit;
        DirectionSigRouteGraphicsPtr[x] = new Graphics::TBitmap;
        DirectionSigRouteGraphicsPtr[x]->PixelFormat = pf8bit;
        DirectionRouteAutoSigsGraphicsPtr[x] = new Graphics::TBitmap;
        DirectionRouteAutoSigsGraphicsPtr[x]->PixelFormat = pf8bit;
        DirectionPrefDirGraphicsPtr[x]->Width = 16;
        DirectionPrefDirGraphicsPtr[x]->Height = 16;
        DirectionNonSigRouteGraphicsPtr[x]->Width = 16;
        DirectionNonSigRouteGraphicsPtr[x]->Height = 16;
        DirectionSigRouteGraphicsPtr[x]->Width = 16;
        DirectionSigRouteGraphicsPtr[x]->Height = 16;
        DirectionRouteAutoSigsGraphicsPtr[x]->Width = 16;
        DirectionRouteAutoSigsGraphicsPtr[x]->Height = 16;
        DirectionPrefDirGraphicsPtr[x]->Transparent = true;
        DirectionNonSigRouteGraphicsPtr[x]->Transparent = true;
        DirectionSigRouteGraphicsPtr[x]->Transparent = true;
        DirectionRouteAutoSigsGraphicsPtr[x]->Transparent = true;
    }

    // points' fillets
    for(int x = 0; x < 32; x++)
    {
        PointModeGraphicsPtr[x][0] = new Graphics::TBitmap;
        PointModeGraphicsPtr[x][0]->PixelFormat = pf8bit;
        PointModeGraphicsPtr[x][1] = new Graphics::TBitmap;
        PointModeGraphicsPtr[x][1]->PixelFormat = pf8bit;
        PointModeGraphicsPtr[x][0]->Width = 16;
        PointModeGraphicsPtr[x][0]->Height = 16;
        PointModeGraphicsPtr[x][1]->Width = 16;
        PointModeGraphicsPtr[x][1]->Height = 16;
        PointModeGraphicsPtr[x][0]->Transparent = true;
        PointModeGraphicsPtr[x][1]->Transparent = true;
    }

    // Note: LockedRouteCancelPtr[0] & [5] unused
    // The following are just pointer copies, not new graphics
    LockedRouteCancelPtr[0] = bmRtCancELink1;
    LockedRouteCancelPtr[1] = bmRtCancELink1;
    LockedRouteCancelPtr[2] = bmRtCancELink2;
    LockedRouteCancelPtr[3] = bmRtCancELink3;
    LockedRouteCancelPtr[4] = bmRtCancELink4;
    LockedRouteCancelPtr[5] = bmRtCancELink4;
    LockedRouteCancelPtr[6] = bmRtCancELink6;
    LockedRouteCancelPtr[7] = bmRtCancELink7;
    LockedRouteCancelPtr[8] = bmRtCancELink8;
    LockedRouteCancelPtr[9] = bmRtCancELink9;

    // LinkGraphicsPtr & BridgeGraphicsPtr graphics don't need to be created as already exist in correct colours
    LinkGraphicsPtr[0] = gl1;
    LinkGraphicsPtr[1] = gl2;
    LinkGraphicsPtr[2] = gl6;
    LinkGraphicsPtr[3] = gl5;
    LinkGraphicsPtr[4] = gl3;
    LinkGraphicsPtr[5] = gl4;
    LinkGraphicsPtr[6] = gl22;
    LinkGraphicsPtr[7] = gl24;
    LinkGraphicsPtr[8] = gl21;
    LinkGraphicsPtr[9] = bm27;
    LinkGraphicsPtr[10] = gl25;
    LinkGraphicsPtr[11] = gl23;
    LinkGraphicsPtr[12] = gl26;
    LinkGraphicsPtr[13] = bm20;
    LinkGraphicsPtr[14] = gl19;
    LinkGraphicsPtr[15] = bm18;
    LinkGraphicsPtr[16] = gl64;
    LinkGraphicsPtr[17] = bm65;
    LinkGraphicsPtr[18] = gl66;
    LinkGraphicsPtr[19] = gl67;
    LinkGraphicsPtr[20] = gl80;
    LinkGraphicsPtr[21] = gl81;
    LinkGraphicsPtr[22] = gl82;
    LinkGraphicsPtr[23] = gl83;
    LinkGraphicsPtr[24] = gl84;
    LinkGraphicsPtr[25] = bm85;
    LinkGraphicsPtr[26] = gl86;
    LinkGraphicsPtr[27] = gl87;
    LinkGraphicsPtr[28] = UnderHFootbridge;
    LinkGraphicsPtr[29] = UnderVFootbridge;

    BridgeGraphicsPtr[0] = br1;
    BridgeGraphicsPtr[1] = br2;
    BridgeGraphicsPtr[2] = br3;
    BridgeGraphicsPtr[3] = br4;
    BridgeGraphicsPtr[4] = br5;
    BridgeGraphicsPtr[5] = br9;
    BridgeGraphicsPtr[6] = br10;
    BridgeGraphicsPtr[7] = br6;
    BridgeGraphicsPtr[8] = br7;
    BridgeGraphicsPtr[9] = br12;
    BridgeGraphicsPtr[10] = br8;
    BridgeGraphicsPtr[11] = br11;
}
// ---------------------------------------------------------------------------

TRailGraphics::~TRailGraphics()
{
    for(int x = 0; x < 30; x++)
    {
        delete LinkPrefDirGraphicsPtr[x];
    }
    for(int x = 0; x < 30; x++)
    {
        delete LinkNonSigRouteGraphicsPtr[x];
    }
    for(int x = 0; x < 30; x++)
    {
        delete LinkSigRouteGraphicsPtr[x];
    }
    for(int x = 0; x < 30; x++)
    {
        delete LinkRouteAutoSigsGraphicsPtr[x];
    }
    for(int x = 0; x < 12; x++)
    {
        delete BridgePrefDirGraphicsPtr[x];
    }
    for(int x = 0; x < 12; x++)
    {
        delete BridgeNonSigRouteGraphicsPtr[x];
    }
    for(int x = 0; x < 12; x++)
    {
        delete BridgeSigRouteGraphicsPtr[x];
    }
    for(int x = 0; x < 12; x++)
    {
        delete BridgeRouteAutoSigsGraphicsPtr[x];
    }
    for(int x = 0; x < 10; x++)
    {
        delete DirectionPrefDirGraphicsPtr[x];
    }
    for(int x = 0; x < 10; x++)
    {
        delete DirectionNonSigRouteGraphicsPtr[x];
    }
    for(int x = 0; x < 10; x++)
    {
        delete DirectionSigRouteGraphicsPtr[x];
    }
    for(int x = 0; x < 10; x++)
    {
        delete DirectionRouteAutoSigsGraphicsPtr[x];
    }
    for(int x = 0; x < 32; x++)
    {
        delete PointModeGraphicsPtr[x][0];
        delete PointModeGraphicsPtr[x][1];
    }

    delete bm10;
    delete bm100;
    delete bm101;
    delete bm106;
    delete bm10Diverging;
    delete bm10Straight;
    delete bm11;
    delete bm11Diverging;
    delete bm11Straight;
    delete bm12;
    delete bm12Diverging;
    delete bm12Straight;
    delete bm13;
    delete bm132;
    delete bm132LeftFork;
    delete bm132RightFork;
    delete bm133;
    delete bm133LeftFork;
    delete bm133RightFork;
    delete bm134;
    delete bm134LeftFork;
    delete bm134RightFork;
    delete bm135;
    delete bm135LeftFork;
    delete bm135RightFork;
    delete bm136;
    delete bm136LeftFork;
    delete bm136RightFork;
    delete bm137;
    delete bm137LeftFork;
    delete bm137RightFork;
    delete bm138;
    delete bm138LeftFork;
    delete bm138RightFork;
    delete bm139;
    delete bm139LeftFork;
    delete bm139RightFork;
    delete bm13Diverging;
    delete bm13Straight;
    delete bm14;
    delete bm140;
    delete bm141;
    delete bm14Diverging;
    delete bm14Straight;
    delete bm16;
    delete bm18;
    delete bm20;
    delete bm27;
    delete bm28;
    delete bm28Diverging;
    delete bm28Straight;
    delete bm29;
    delete bm29Diverging;
    delete bm29Straight;
    delete bm30;
    delete bm30Diverging;
    delete bm30Straight;
    delete bm31;
    delete bm31Diverging;
    delete bm31Straight;
    delete bm32;
    delete bm32Diverging;
    delete bm32Straight;
    delete bm33;
    delete bm33Diverging;
    delete bm33Straight;
    delete bm34;
    delete bm34Diverging;
    delete bm34Straight;
    delete bm35;
    delete bm35Diverging;
    delete bm35Straight;
    delete bm36;
    delete bm36Diverging;
    delete bm36Straight;
    delete bm37;
    delete bm37Diverging;
    delete bm37Straight;
    delete bm38;
    delete bm38Diverging;
    delete bm38Straight;
    delete bm39;
    delete bm39Diverging;
    delete bm39Straight;
    delete bm40;
    delete bm40Diverging;
    delete bm40Straight;
    delete bm41;
    delete bm41Diverging;
    delete bm41Straight;
    delete bm42;
    delete bm42Diverging;
    delete bm42Straight;
    delete bm43;
    delete bm43Diverging;
    delete bm43Straight;
    delete bm45;
    delete bm46;
    delete bm50;
    delete bm51;
    delete bm53;
    delete bm54;
    delete bm56;
    delete bm59;
    delete bm65;
    delete bm68CallingOn;
    delete bm68dblyellow;
    delete bm68grounddblred; // added at v2.3.1 (missed)
    delete bm68grounddblwhite; // added at v2.3.1 (missed)
    delete bm68green;
    delete bm68yellow;
    delete bm69CallingOn;
    delete bm69dblyellow;
    delete bm69grounddblred; // added at v2.3.1 (missed)
    delete bm69grounddblwhite; // added at v2.3.1 (missed)
    delete bm69green;
    delete bm69yellow;
    delete bm7;
    delete bm70CallingOn;
    delete bm70dblyellow;
    delete bm70grounddblred; // added at v2.3.1 (missed)
    delete bm70grounddblwhite; // added at v2.3.1 (missed)
    delete bm70green;
    delete bm70yellow;
    delete bm71CallingOn;
    delete bm71dblyellow;
    delete bm71grounddblred; // added at v2.3.1 (missed)
    delete bm71grounddblwhite; // added at v2.3.1 (missed)
    delete bm71green;
    delete bm71yellow;
    delete bm72CallingOn;
    delete bm72dblyellow;
    delete bm72grounddblred; // added at v2.3.1 (missed)
    delete bm72grounddblwhite; // added at v2.3.1 (missed)
    delete bm72green;
    delete bm72yellow;
    delete bm73;
    delete bm73CallingOn;
    delete bm73dblyellow;
    delete bm73grounddblred; // added at v2.3.1 (missed)
    delete bm73grounddblwhite; // added at v2.3.1 (missed)
    delete bm73green;
    delete bm73yellow;
    delete bm74;
    delete bm74CallingOn;
    delete bm74dblyellow;
    delete bm74grounddblred; // added at v2.3.1 (missed)
    delete bm74grounddblwhite; // added at v2.3.1 (missed)
    delete bm74green;
    delete bm74yellow;
    delete bm75CallingOn;
    delete bm75dblyellow;
    delete bm75grounddblred; // added at v2.3.1 (missed)
    delete bm75grounddblwhite; // added at v2.3.1 (missed)
    delete bm75green;
    delete bm75yellow;
    delete bm77;
    delete bm77Striped;
    delete bm78;
    delete bm78Striped;
    delete bm7Diverging;
    delete bm7Straight;
    delete bm8;
    delete bm85;
    delete bm8Diverging;
    delete bm8Straight;
    delete bm9;
    delete bm93set;
    delete bm93unset;
    delete bm94set;
    delete bm94unset;
    delete bm9Diverging;
    delete bm9Straight;
    delete bmDiagonalSignalBlank;
    delete bmGreenEllipse;
    delete bmGreenRect;
    delete bmGrid;
    delete bmLightBlueRect;
    delete bmName;
    delete bmNameStriped;
    delete bmPointBlank;
    delete bmRedEllipse;
    delete bmRedRect;
    delete bmRtCancELink1;
    delete bmRtCancELink2;
    delete bmRtCancELink3;
    delete bmRtCancELink4;
    delete bmRtCancELink6;
    delete bmRtCancELink7;
    delete bmRtCancELink8;
    delete bmRtCancELink9;
    delete bmStraightEWSignalBlank;
    delete bmStraightNSSignalBlank;
    delete bmSolidBgnd;
    delete br1;
    delete br10;
    delete br11;
    delete br12;
    delete br2;
    delete br3;
    delete br4;
    delete br5;
    delete br6;
    delete br7;
    delete br8;
    delete br9;
    delete Code_a;
    delete Code_b;
    delete Code_c;
    delete Code_d;
    delete Code_e;
    delete Code_f;
    delete Code_g;
    delete Code_h;
    delete Code_i;
    delete Code_j;
    delete Code_k;
    delete Code_l;
    delete Code_m;
    delete Code_n;
    delete Code_o;
    delete Code_p;
    delete Code_q;
    delete Code_r;
    delete Code_s;
    delete Code_t;
    delete Code_u;
    delete Code_v;
    delete Code_w;
    delete Code_x;
    delete Code_y;
    delete Code_z;
    delete Code0;
    delete Code1;
    delete Code2;
    delete Code3;
    delete Code4;
    delete Code5;
    delete Code6;
    delete Code7;
    delete Code8;
    delete Code9;
    delete CodeA;
    delete CodeB;
    delete CodeC;
    delete CodeD;
    delete CodeE;
    delete CodeF;
    delete CodeG;
    delete CodeH;
    delete CodeI;
    delete CodeJ;
    delete CodeK;
    delete CodeL;
    delete CodeM;
    delete CodeN;
    delete CodeO;
    delete CodeP;
    delete CodeQ;
    delete CodeR;
    delete CodeS;
    delete CodeT;
    delete CodeU;
    delete CodeV;
    delete CodeW;
    delete CodeX;
    delete CodeY;
    delete CodeZ;
    delete Concourse;
    delete ConcourseGlyph;
    delete ConcourseStriped;
    delete ELk1;
    delete ELk2;
    delete ELk3;
    delete ELk4;
    delete ELk6;
    delete ELk7;
    delete ELk8;
    delete ELk9;
    delete gl1;
    delete gl10;
    delete gl100;
    delete gl101;
    delete gl102;
    delete gl103;
    delete gl104;
    delete gl105;
    delete gl106;
    delete gl107;
    delete gl108;
    delete gl109;
    delete gl11;
    delete gl110;
    delete gl111;
    delete gl112;
    delete gl113;
    delete gl114;
    delete gl115;
    delete gl116;
    delete gl117;
    delete gl118;
    delete gl119;
    delete gl12;
    delete gl120;
    delete gl121;
    delete gl122;
    delete gl123;
    delete gl124;
    delete gl125;
    delete gl126;
    delete gl127;
    delete gl128;
    delete gl129;
    delete gl129Striped;
    delete gl13;
    delete gl130;
    delete gl130Striped;
    delete gl131;
    delete gl132;
    delete gl133;
    delete gl134;
    delete gl135;
    delete gl136;
    delete gl137;
    delete gl138;
    delete gl139;
    delete gl14;
    delete gl140;
    delete gl141;
    delete gl142;
    delete gl143;
    delete gl145;
    delete gl145Striped;
    delete gl146;
    delete gl146Striped;
    delete gl15;
    delete gl16;
    delete gl18;
    delete gl19;
    delete gl2;
    delete gl20;
    delete gl21;
    delete gl22;
    delete gl23;
    delete gl24;
    delete gl25;
    delete gl26;
    delete gl27;
    delete gl28;
    delete gl29;
    delete gl3;
    delete gl30;
    delete gl31;
    delete gl32;
    delete gl33;
    delete gl34;
    delete gl35;
    delete gl36;
    delete gl37;
    delete gl38;
    delete gl39;
    delete gl4;
    delete gl40;
    delete gl41;
    delete gl42;
    delete gl43;
    delete gl44;
    delete gl45;
    delete gl46;
    delete gl47;
    delete gl48;
    delete gl49;
    delete gl5;
    delete gl50;
    delete gl51;
    delete gl52;
    delete gl53;
    delete gl54;
    delete gl55;
    delete gl56;
    delete gl57;
    delete gl58;
    delete gl59;
    delete gl6;
    delete gl60;
    delete gl61;
    delete gl62;
    delete gl63;
    delete gl64;
    delete gl65;
    delete gl66;
    delete gl67;
    delete gl68;
    delete gl69;
    delete gl7;
    delete gl70;
    delete gl71;
    delete gl72;
    delete gl73;
    delete gl73grounddblred;
    delete gl74;
    delete gl74grounddblred;
    delete gl75;
    delete gl76;
    delete gl76Striped;
    delete gl77;
    delete gl78;
    delete gl79;
    delete gl79Striped;
    delete gl8;
    delete gl80;
    delete gl81;
    delete gl82;
    delete gl83;
    delete gl84;
    delete gl85;
    delete gl86;
    delete gl87;
    delete gl88set;
    delete gl88unset;
    delete gl89set;
    delete gl89unset;
    delete gl9;
    delete gl90set;
    delete gl90unset;
    delete gl91set;
    delete gl91unset;
    delete gl92set;
    delete gl92unset;
    delete gl93set;
    delete gl94set;
    delete gl95set;
    delete gl95unset;
    delete gl97;
    delete gl98;
    delete gl99;
    delete Plat68;
    delete Plat68Striped;
    delete Plat69;
    delete Plat69Striped;
    delete Plat70;
    delete Plat70Striped;
    delete Plat71;
    delete Plat71Striped;
    delete sm1;
    delete sm10;
    delete sm100;
    delete sm101;
    delete sm102;
    delete sm103;
    delete sm104;
    delete sm105;
    delete sm106;
    delete sm107;
    delete sm108;
    delete sm109;
    delete sm11;
    delete sm110;
    delete sm111;
    delete sm112;
    delete sm115;
    delete sm117;
    delete sm12;
    delete sm129;
    delete sm129striped;
    delete sm13;
    delete sm130;
    delete sm130striped;
    delete sm131striped;
    delete sm132;
    delete sm133;
    delete sm134;
    delete sm135;
    delete sm136;
    delete sm137;
    delete sm138;
    delete sm139;
    delete sm14;
    delete sm15;
    delete sm16;
    delete sm18;
    delete sm19;
    delete sm2;
    delete sm20;
    delete sm21;
    delete sm22;
    delete sm23;
    delete sm24;
    delete sm25;
    delete sm26;
    delete sm27;
    delete sm28;
    delete sm29;
    delete sm3;
    delete sm30;
    delete sm31;
    delete sm32;
    delete sm33;
    delete sm34;
    delete sm35;
    delete sm36;
    delete sm37;
    delete sm38;
    delete sm39;
    delete sm4;
    delete sm40;
    delete sm41;
    delete sm42;
    delete sm43;
    delete sm44;
    delete sm45;
    delete sm46;
    delete sm47;
    delete sm48;
    delete sm49;
    delete sm5;
    delete sm50;
    delete sm51;
    delete sm52;
    delete sm53;
    delete sm54;
    delete sm55;
    delete sm56;
    delete sm57;
    delete sm58;
    delete sm59;
    delete sm6;
    delete sm60;
    delete sm61;
    delete sm62;
    delete sm63;
    delete sm64;
    delete sm65;
    delete sm66;
    delete sm67;
    delete sm7;
    delete sm76;
    delete sm76striped;
    delete sm77;
    delete sm77striped;
    delete sm78;
    delete sm78striped;
    delete sm79;
    delete sm79striped;
    delete sm8;
    delete sm80;
    delete sm81;
    delete sm82;
    delete sm83;
    delete sm84;
    delete sm85;
    delete sm86;
    delete sm87;
    delete sm88;
    delete sm89;
    delete sm9;
    delete sm90;
    delete sm91;
    delete sm92;
    delete sm93;
    delete sm94;
    delete sm95;
    delete sm96;
    delete sm96striped;
    delete sm97;
    delete sm98;
    delete sm99;
    delete smBlack;
    delete smBlue;
    delete smBrightGreen;
    delete smCaramel;
    delete smCyan;
    delete smLC;
    delete smLightBlue;
    delete smMagenta;
    delete smName;
    delete smPaleGreen;
    delete smRed;
    delete smSolidBgnd;
    delete smYellow;
    delete smTransparent;
    delete TempBackground;
    delete TempHeadCode;
    delete UnderHFootbridge;
    delete UnderVFootbridge;

    delete LCBothHor;
    delete LCBotHor;
    delete LCBothVer;
    delete LCLHSVer;
    delete LCPlain;
    delete LCRHSVer;
    delete LCTopHor;
    delete LCBothHorMan;
    delete LCBotHorMan;
    delete LCBothVerMan;
    delete LCLHSVerMan;
    delete LCPlainMan;
    delete LCRHSVerMan;
    delete LCTopHorMan;

    delete SpeedBut68NormBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut69NormBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut70NormBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut71NormBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut72NormBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut73NormBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut74NormBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut75NormBlackGlyph; // added at v2.3.1 (missed)

    delete SpeedBut68GrndBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut69GrndBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut70GrndBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut71GrndBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut72GrndBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut73GrndBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut74GrndBlackGlyph; // added at v2.3.1 (missed)
    delete SpeedBut75GrndBlackGlyph; // added at v2.3.1 (missed)

    delete bmTransparentBgnd;
    delete GridBitmap;
}

// ---------------------------------------------------------------------------
void TRailGraphics::ChangeTransparentColour(Graphics::TBitmap *BitmapIn, Graphics::TBitmap *BitmapOut, TColor NewTransparentColour, TColor OldTransparentColour)
{
    // Change OldTransparentColour to NewTransparentColour, if NewTransparentColour not white change black to white
    // else change white to black
    if(NewTransparentColour == OldTransparentColour)
    {
        return; // already stored

    }
    Utilities->CallLog.push_back(Utilities->TimeStamp() + ",ChangeTransparentColour," + AnsiString(NewTransparentColour) + "," +
                                 AnsiString(OldTransparentColour));
    BitmapOut->Height = BitmapIn->Height;
    BitmapOut->Width = BitmapIn->Width;
    for(int x = 0; x < BitmapIn->Width; x++)
    {
        for(int y = 0; y < BitmapIn->Height; y++)
        {
            if(NewTransparentColour != clB5G5R5) // new is dark, old could be dark or light
            {
                if(BitmapIn->Canvas->Pixels[x][y] == OldTransparentColour)
                {
                    BitmapOut->Canvas->Pixels[x][y] = NewTransparentColour;
                }
                else if(BitmapIn->Canvas->Pixels[x][y] == clB0G0R0)
                // black - if OldTransparentColour was black (or any dark colour) then track will be white already
                {
                    BitmapOut->Canvas->Pixels[x][y] = clB5G5R5; // white
                }
                else
                {
                    BitmapOut->Canvas->Pixels[x][y] = BitmapIn->Canvas->Pixels[x][y];
                }
            }
            else
            {
                // new is white, old must be dark else new = old & wouldn't reach here
                if(BitmapIn->Canvas->Pixels[x][y] == OldTransparentColour)
                {
                    BitmapOut->Canvas->Pixels[x][y] = NewTransparentColour;
                }
                else if(BitmapIn->Canvas->Pixels[x][y] == clB5G5R5) // white
                {
                    BitmapOut->Canvas->Pixels[x][y] = clB0G0R0; // black
                }
                else
                {
                    BitmapOut->Canvas->Pixels[x][y] = BitmapIn->Canvas->Pixels[x][y];
                }
            }
        }
    }
    BitmapOut->TransparentColor = NewTransparentColour;
    Utilities->CallLogPop(1795);
}

// ---------------------------------------------------------------------------

void TRailGraphics::ChangeForegroundColour(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap *BitmapOut, TColor NewForegroundColour,
                                           TColor BackgroundColour)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ChangeForegroundColour," + AnsiString(NewForegroundColour) + "," +
                                 AnsiString(BackgroundColour));
    BitmapOut->Height = BitmapIn->Height;
    BitmapOut->Width = BitmapIn->Width;
    for(int x = 0; x < BitmapIn->Width; x++)
    {
        for(int y = 0; y < BitmapIn->Height; y++)
        {
            if(BitmapIn->Canvas->Pixels[x][y] != BackgroundColour)
            {
                BitmapOut->Canvas->Pixels[x][y] = NewForegroundColour;
            }
            else
            {
                BitmapOut->Canvas->Pixels[x][y] = BackgroundColour;
            }
        }
    }
    Utilities->CallLogPop(1480);
}

// ---------------------------------------------------------------------------

void TRailGraphics::ChangeForegroundColour2(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap *BitmapOut, TColor NewForegroundColour,
                                            TColor BackgroundColour)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ChangeForegroundColour2," + AnsiString(NewForegroundColour) + "," +
                                 AnsiString(BackgroundColour));
    Graphics::TBitmap *TempBitmapOut = new Graphics::TBitmap;

    TempBitmapOut->Assign(BitmapIn); // in case BitmapOut isn't fully defined at this stage
    TRect FullRect(0, 0, BitmapIn->Width, BitmapIn->Height); // the full size of both bitmaps

    TempBitmapOut->Canvas->Brush->Color = BackgroundColour; // BrushStyle default is solid so leave at that
    TempBitmapOut->Canvas->FillRect(FullRect);
    for(int x = 0; x < BitmapIn->Width; x++)
    {
        for(int y = 0; y < BitmapIn->Height; y++)
        {
            if(BitmapIn->Canvas->Pixels[x][y] != BackgroundColour)
            {
                TempBitmapOut->Canvas->Pixels[x][y] = NewForegroundColour;
            }
        }
    }
    BitmapOut->Assign(TempBitmapOut);
    delete TempBitmapOut;
    Utilities->CallLogPop(2101);
}

// ---------------------------------------------------------------------------

void TRailGraphics::ChangeSpecificColour(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap *BitmapOut, TColor ColourToBeChanged, TColor NewColour)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ChangeSpecificColour," + AnsiString(ColourToBeChanged) + "," +
                                 AnsiString(NewColour));
    BitmapOut->Height = BitmapIn->Height;
    BitmapOut->Width = BitmapIn->Width;
    for(int x = 0; x < BitmapIn->Width; x++)
    {
        for(int y = 0; y < BitmapIn->Height; y++)
        {
            if(BitmapIn->Canvas->Pixels[x][y] == ColourToBeChanged)
            {
                BitmapOut->Canvas->Pixels[x][y] = NewColour;
            }
        }
    }
    Utilities->CallLogPop(1481);
}

// ---------------------------------------------------------------------------

void TRailGraphics::ChangeBackgroundColour(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap* BitmapOut, TColor NewBackgroundColour,
                                           TColor OldBackgroundColour, bool &ColourError)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ChangeBackgroundColour," + AnsiString(NewBackgroundColour) + "," +
                                 AnsiString(OldBackgroundColour));
    ColourError = false;
    BitmapOut->Height = BitmapIn->Height;
    BitmapOut->Width = BitmapIn->Width;
    bool OneChanged = false;

    for(int x = 0; x < BitmapIn->Width; x++)
    {
        for(int y = 0; y < BitmapIn->Height; y++)
        {
            if(BitmapIn->Canvas->Pixels[x][y] == OldBackgroundColour)
            {
                BitmapOut->Canvas->Pixels[x][y] = NewBackgroundColour;
                if(!OneChanged)
                {
                    if(BitmapOut->Canvas->Pixels[x][y] != NewBackgroundColour)
                    {
                        // can be different if the palette of the pixel is different from that expected
                        ColourError = true;
// throw Exception("BackgroundColour change incorrect - actual = " + AnsiString((int)BitmapOut->Canvas->Pixels[x][y]) + ", expected " + AnsiString((int)NewBackgroundColour));
                    }
                }
                OneChanged = true;
            }
            else
            {
                BitmapOut->Canvas->Pixels[x][y] = BitmapIn->Canvas->Pixels[x][y];
            }
        }
    }
    Utilities->CallLogPop(1482);
}

// ---------------------------------------------------------------------------

void TRailGraphics::ChangeBackgroundColour2(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap* BitmapOut, TColor NewBackgroundColour,
                                            TColor OldBackgroundColour)
{
// superseded by ChangeBackgroundColour3 using direct pixel manipulation
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ChangeBackgroundColour2," + AnsiString(NewBackgroundColour) + "," +
                                 AnsiString(OldBackgroundColour));
    Graphics::TBitmap *TempBitmapOut = new Graphics::TBitmap;

    TempBitmapOut->Assign(BitmapIn); // in case BitmapOut isn't fully defined at this stage
    TRect FullRect(0, 0, BitmapIn->Width, BitmapIn->Height); // the full size of both bitmaps

    TempBitmapOut->Canvas->Brush->Color = NewBackgroundColour; // BrushStyle default is solid so leave at that
    TempBitmapOut->Canvas->FillRect(FullRect); // now all coloured same as new background

// TempBitmapOut->Canvas->CopyMode = cmSrcAnd; //these lines instead of the for.. next pixel loop didn't work, they gave distorted
// TempBitmapOut->Canvas->CopyRect(FullRect, BitmapIn->Canvas, FullRect); //colours, but worked OK without the Assign for a reason I couldn't fathom

    for(int x = 0; x < BitmapIn->Width; x++)
    {
        for(int y = 0; y < BitmapIn->Height; y++)
        {
            {
                if(BitmapIn->Canvas->Pixels[x][y] != OldBackgroundColour)
                {
                    TempBitmapOut->Canvas->Pixels[x][y] = BitmapIn->Canvas->Pixels[x][y];
                }
            }
        }
    }

    BitmapOut->Assign(TempBitmapOut);
    delete TempBitmapOut;
    Utilities->CallLogPop(2102);
}

// ---------------------------------------------------------------------------

void TRailGraphics::ChangeBackgroundColour3(int Caller, Graphics::TBitmap *BitmapIn, Graphics::TBitmap* BitmapOut, TColor NewBackgroundColour,
                                            TColor OldBackgroundColour)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ChangeBackgroundColour3" + AnsiString(NewBackgroundColour) + "," +
                                 AnsiString(OldBackgroundColour));
    Byte *SLPtrIn; // pointer to the ScanLine values in BitmapIn
    Byte *SLPtrOut; // pointer to the ScanLine values in TempBitmapOut
    Graphics::TBitmap *TempBitmapOut = new Graphics::TBitmap;

    TempBitmapOut->Assign(BitmapIn); // in case BitmapOut isn't fully defined at this stage
    int NewBGColourNumber = ColNametoNumber(0, NewBackgroundColour);
    int OldBGColourNumber = ColNametoNumber(1, OldBackgroundColour);

    for(int x = 0; x < BitmapIn->Height; x++)
    {
        SLPtrIn = reinterpret_cast<Byte*>(BitmapIn->ScanLine[x]);
        SLPtrOut = reinterpret_cast<Byte*>(TempBitmapOut->ScanLine[x]);
        for(int y = 0; y < BitmapIn->Width; y++)
        {
            if(SLPtrIn[y] == OldBGColourNumber)
            {
                SLPtrOut[y] = NewBGColourNumber;
            }
            else
            {
                SLPtrOut[y] = SLPtrIn[y];
            }
        }
    }
    BitmapOut->Assign(TempBitmapOut);
    delete TempBitmapOut;
    Utilities->CallLogPop(2103);
}

// ---------------------------------------------------------------------------

int TRailGraphics::ColNametoNumber(int Caller, TColor Colour)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ColNametoNumber" + AnsiString(Colour));
    int Number;

    switch(Colour)
    {
    case clB0G0R0: // black
        Number = 0x0;
        break;

    case clB5G5R5: // white
        Number = 0xd7;
        break;

    case clBufferAttentionNeeded:
        Number = 0x23;
        break;

    case clBufferStopBackground:
        Number = 0xb3;
        break;

    case clCallOnBackground:
        Number = 0xbf;
        break;

    case clCrashedBackground: // covers DerailedBackground because that has the same colour number
        Number = 0xb4;
        break;

    case clNormalBackground:
        Number = 0xac;
        break;

    case clSignallerStopped:
        Number = 0xcf;
        break;

    case clSignalStopBackground:
        Number = 0x66;
        break;

    case clSPADBackground:
        Number = 0xd2;
        break;

    case clStationStopBackground:
        Number = 0xb2;
        break;

    case clStoppedTrainInFront:
        Number = 0x83;
        break;

    case clTRSBackground:
        Number = 0xd1;
        break;

    case clTrainFailedBackground: // added at v2.4.0
        Number = 0xc0;
        break;

    default:
// UnicodeString MessageStr = "Can't find required colour - normal background colour will be used.");
// Application->MessageBox(MessageStr.c_str(), L"", MB_OK);  don't give message as can be called when operating (need StopTTClockMessage)
        Number = 0xac; // normal background
        break;
    }
    Utilities->CallLogPop(2104);
    return(Number);
}

// ---------------------------------------------------------------------------

void TRailGraphics::ChangeAllTransparentColours(TColor NewTransparentColour, TColor OldTransparentColour)
{
    if(NewTransparentColour == OldTransparentColour)
    {
        return; // already stored

    }
    ChangeTransparentColour(bm10, bm10, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm100, bm100, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm101, bm101, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm106, bm106, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm10Diverging, bm10Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm10Straight, bm10Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm11, bm11, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm11Diverging, bm11Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm11Straight, bm11Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm12, bm12, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm12Diverging, bm12Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm12Straight, bm12Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm13, bm13, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm132, bm132, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm132LeftFork, bm132LeftFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm132RightFork, bm132RightFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm133, bm133, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm133LeftFork, bm133LeftFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm133RightFork, bm133RightFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm134, bm134, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm134LeftFork, bm134LeftFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm134RightFork, bm134RightFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm135, bm135, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm135LeftFork, bm135LeftFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm135RightFork, bm135RightFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm136, bm136, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm136LeftFork, bm136LeftFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm136RightFork, bm136RightFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm137, bm137, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm137LeftFork, bm137LeftFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm137RightFork, bm137RightFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm138, bm138, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm138LeftFork, bm138LeftFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm138RightFork, bm138RightFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm139, bm139, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm139LeftFork, bm139LeftFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm139RightFork, bm139RightFork, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm13Diverging, bm13Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm13Straight, bm13Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm14, bm14, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm140, bm140, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm141, bm141, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm14Diverging, bm14Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm14Straight, bm14Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm16, bm16, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm18, bm18, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm20, bm20, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm27, bm27, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm28, bm28, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm28Diverging, bm28Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm28Straight, bm28Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm29, bm29, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm29Diverging, bm29Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm29Straight, bm29Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm30, bm30, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm30Diverging, bm30Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm30Straight, bm30Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm31, bm31, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm31Diverging, bm31Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm31Straight, bm31Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm32, bm32, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm32Diverging, bm32Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm32Straight, bm32Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm33, bm33, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm33Diverging, bm33Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm33Straight, bm33Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm34, bm34, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm34Diverging, bm34Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm34Straight, bm34Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm35, bm35, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm35Diverging, bm35Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm35Straight, bm35Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm36, bm36, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm36Diverging, bm36Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm36Straight, bm36Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm37, bm37, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm37Diverging, bm37Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm37Straight, bm37Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm38, bm38, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm38Diverging, bm38Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm38Straight, bm38Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm39, bm39, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm39Diverging, bm39Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm39Straight, bm39Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm40, bm40, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm40Diverging, bm40Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm40Straight, bm40Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm41, bm41, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm41Diverging, bm41Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm41Straight, bm41Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm42, bm42, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm42Diverging, bm42Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm42Straight, bm42Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm43, bm43, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm43Diverging, bm43Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm43Straight, bm43Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm45, bm45, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm46, bm46, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm50, bm50, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm51, bm51, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm53, bm53, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm54, bm54, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm56, bm56, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm59, bm59, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm65, bm65, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm68CallingOn, bm68CallingOn, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm68dblyellow, bm68dblyellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm68grounddblred, bm68grounddblred, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm68grounddblwhite, bm68grounddblwhite, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm68green, bm68green, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm68yellow, bm68yellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm69CallingOn, bm69CallingOn, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm69dblyellow, bm69dblyellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm69grounddblred, bm69grounddblred, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm69grounddblwhite, bm69grounddblwhite, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm69green, bm69green, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm69yellow, bm69yellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm7, bm7, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm70CallingOn, bm70CallingOn, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm70dblyellow, bm70dblyellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm70grounddblred, bm70grounddblred, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm70grounddblwhite, bm70grounddblwhite, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm70green, bm70green, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm70yellow, bm70yellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm71CallingOn, bm71CallingOn, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm71dblyellow, bm71dblyellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm71grounddblred, bm71grounddblred, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm71grounddblwhite, bm71grounddblwhite, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm71green, bm71green, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm71yellow, bm71yellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm72CallingOn, bm72CallingOn, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm72dblyellow, bm72dblyellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm72grounddblred, bm72grounddblred, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm72grounddblwhite, bm72grounddblwhite, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm72green, bm72green, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm72yellow, bm72yellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm73, bm73, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm73CallingOn, bm73CallingOn, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm73dblyellow, bm73dblyellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm73grounddblred, bm73grounddblred, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm73grounddblwhite, bm73grounddblwhite, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm73green, bm73green, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm73yellow, bm73yellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm74, bm74, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm74CallingOn, bm74CallingOn, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm74dblyellow, bm74dblyellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm74grounddblred, bm74grounddblred, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm74grounddblwhite, bm74grounddblwhite, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm74green, bm74green, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm74yellow, bm74yellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm75CallingOn, bm75CallingOn, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm75dblyellow, bm75dblyellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm75grounddblred, bm75grounddblred, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm75grounddblwhite, bm75grounddblwhite, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm75green, bm75green, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm75yellow, bm75yellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm77, bm77, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm77Striped, bm77Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm78, bm78, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm78Striped, bm78Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm7Diverging, bm7Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm7Straight, bm7Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm8, bm8, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm85, bm85, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm8Diverging, bm8Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm8Straight, bm8Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm9, bm9, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm93set, bm93set, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm93unset, bm93unset, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm94set, bm94set, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm94unset, bm94unset, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm9Diverging, bm9Diverging, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bm9Straight, bm9Straight, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmGreenEllipse, bmGreenEllipse, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmGreenRect, bmGreenRect, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmGrid, bmGrid, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmLightBlueRect, bmLightBlueRect, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmName, bmName, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmNameStriped, bmNameStriped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmRedEllipse, bmRedEllipse, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmRedRect, bmRedRect, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmRtCancELink1, bmRtCancELink1, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmRtCancELink2, bmRtCancELink2, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmRtCancELink3, bmRtCancELink3, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmRtCancELink4, bmRtCancELink4, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmRtCancELink6, bmRtCancELink6, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmRtCancELink7, bmRtCancELink7, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmRtCancELink8, bmRtCancELink8, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmRtCancELink9, bmRtCancELink9, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br1, br1, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br10, br10, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br11, br11, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br12, br12, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br2, br2, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br3, br3, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br4, br4, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br5, br5, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br6, br6, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br7, br7, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br8, br8, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(br9, br9, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(Concourse, Concourse, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(ConcourseGlyph, ConcourseGlyph, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(ConcourseStriped, ConcourseStriped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(ELk1, ELk1, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(ELk2, ELk2, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(ELk3, ELk3, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(ELk4, ELk4, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(ELk6, ELk6, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(ELk7, ELk7, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(ELk8, ELk8, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(ELk9, ELk9, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl1, gl1, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl10, gl10, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl100, gl100, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl101, gl101, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl102, gl102, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl103, gl103, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl104, gl104, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl105, gl105, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl106, gl106, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl107, gl107, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl108, gl108, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl109, gl109, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl11, gl11, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl110, gl110, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl111, gl111, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl112, gl112, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl113, gl113, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl114, gl114, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl115, gl115, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl116, gl116, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl117, gl117, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl118, gl118, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl119, gl119, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl12, gl12, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl120, gl120, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl121, gl121, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl122, gl122, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl123, gl123, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl124, gl124, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl125, gl125, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl126, gl126, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl127, gl127, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl128, gl128, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl129, gl129, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl129Striped, gl129Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl13, gl13, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl130, gl130, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl130Striped, gl130Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl131, gl131, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl132, gl132, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl133, gl133, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl134, gl134, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl135, gl135, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl136, gl136, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl137, gl137, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl138, gl138, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl139, gl139, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl14, gl14, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl140, gl140, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl141, gl141, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl142, gl142, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl143, gl143, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl145, gl145, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl145Striped, gl145Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl146, gl146, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl146Striped, gl146Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl15, gl15, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl16, gl16, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl18, gl18, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl19, gl19, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl2, gl2, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl20, gl20, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl21, gl21, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl22, gl22, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl23, gl23, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl24, gl24, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl25, gl25, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl26, gl26, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl27, gl27, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl28, gl28, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl29, gl29, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl3, gl3, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl30, gl30, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl31, gl31, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl32, gl32, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl33, gl33, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl34, gl34, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl35, gl35, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl36, gl36, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl37, gl37, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl38, gl38, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl39, gl39, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl4, gl4, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl40, gl40, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl41, gl41, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl42, gl42, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl43, gl43, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl44, gl44, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl45, gl45, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl46, gl46, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl47, gl47, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl48, gl48, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl49, gl49, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl5, gl5, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl50, gl50, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl51, gl51, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl52, gl52, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl53, gl53, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl54, gl54, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl55, gl55, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl56, gl56, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl57, gl57, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl58, gl58, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl59, gl59, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl6, gl6, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl60, gl60, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl61, gl61, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl62, gl62, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl63, gl63, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl64, gl64, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl65, gl65, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl66, gl66, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl67, gl67, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl68, gl68, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl69, gl69, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl7, gl7, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl70, gl70, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl71, gl71, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl72, gl72, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl73, gl73, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl74, gl74, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl75, gl75, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl76, gl76, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl76Striped, gl76Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl77, gl77, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl78, gl78, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl79, gl79, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl79Striped, gl79Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl8, gl8, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl80, gl80, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl81, gl81, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl82, gl82, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl83, gl83, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl84, gl84, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl85, gl85, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl86, gl86, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl87, gl87, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl88set, gl88set, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl88unset, gl88unset, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl89set, gl89set, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl89unset, gl89unset, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl9, gl9, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl90set, gl90set, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl90unset, gl90unset, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl91set, gl91set, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl91unset, gl91unset, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl92set, gl92set, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl92unset, gl92unset, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl93set, gl93set, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl94set, gl94set, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl95set, gl95set, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl95unset, gl95unset, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl97, gl97, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl98, gl98, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(gl99, gl99, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(Plat68, Plat68, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(Plat68Striped, Plat68Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(Plat69, Plat69, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(Plat69Striped, Plat69Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(Plat70, Plat70, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(Plat70Striped, Plat70Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(Plat71, Plat71, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(Plat71Striped, Plat71Striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm1, sm1, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm10, sm10, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm100, sm100, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm101, sm101, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm102, sm102, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm103, sm103, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm104, sm104, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm105, sm105, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm106, sm106, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm107, sm107, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm108, sm108, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm109, sm109, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm11, sm11, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm110, sm110, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm111, sm111, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm112, sm112, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm115, sm115, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm117, sm117, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm12, sm12, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm129, sm129, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm129striped, sm129striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm13, sm13, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm130, sm130, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm130striped, sm130striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm131striped, sm131striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm132, sm132, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm133, sm133, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm134, sm134, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm135, sm135, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm136, sm136, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm137, sm137, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm138, sm138, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm139, sm139, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm14, sm14, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm15, sm15, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm16, sm16, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm18, sm18, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm19, sm19, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm2, sm2, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm20, sm20, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm21, sm21, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm22, sm22, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm23, sm23, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm24, sm24, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm25, sm25, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm26, sm26, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm27, sm27, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm28, sm28, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm29, sm29, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm3, sm3, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm30, sm30, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm31, sm31, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm32, sm32, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm33, sm33, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm34, sm34, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm35, sm35, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm36, sm36, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm37, sm37, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm38, sm38, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm39, sm39, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm4, sm4, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm40, sm40, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm41, sm41, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm42, sm42, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm43, sm43, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm44, sm44, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm45, sm45, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm46, sm46, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm47, sm47, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm48, sm48, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm49, sm49, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm5, sm5, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm50, sm50, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm51, sm51, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm52, sm52, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm53, sm53, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm54, sm54, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm55, sm55, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm56, sm56, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm57, sm57, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm58, sm58, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm59, sm59, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm6, sm6, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm60, sm60, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm61, sm61, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm62, sm62, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm63, sm63, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm64, sm64, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm65, sm65, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm66, sm66, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm67, sm67, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm7, sm7, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm76, sm76, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm76striped, sm76striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm77, sm77, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm77striped, sm77striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm78, sm78, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm78striped, sm78striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm79, sm79, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm79striped, sm79striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm8, sm8, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm80, sm80, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm81, sm81, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm82, sm82, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm83, sm83, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm84, sm84, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm85, sm85, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm86, sm86, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm87, sm87, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm88, sm88, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm89, sm89, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm9, sm9, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm90, sm90, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm91, sm91, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm92, sm92, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm93, sm93, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm94, sm94, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm95, sm95, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm96, sm96, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm96striped, sm96striped, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm97, sm97, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm98, sm98, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(sm99, sm99, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smBlack, smBlack, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smBlue, smBlue, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smBrightGreen, smBrightGreen, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smCaramel, smCaramel, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smCyan, smCyan, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smLightBlue, smLightBlue, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smMagenta, smMagenta, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smName, smName, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smPaleGreen, smPaleGreen, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smRed, smRed, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smYellow, smYellow, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smTransparent, smTransparent, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(TempBackground, TempBackground, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(TempHeadCode, TempHeadCode, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(UnderHFootbridge, UnderHFootbridge, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(UnderVFootbridge, UnderVFootbridge, NewTransparentColour, OldTransparentColour);

    // The following are created as new bitmaps from existing  files
    ChangeTransparentColour(bmTransparentBgnd, bmTransparentBgnd, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(GridBitmap, GridBitmap, NewTransparentColour, OldTransparentColour);

    // non-transparent graphics - don't change headcodes
    ChangeTransparentColour(bmSolidBgnd, bmSolidBgnd, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(smSolidBgnd, smSolidBgnd, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmDiagonalSignalBlank, bmDiagonalSignalBlank, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmPointBlank, bmPointBlank, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmStraightEWSignalBlank, bmStraightEWSignalBlank, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(bmStraightNSSignalBlank, bmStraightNSSignalBlank, NewTransparentColour, OldTransparentColour);

    // level crossing graphics
    ChangeTransparentColour(LCBothHor, LCBothHor, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCBotHor, LCBotHor, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCBothVer, LCBothVer, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCLHSVer, LCLHSVer, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCPlain, LCPlain, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCRHSVer, LCRHSVer, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCTopHor, LCTopHor, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCBothHorMan, LCBothHorMan, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCBotHorMan, LCBotHorMan, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCBothVerMan, LCBothVerMan, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCLHSVerMan, LCLHSVerMan, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCPlainMan, LCPlainMan, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCRHSVerMan, LCRHSVerMan, NewTransparentColour, OldTransparentColour);
    ChangeTransparentColour(LCTopHorMan, LCTopHorMan, NewTransparentColour, OldTransparentColour);

// change the grid to the nearest grey colour to the background
    if(NewTransparentColour != clB5G5R5)
    {
        ChangeSpecificColour(1, GridBitmap, GridBitmap, clB4G4R4, clB1G1R1); // if already dark will ignore
    }
    else
    {
        ChangeSpecificColour(3, GridBitmap, GridBitmap, clB1G1R1, clB4G4R4); // if already light will ignore
    }
}

// ---------------------------------------------------------------------------

void TRailGraphics::SetUpAllDerivitiveGraphics(TColor TransparentColour)
{
    for(int x = 0; x < 30; x++)
    {
        LinkPrefDirGraphicsPtr[x]->TransparentColor = TransparentColour;
        LinkNonSigRouteGraphicsPtr[x]->TransparentColor = TransparentColour;
        LinkSigRouteGraphicsPtr[x]->TransparentColor = TransparentColour;
        LinkRouteAutoSigsGraphicsPtr[x]->TransparentColor = TransparentColour;
    }
    for(int x = 0; x < 12; x++)
    {
        BridgePrefDirGraphicsPtr[x]->TransparentColor = TransparentColour;
        BridgeNonSigRouteGraphicsPtr[x]->TransparentColor = TransparentColour;
        BridgeSigRouteGraphicsPtr[x]->TransparentColor = TransparentColour;
        BridgeRouteAutoSigsGraphicsPtr[x]->TransparentColor = TransparentColour;
    }

/*
      EXArray with associated Tag numbers:-
      {4,6} 1,{2,8} 2,                                                         //horizontal & vertical
      {2,4} 6,{6,2} 5,{8,6} 3,{4,8}, 4                                         //sharp curves
      {1,6} 22,{3,8} 24,{9,4} 21,{7,2} 27,{1,8} 25,{3,4} 23,{9,2} 26,{7,6}, 20 //loose curves
      {1,9} 19,{3,7} 18,                                                       //forward & reverse diagonals

      int Bridge EXArray with Tag numbers:-
      {4,6} 48,{2,8} 49,{1,9} 50,{3,7} 51,
      {1,9} 52,{3,7} 56,{1,9} 57,{3,7} 53,
      {2,8} 54,{4,6} 59,{2,8} 55,{4,6} 58

      reverse direction same in each case
*/

    // Note: DirectionPrefDirGraphicsPtr[0] & [5] unused
    DirectionPrefDirGraphicsPtr[0]->Assign(ELk1);
    DirectionPrefDirGraphicsPtr[1]->Assign(ELk1);
    DirectionPrefDirGraphicsPtr[2]->Assign(ELk2);
    DirectionPrefDirGraphicsPtr[3]->Assign(ELk3);
    DirectionPrefDirGraphicsPtr[4]->Assign(ELk4);
    DirectionPrefDirGraphicsPtr[5]->Assign(ELk4);
    DirectionPrefDirGraphicsPtr[6]->Assign(ELk6);
    DirectionPrefDirGraphicsPtr[7]->Assign(ELk7);
    DirectionPrefDirGraphicsPtr[8]->Assign(ELk8);
    DirectionPrefDirGraphicsPtr[9]->Assign(ELk9);

    DirectionNonSigRouteGraphicsPtr[0]->Assign(ELk1);
    DirectionNonSigRouteGraphicsPtr[1]->Assign(ELk1);
    DirectionNonSigRouteGraphicsPtr[2]->Assign(ELk2);
    DirectionNonSigRouteGraphicsPtr[3]->Assign(ELk3);
    DirectionNonSigRouteGraphicsPtr[4]->Assign(ELk4);
    DirectionNonSigRouteGraphicsPtr[5]->Assign(ELk4);
    DirectionNonSigRouteGraphicsPtr[6]->Assign(ELk6);
    DirectionNonSigRouteGraphicsPtr[7]->Assign(ELk7);
    DirectionNonSigRouteGraphicsPtr[8]->Assign(ELk8);
    DirectionNonSigRouteGraphicsPtr[9]->Assign(ELk9);

    DirectionSigRouteGraphicsPtr[0]->Assign(ELk1);
    DirectionSigRouteGraphicsPtr[1]->Assign(ELk1);
    DirectionSigRouteGraphicsPtr[2]->Assign(ELk2);
    DirectionSigRouteGraphicsPtr[3]->Assign(ELk3);
    DirectionSigRouteGraphicsPtr[4]->Assign(ELk4);
    DirectionSigRouteGraphicsPtr[5]->Assign(ELk4);
    DirectionSigRouteGraphicsPtr[6]->Assign(ELk6);
    DirectionSigRouteGraphicsPtr[7]->Assign(ELk7);
    DirectionSigRouteGraphicsPtr[8]->Assign(ELk8);
    DirectionSigRouteGraphicsPtr[9]->Assign(ELk9);

    DirectionRouteAutoSigsGraphicsPtr[0]->Assign(ELk1);
    DirectionRouteAutoSigsGraphicsPtr[1]->Assign(ELk1);
    DirectionRouteAutoSigsGraphicsPtr[2]->Assign(ELk2);
    DirectionRouteAutoSigsGraphicsPtr[3]->Assign(ELk3);
    DirectionRouteAutoSigsGraphicsPtr[4]->Assign(ELk4);
    DirectionRouteAutoSigsGraphicsPtr[5]->Assign(ELk4);
    DirectionRouteAutoSigsGraphicsPtr[6]->Assign(ELk6);
    DirectionRouteAutoSigsGraphicsPtr[7]->Assign(ELk7);
    DirectionRouteAutoSigsGraphicsPtr[8]->Assign(ELk8);
    DirectionRouteAutoSigsGraphicsPtr[9]->Assign(ELk9);

    for(int x = 0; x < 30; x++)
    {
        ChangeForegroundColour(5, LinkGraphicsPtr[x], LinkPrefDirGraphicsPtr[x], clB2G0R4, TransparentColour); // magenta
        ChangeForegroundColour(6, LinkGraphicsPtr[x], LinkNonSigRouteGraphicsPtr[x], clB0G0R5, TransparentColour); // red
        ChangeForegroundColour(7, LinkGraphicsPtr[x], LinkSigRouteGraphicsPtr[x], clB0G4R0, TransparentColour); // green
        ChangeForegroundColour(8, LinkGraphicsPtr[x], LinkRouteAutoSigsGraphicsPtr[x], clB5G3R0, TransparentColour); // blue
    }
    for(int x = 0; x < 12; x++)
    {
        ChangeForegroundColour(9, BridgeGraphicsPtr[x], BridgePrefDirGraphicsPtr[x], clB2G0R4, TransparentColour);
        ChangeForegroundColour(10, BridgeGraphicsPtr[x], BridgeNonSigRouteGraphicsPtr[x], clB0G0R5, TransparentColour);
        ChangeForegroundColour(11, BridgeGraphicsPtr[x], BridgeSigRouteGraphicsPtr[x], clB0G4R0, TransparentColour);
        ChangeForegroundColour(12, BridgeGraphicsPtr[x], BridgeRouteAutoSigsGraphicsPtr[x], clB5G3R0, TransparentColour);
    }
    for(int x = 0; x < 10; x++)
    {
        ChangeForegroundColour(13, DirectionPrefDirGraphicsPtr[x], DirectionPrefDirGraphicsPtr[x], clB2G0R4, TransparentColour);
        ChangeForegroundColour(14, DirectionNonSigRouteGraphicsPtr[x], DirectionNonSigRouteGraphicsPtr[x], clB0G0R5, TransparentColour);
        ChangeForegroundColour(15, DirectionSigRouteGraphicsPtr[x], DirectionSigRouteGraphicsPtr[x], clB0G4R0, TransparentColour);
        ChangeForegroundColour(16, DirectionRouteAutoSigsGraphicsPtr[x], DirectionRouteAutoSigsGraphicsPtr[x], clB5G3R0, TransparentColour);
    }

    // set up points' fillets
    PointModeGraphicsPtr[0][0]->Assign(bm7Straight);
    PointModeGraphicsPtr[1][0]->Assign(bm8Straight);
    PointModeGraphicsPtr[2][0]->Assign(bm9Straight);
    PointModeGraphicsPtr[3][0]->Assign(bm10Straight);
    PointModeGraphicsPtr[4][0]->Assign(bm11Straight);
    PointModeGraphicsPtr[5][0]->Assign(bm12Straight);
    PointModeGraphicsPtr[6][0]->Assign(bm13Straight);
    PointModeGraphicsPtr[7][0]->Assign(bm14Straight);
    PointModeGraphicsPtr[8][0]->Assign(bm28Straight);
    PointModeGraphicsPtr[9][0]->Assign(bm29Straight);
    PointModeGraphicsPtr[10][0]->Assign(bm30Straight);
    PointModeGraphicsPtr[11][0]->Assign(bm31Straight);
    PointModeGraphicsPtr[12][0]->Assign(bm32Straight);
    PointModeGraphicsPtr[13][0]->Assign(bm33Straight);
    PointModeGraphicsPtr[14][0]->Assign(bm34Straight);
    PointModeGraphicsPtr[15][0]->Assign(bm35Straight);
    PointModeGraphicsPtr[16][0]->Assign(bm36Straight);
    PointModeGraphicsPtr[17][0]->Assign(bm37Straight);
    PointModeGraphicsPtr[18][0]->Assign(bm38Straight);
    PointModeGraphicsPtr[19][0]->Assign(bm39Straight);
    PointModeGraphicsPtr[20][0]->Assign(bm40Straight);
    PointModeGraphicsPtr[21][0]->Assign(bm41Straight);
    PointModeGraphicsPtr[22][0]->Assign(bm42Straight);
    PointModeGraphicsPtr[23][0]->Assign(bm43Straight);

    PointModeGraphicsPtr[24][0]->Assign(bm132LeftFork);
    PointModeGraphicsPtr[25][0]->Assign(bm133LeftFork);
    PointModeGraphicsPtr[26][0]->Assign(bm134LeftFork);
    PointModeGraphicsPtr[27][0]->Assign(bm135LeftFork);
    PointModeGraphicsPtr[28][0]->Assign(bm136LeftFork);
    PointModeGraphicsPtr[29][0]->Assign(bm137LeftFork);
    PointModeGraphicsPtr[30][0]->Assign(bm138LeftFork);
    PointModeGraphicsPtr[31][0]->Assign(bm139LeftFork);

    PointModeGraphicsPtr[0][1]->Assign(bm7Diverging);
    PointModeGraphicsPtr[1][1]->Assign(bm8Diverging);
    PointModeGraphicsPtr[2][1]->Assign(bm9Diverging);
    PointModeGraphicsPtr[3][1]->Assign(bm10Diverging);
    PointModeGraphicsPtr[4][1]->Assign(bm11Diverging);
    PointModeGraphicsPtr[5][1]->Assign(bm12Diverging);
    PointModeGraphicsPtr[6][1]->Assign(bm13Diverging);
    PointModeGraphicsPtr[7][1]->Assign(bm14Diverging);
    PointModeGraphicsPtr[8][1]->Assign(bm28Diverging);
    PointModeGraphicsPtr[9][1]->Assign(bm29Diverging);
    PointModeGraphicsPtr[10][1]->Assign(bm30Diverging);
    PointModeGraphicsPtr[11][1]->Assign(bm31Diverging);
    PointModeGraphicsPtr[12][1]->Assign(bm32Diverging);
    PointModeGraphicsPtr[13][1]->Assign(bm33Diverging);
    PointModeGraphicsPtr[14][1]->Assign(bm34Diverging);
    PointModeGraphicsPtr[15][1]->Assign(bm35Diverging);
    PointModeGraphicsPtr[16][1]->Assign(bm36Diverging);
    PointModeGraphicsPtr[17][1]->Assign(bm37Diverging);
    PointModeGraphicsPtr[18][1]->Assign(bm38Diverging);
    PointModeGraphicsPtr[19][1]->Assign(bm39Diverging);
    PointModeGraphicsPtr[20][1]->Assign(bm40Diverging);
    PointModeGraphicsPtr[21][1]->Assign(bm41Diverging);
    PointModeGraphicsPtr[22][1]->Assign(bm42Diverging);
    PointModeGraphicsPtr[23][1]->Assign(bm43Diverging);

    PointModeGraphicsPtr[24][1]->Assign(bm132RightFork);
    PointModeGraphicsPtr[25][1]->Assign(bm133RightFork);
    PointModeGraphicsPtr[26][1]->Assign(bm134RightFork);
    PointModeGraphicsPtr[27][1]->Assign(bm135RightFork);
    PointModeGraphicsPtr[28][1]->Assign(bm136RightFork);
    PointModeGraphicsPtr[29][1]->Assign(bm137RightFork);
    PointModeGraphicsPtr[30][1]->Assign(bm138RightFork);
    PointModeGraphicsPtr[31][1]->Assign(bm139RightFork);
}

// ---------------------------------------------------------------------------

void TRailGraphics::ConvertSignalsToOppositeHand(int Caller) // new at v2.3.0

{
    Utilities->EventLog.push_back("ConvertSignalsToRightHand");
    if(Utilities->EventLog.size() > 1000)
    {
        Utilities->EventLog.pop_front();
    }
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ", ConvertSignalsToRightHand");

    Graphics::TBitmap* HorizSignalArray[18] =
    {
        Plat68, Plat68Striped, Plat69, Plat69Striped, bm68CallingOn, bm68dblyellow, bm68grounddblred, bm68grounddblwhite, bm68green, bm68yellow, bm69CallingOn,
        bm69dblyellow, bm69grounddblred, bm69grounddblwhite, bm69green, bm69yellow, gl68, gl69
    };

    Graphics::TBitmap* VertSignalArray[18] =
    {
        Plat70, Plat70Striped, Plat71, Plat71Striped, bm70CallingOn, bm70dblyellow, bm70grounddblred, bm70grounddblwhite, bm70green, bm70yellow, bm71CallingOn,
        bm71dblyellow, bm71grounddblred, bm71grounddblwhite, bm71green, bm71yellow, gl70, gl71
    };

    Graphics::TBitmap* BackDiagSignalArray[14] =
    {
        bm72CallingOn, bm72dblyellow, bm72grounddblred, bm72grounddblwhite, bm72green, bm72yellow, bm75CallingOn, bm75dblyellow, bm75grounddblred,
        bm75grounddblwhite, bm75green, bm75yellow, gl72, gl75
    };

    Graphics::TBitmap* FwdDiagSignalArray[16] =
    {
        bm73, bm73CallingOn, bm73dblyellow, bm73grounddblred, bm73grounddblwhite, bm73green, bm73yellow, bm74, bm74CallingOn, bm74dblyellow, bm74grounddblred,
        bm74grounddblwhite, bm74green, bm74yellow, gl73, gl74
    };

// the following are the stay black glyphs for the speedbuttons
    Graphics::TBitmap* HorizSignalGlyphArray[4] =
    {
        SpeedBut68NormBlackGlyph, SpeedBut69NormBlackGlyph, SpeedBut68GrndBlackGlyph, SpeedBut69GrndBlackGlyph
    };

    Graphics::TBitmap* VertSignalGlyphArray[4] =
    {
        SpeedBut70NormBlackGlyph, SpeedBut71NormBlackGlyph, SpeedBut70GrndBlackGlyph, SpeedBut71GrndBlackGlyph
    };

    Graphics::TBitmap* BackDiagSignalGlyphArray[4] =
    {
        SpeedBut72NormBlackGlyph, SpeedBut75NormBlackGlyph, SpeedBut72GrndBlackGlyph, SpeedBut75GrndBlackGlyph,
    };

    Graphics::TBitmap* FwdDiagSignalGlyphArray[4] =
    {
        SpeedBut73NormBlackGlyph, SpeedBut74NormBlackGlyph, SpeedBut73GrndBlackGlyph, SpeedBut74GrndBlackGlyph
    };

    Graphics::TBitmap* TmpBM;

    TmpBM = new Graphics::TBitmap;
    TmpBM->Assign(bmTransparentBgnd); // current background colour as transparent colour

    for(int x = 0; x < 18; x++)
    {
        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                TmpBM->Canvas->Pixels[i][j] = HorizSignalArray[x]->Canvas->Pixels[i][15 - j];
            }
        }
        HorizSignalArray[x]->Assign(TmpBM);
    }

    TmpBM->Assign(bmTransparentBgnd); // current background colour as transparent colour
    for(int x = 0; x < 18; x++)
    {
        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                TmpBM->Canvas->Pixels[i][j] = VertSignalArray[x]->Canvas->Pixels[15 - i][j];
            }
        }
        VertSignalArray[x]->Assign(TmpBM);
    }

    TmpBM->Assign(bmTransparentBgnd); // current background colour as transparent colour
    for(int x = 0; x < 14; x++)
    {
        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                TmpBM->Canvas->Pixels[i][j] = BackDiagSignalArray[x]->Canvas->Pixels[j][i];
            }
        }
        BackDiagSignalArray[x]->Assign(TmpBM); // current background colour as transparent colour
    }

    TmpBM->Assign(bmTransparentBgnd);
    for(int x = 0; x < 16; x++)
    {
        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                TmpBM->Canvas->Pixels[i][j] = FwdDiagSignalArray[x]->Canvas->Pixels[15 - j][15 - i];
            }
        }
        FwdDiagSignalArray[x]->Assign(TmpBM);
    }

    TmpBM->LoadFromResourceName(0, "bmSolidBgnd");
    TmpBM->Transparent = true;
    TmpBM->TransparentColor = clB5G5R5;
    // white as transparent colour for speedbutton glyphs
    for(int x = 0; x < 4; x++)
    {
        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                TmpBM->Canvas->Pixels[i][j] = HorizSignalGlyphArray[x]->Canvas->Pixels[i][15 - j];
            }
        }
        HorizSignalGlyphArray[x]->Assign(TmpBM);
    }

    TmpBM->LoadFromResourceName(0, "bmSolidBgnd");
    TmpBM->Transparent = true;
    TmpBM->TransparentColor = clB5G5R5;
    // white as transparent colour for speedbutton glyphs
    for(int x = 0; x < 4; x++)
    {
        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                TmpBM->Canvas->Pixels[i][j] = VertSignalGlyphArray[x]->Canvas->Pixels[15 - i][j];
            }
        }
        VertSignalGlyphArray[x]->Assign(TmpBM);
    }

    TmpBM->LoadFromResourceName(0, "bmSolidBgnd");
    TmpBM->Transparent = true;
    TmpBM->TransparentColor = clB5G5R5;
    // white as transparent colour for speedbutton glyphs
    for(int x = 0; x < 4; x++)
    {
        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                TmpBM->Canvas->Pixels[i][j] = BackDiagSignalGlyphArray[x]->Canvas->Pixels[j][i];
            }
        }
        BackDiagSignalGlyphArray[x]->Assign(TmpBM);
    }

    TmpBM->LoadFromResourceName(0, "bmSolidBgnd");
    TmpBM->Transparent = true;
    TmpBM->TransparentColor = clB5G5R5;
    // white as transparent colour for speedbutton glyphs
    for(int x = 0; x < 4; x++)
    {
        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                TmpBM->Canvas->Pixels[i][j] = FwdDiagSignalGlyphArray[x]->Canvas->Pixels[15 - j][15 - i];
            }
        }
        FwdDiagSignalGlyphArray[x]->Assign(TmpBM);
    }

    Utilities->RHSignalFlag = !Utilities->RHSignalFlag; // set in itially to false (= LH)
    Utilities->CallLogPop(74);
}

// ---------------------------------------------------------------------------

