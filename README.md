# railway-dot-exe
Automatically exported from code.google.com/p/railway-dot-exe

Welcome to the railway.exe project, a railway simulator for Windows written in Borland's C++ Builder 4 Professional.  Development to date has resulted in a complete and usable program that provides options to:-

build a railway of any size;
add text of any available colour, font and size;
set preferred and bi-directional running directions;
choose light or dark backgrounds;
develop timetables with shuttle services, changes in direction, splits, joins, and repeating services;
operate trains; and zoom-out for a wider display

A wide selection of track element types is available for building the railway, together with station elements consisting of platforms, concourses and footbridges.  Also available are non-station named location elements for sidings, works, depots, junction approaches and anything else that needs a name.  Location names are displayed on the railway in user-selectable font, style, size and colour.  In addition to named locations any other text may be added to the railway, again in any font, style, size and colour.  Location names and other text may be moved in order to improve the appearance of the railway and areas of track may be selected and cut, copied, pasted, deleted, mirrored and flipped etc.  Railway files may be saved and loaded in both development form during construction and in operational form on completion.  Track element lengths and line speed limits may be set individually, along tracks, or in areas by selection, as can preferred running directions.

Trains may operate to a timetable developed using the internal timetable editor, or under signaller control.  Three types of route are available:-

Automatic signal routes:  set signal to signal in preferred directions, the route is retained after trains pass and signals automatically return to green in stages as blocks ahead are cleared;

Preferred direction routes:  set signal to signal in preferred directions, and the route is cleared as trains pass; and

Unrestricted routes:  set from most types of track element to other track elements in any direction.  The route is cleared as trains pass.

In addition trains will run on track that has no route set, but they are then much more vulnerable to derailments and crashes.

Although the program is fully usable now, there remains great potential for further development.  Some ideas include:-

automatic route setting;
option to use & display imperial units as well as metric units;
ability to record and replay sessions;
signalbox mode where individual signals and points are operated directly, perhaps via a graphical lever frame, with user-defined interlocking;
sound effects, e.g. enter a track ID & hear trains & station announcements at that location;
random failures of trains, signals & points etc;
incorporation of user-defined graphics;
variable train lengths;
restricted routes - e.g. DC 3 & 4 rail, 25kV AC, tube lines etc; and
multi-player operation over the internet.
More recent suggestions are included in file DevHistory.txt.

'DeveloperGuide.doc' in the 'master' folder provides a summary of program history, structure, operation and interfaces, and also explains the file structure (click the document then select 'View Raw', and open with an application that will read Word 2003 files). Anyone wishing to contribute to further development should create a GitHub account then contact me with evidence of proficiency and I'll provide write access to the files. Version control is provided by either 'Git' or 'Subversion', so anyone wishing to contribute needs to be familiar with one or other of these systems - plenty of information is available on the web and it's not difficult to learn the basics.

I can be contacted at railwayfeedback@gmail.com or via the website at http://www.railwayoperationsimulator.com/
