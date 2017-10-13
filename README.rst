C Curl Extension for PyOTA
==========================
This is an extension for the crypto functionality in `PyOTA <https://pypi.python.org/pypi/PyOTA>`_, the Python API
for IOTA.  When installed, it will significantly boost the performance of
PyOTA's crypto functionality.

This extension is installed as an add-on to the ``pyota`` package::

   pip install pyota[ccurl]

After running the above command, you can verify that the C extension is
installed correctly by running the ``check_ccurl`` command::

   > check_ccurl
   😸  CCurl is installed correctly!
   For support, visit the #iota-libs-pyota channel on the IOTA Slack.
   https://slack.iota.org/

Compatibility
-------------
This extension is compatible with Python 3.6, 3.5 and 2.7.

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
