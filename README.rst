C Curl Extension for PyOTA
==========================
This is an extension for the crypto functionality in `PyOTA <https://pypi.python.org/pypi/PyOTA>`_, the Python API
for IOTA.  When installed, it will significantly boost the performance of
PyOTA's crypto functionality.

This extension is installed as an add-on to the ``pyota`` package::

   pip install pyota[ccurl]

Compatibility
-------------
This extension is currently compatible with Python 3.6 and 3.5 only.

`I've documented the issues with the Python 2 C API <https://github.com/todofixthis/pyota-ccurl/issues/4>`_.
Adding Python 2 support is on my list, but it may take me a bit before I can get to it.

If you have experience with C and are interested in getting involved,
please reach out to me on the `IOTA Slack network <https://slack.iota.org/>`_ or submit a pull request.

Testing
-------
The easiest way to test the extension is to install it and then run the PyOTA unit tests.

Here's a modified ``tox.ini`` file that for PyOTA that will install the C extension before running unit tests::

   [tox]
   envlist = py27, py35, py36

   [testenv]
   commands =
     pip install --pre pyota-ccurl
     nosetests
   deps =
     mock
     nose

Note that this file needs to be installed in the PyOTA codebase!
