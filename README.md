# Railway Operation Simulator (RailOS)

Welcome to the RailOS project, a railway simulator for Windows originally written in Borland's C++ Builder 4 Professional and now updated to work with Embarcadero's C++ Builder Community Edition v10.4.  Development to date has resulted in a complete and usable program that provides options to:

- build a railway of any size
- add text of any available colour, font and size
- set preferred and bi-directional running directions
- allow random train failures
- allow random train delays, failures of signals and points, and temporary speed restrictions
- choose light or dark backgrounds
- develop timetables with splits, joins, follow-on services, changes in direction, shuttles and repeating services.
- operate trains
- save and reload an operating session at any time
- add your own graphics
- zoom-out for a wider display
- and many other features

A wide selection of track element types is available for building the railway, together with station elements consisting of platforms, concourses and footbridges.  Also available are non-station named location elements for sidings, works, depots, junction approaches and anything else that needs a name.  Location names are displayed on the railway in user-selectable font, style, size and colour.  In addition to named locations any other text may be added to the railway, again in any font, style, size and colour.  Location names and other text may be moved in order to improve the appearance of the railway and areas of railway may be selected and cut, copied, pasted, deleted, mirrored and flipped etc.  Railway files may be saved and loaded in both development form during construction and in operational form on completion.  Track element lengths and line speed limits may be set individually, along tracks, or in areas by selection, as can preferred running directions.

Trains may operate to a timetable developed using the internal timetable editor, or under signaller control.  Three types of route are available:

1. Automatic signal routes:  set signal to signal in preferred directions, the route is retained after trains pass and signals automatically return to green in stages as blocks ahead are cleared
2. Preferred direction routes:  set signal to signal or from signal to any following signal in preferred directions, and the route is cleared as trains pass
3. Unrestricted routes:  set from most types of track element to other track elements in any direction.  The route is cleared as trains pass

In addition trains will run on track that has no route set, but they are then much more vulnerable to derailments and crashes.

Although the program is fully usable now, there remains great potential for further development.  Some ideas include:

- automatic route setting
- option to use & display imperial units as well as metric units
- ability to record and replay sessions
- signalbox mode where individual signals and points are operated directly, perhaps via a graphical lever frame, with user-defined interlocking
- sound effects, e.g. enter a track ID & hear trains & station announcements at that location
- variable train lengths
- restricted routes - e.g. DC 3 & 4 rail, 25kV AC, tube lines, etc.
- multi-player operation over the internet

More recent suggestions are included in the file DevHistory.txt.

'DeveloperGuide.pdf' in the 'master' branch provides a summary of program history, structure, operation and interfaces, and also explains the file structure.  Anyone wishing to contribute to further development should read 'CONTRIBUTING.md'.

We can be contacted at railwayfeedback@gmail.com or via the website at www.railwayoperationsimulator.com

Menu icons from Silk Icon Set 1.3 by Mark James used under Creative Commons Attribution 2.5 License http://creativecommons.org/licenses/by/2.5/

Timetable editor icons from https://icons8.com used under Creative Commons Attribution-NoDerivs 3.0 Unported License http://creativecommons.org/licenses/by-nd/3.0/

The following notice relates to incorporation of Internet Direct (Indy) components used in the multiplayer functions under the BSD Licence.

Portions of this software are Copyright (c) 1993 ??? 2018, Chad Z. Hower (Kudzu) and the Indy Pit Crew ??? http://www.IndyProject.org/

License

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation, about box and/or other materials provided with the distribution.
    No personal names or organizations names associated with the Indy project may be used to endorse or promote products derived from this software without specific prior written permission of the specific individual or organization.

THIS SOFTWARE IS PROVIDED BY Chad Z. Hower (Kudzu) and the Indy Pit Crew ???AS IS??? AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,  ROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
