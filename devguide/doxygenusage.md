Doxygen Setup
====================================

This project uses Doxygen to generate the HTML documentation files from the source files and a set of markdown files.

The following files and folders are significant to the Doxygen generation:

* ``Doxyfile`` -- the Doxygen configuration file, which controls which files are included and what the output looks like, among other things.
* ``autowiring/`` -- Autowiring header files.
* ``src/`` -- Autowiring source files.
* ``docs/`` -- the Doxygen output.
* ``examples/`` -- Examples of simple Autowiring programs, including a progressive tutorial series on the concepts of Autowiring.
* ``devguide/`` -- Dev Guide topics (as Markdown files) and some control files (defining groups and examples).

Generating the HTML docs
-----------------------------

You can use the command line or the Doxygen "Wizard." 

Using the command line, run the following from the autowiring root directory (containing the ``Doxyfile`` file):

    doxygen Doxyfile
    
This will give you the public API docs. 