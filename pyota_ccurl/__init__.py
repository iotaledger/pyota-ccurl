# coding=utf-8
from __future__ import absolute_import, division, print_function, \
  unicode_literals


def is_installed():
  # type: () -> bool
  """
  Returns whether the C extension is installed correctly.
  """
  try:
    # noinspection PyUnresolvedReferences
    from ccurl import Curl as CCurl
  except ImportError:
    return False
  else:
    # noinspection PyUnresolvedReferences
    from iota.crypto import Curl
    return issubclass(Curl, CCurl)


def check_installation():
  """
  Outputs a message indicating whether the C extension is installed
  correctly.
  """
  print(
    '😸  CCurl is installed correctly!'
      if is_installed()
      else '😿  CCurl is NOT installed correctly!'
  )
  print('For support, visit the #iota-libs-pyota channel on the IOTA Slack.')
  print('https://slack.iota.org/')
