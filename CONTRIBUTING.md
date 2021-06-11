# Contributing to railway-dot-exe

:+1::tada: First off, thanks for taking the time to contribute! :tada::+1:

The following is a set of guidelines for contributing to railway-dot-exe.

#### Table Of Contents

[Code of Conduct](#code-of-conduct)

[I don't want to read this whole thing, I just have a question!!!](#i-dont-want-to-read-this-whole-thing-i-just-have-a-question)

[What should I know before I get started as a contributor?](#what-should-i-know-before-i-get-started-as-a-contributor)
  * [Railway-dot-exe and C++](#railway-dot-exe-and-c++)

[How Can I Contribute?](#how-can-i-contribute)
  * [Reporting Bugs](#reporting-bugs)
  * [Pull Requests](#pull-requests)
  * [Suggested Enhancements](#suggested-enhancements)

[Styleguides](#styleguides)
  * [Git Commit Messages](#git-commit-messages)
  * [Editing the Developer Guide](#editing-the-developer-guide)

[Attributions](#attributions)

## Code of Conduct

Be nice to each other. Please report unacceptable behaviour to [railwayfeedback@gmail.com](mailto:railwayfeedback@gmail.com).

## I don't want to read this whole thing I just have a question!!!

We have an official [website](http://www.railwayoperationsimulator.com) where you can ask questions via the [Contact](http://www.railwayoperationsimulator.com/contact) form, or simply [Email us](mailto:railwayfeedback@gmail.com).
Better still join the Discord community via the Discord tab on the [website] (http://www.railwayoperationsimulator.com) and ask the question in the 'software-development' channel.

## What should I know before I get started as a contributor?

### Railway-dot-exe and C++

Railway-dot-exe is a large open source project &mdash;  and it's written in C++. In order to make changes you will need to have some competence in C++ programming. If you want to experiment or learn C++ using railway-dot-exe by all means please do &mdash; but fork your own copy of the project. Once you feel confident with your level of experience you can submit a pull-request via GitHub. Many people use the software, and have for many years, so you don't want to break things on the master repository because you're learning or trying new things. Once we see evidence of proficiency and are confident that your changes work and are beneficial then we'll pull them into the master. In time when you're keen to play a significant part in further development and have the skills, we'll add you as a collaborator.

The program is written using Embarcardero's C++ Builder Community Edition. As such, it's only able to produce a win32 version of the program. This will happily run on your 64bit Windows machine though.  

Simply visit Embarcadero's site to download the free C++ Builder Community Edition. Then `git clone` or 'fork' our repository and open the railway.cbproj file in C++ Builder.

* [Embarcadero C++ Community Edition](https://www.embarcadero.com/products/cbuilder/starter)

A brief description of the program is provided in 'DeveloperGuide.pdf', but the main documentation along with extensive diagrams is provided by Doxygen in the 'docs' folder. It's written in html and the starting point is index.html.

## How Can I Contribute?

### Reporting Bugs

Use the Issues tab on our project page to report a bug. See the guide [GitHub issues](https://guides.github.com/features/issues/) if you need help with creating an Issue .

#### How Do I Submit A (Good) Bug Report?

Explain the problem and include additional details to help maintainers reproduce the problem:

* **Use a clear and descriptive title** for the issue to identify the problem.
* **Describe the exact steps which reproduce the problem** in as much detail as possible.
* **Explain which behavior you expected to see instead and why.**
* **Include screenshots if possible**.

Include details about your environment:

* **Which version of railway-dot-exe are you using?** You can get the exact version by opening the About form using the Help menu.
* **What's the name and version of the OS you are using**?
* **Are you running railway-dot-exe in a virtual machine?** If so, which VM software are you using and which operating systems and versions are used for the host and the guest?

Optionally, provide more context by answering these questions:

* **Did the problem start happening recently** (e.g. after updating to a new version of railway-dot-exe) or was this always a problem?
* If the problem started happening recently, **can you reproduce the problem in an older version of railway-dot-exe?** What's the most recent version in which the problem doesn't happen? You can download older versions of railway-dot-exe from the official site's download page link to [earlier versions](https://www.dropbox.com/sh/wvruss55cfzdvgw/AAApyZeGaIRyJAtS6clOuo0La?dl=0).
* **Can you reliably reproduce the issue?** If not, provide details about how often the problem happens and under which conditions it normally happens.

### Pull Requests

* There is no template for pull requests at this time. Simply create your pull request and try to include a meaningful description of what you are proposing. 

### Suggested Enhancements

Although the program is fully usable now, there remains great potential for further development. Some ideas include:

* automatic route setting
* option to use & display imperial units as well as metric units
* ability to record and replay sessions
* addition of automatic signal routes over level crossings, with train-triggered crossing opening 
* multi-player operation over the internet
* signalbox mode where individual signals and points are operated directly, perhaps via a graphical lever frame, with user-defined interlocking
* sound effects, e.g. enter a track ID & hear trains & station announcements at that location
* random failures of signals & points etc, and random delay times at stations
* variable train lengths
* restricted routes – e.g. DC 3 & 4 rail, 25kV AC, tube lines and so on
* etc. etc.

So… there’s plenty still to do… what are you waiting for?

## Styleguides

### Git Commit Messages

* Limit the first line to 72 characters or less
* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move icon to..." not "Moves icon to...")
* A properly formatted commit subject line should always be able to complete the following sentence:
    * This commit will __your commit message here__

## Attributions

Menu icons from Silk Icon Set 1.3 by Mark James used under Creative Commons Attribution 2.5 License
 http://creativecommons.org/licenses/by/2.5/
 
Timetable editor icons from https://icons8.com used under Creative Commons Attribution-NoDerivs 3.0 Unported License
http://creativecommons.org/licenses/by-nd/3.0/

