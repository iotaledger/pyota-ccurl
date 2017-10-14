# coding=utf-8
from __future__ import absolute_import, division, print_function, \
  unicode_literals

from unittest import TestCase

from pyota_ccurl import is_installed


class IsInstalledTestCase(TestCase):
  def test_is_installed(self):
    """
    Verify that the C extension is installed correctly.
    """
    self.assertTrue(is_installed())
