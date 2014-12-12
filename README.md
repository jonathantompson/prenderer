**prenderer - A Pretty OpenGL Renderer**
---------
---------

**Overview**
--------

This application is escentially a usage example for the OpenGL deferred renderer of my jtil library.  Some features include.

- Fully deferred lighting pipeline.
- Motion blur
- Screen space ambient occlusion
- Various culling optimizations
- Parallel-split variance shadow mapping
- Depth of field blur
- Displacement mapping tessellation pipeline
- FXAA post processing anti-aliasing

**Compilation**
---------------

Building prenderer uses Visual Studio 2012 on Windows, and cmake + gcc 4.7 (or greater) on Mac OS X.  The only real dependancy is the jtil library.  See <https://github.com/jonathantompson/jtil> for more details.

VS2012 and cmake expect a specific directory structure:

- \\include\\WIN\\
- \\include\\MAC\_OS\_X\\
- \\lib\\WIN\\
- \\lib\\MAC\_OS\_X\\
- \\jtil\\
- \\prenderer\\

So the dependancy headers and static libraries (.lib on Windows and .a on Mac OS X) are separated by OS and exist in directories at the same level as jtil and prenderer.  I have pre-compiled the dependencies and put them in dropbox, let me know if you need the link.

**Style**
---------

This project follows the Google C++ style conventions: 

<http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml>

**License**
-----------
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
