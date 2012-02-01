least
=====

A less sophisticated terminal pager than less.

Building
--------

All you need is a C compiler and make. Then execute:

    make

Usage
-----

You can use least to display a file _foo_ like this:

    least foo

Unlike less, it does not read from stdin yet.

This is how you control least:

<table>
    <tr><th>Key</th><th>Action</th></tr>
    <tr><td>q</td><td>Exit least</td></tr>
    <tr><td>up arrow</td><td>Scroll one line up</td></tr>
    <tr><td>down arrow</td><td>Scroll one line down</td></tr>
    <tr><td>page up</td><td>Scroll one page up</td></tr>
    <tr><td>page down</td><td>Scroll one page down</td></tr>
    <tr><td>home</td><td>Scroll to the top</td></tr>
    <tr><td>end</td><td>Scroll to the bottom</td></tr>
</table>

Vi and Emacs bindings to come.
