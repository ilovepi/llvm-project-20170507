"""
Test how lldb reacts to wrong commands
"""

from __future__ import print_function

import os
import time
import lldb
from lldbsuite.test.decorators import *
from lldbsuite.test.lldbtest import *
from lldbsuite.test import lldbutil

class UnknownCommandTestCase(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    @no_debug_info_test
    def test_ambiguous_command(self):
        command_interpreter = self.dbg.GetCommandInterpreter()
        self.assertTrue(command_interpreter, VALID_COMMAND_INTERPRETER)
        result = lldb.SBCommandReturnObject()

        command_interpreter.HandleCommand("g", result)
        self.assertFalse(result.Succeeded())
        self.assertRegexpMatches(result.GetError(), "Ambiguous command 'g'. Possible matches:")
        self.assertRegexpMatches(result.GetError(), "gui")
        self.assertRegexpMatches(result.GetError(), "gdb-remote")
        # FIXME: Somehow we get 'gui' and 'gdb-remote' twice in the output.

    @no_debug_info_test
    def test_unknown_command(self):
        command_interpreter = self.dbg.GetCommandInterpreter()
        self.assertTrue(command_interpreter, VALID_COMMAND_INTERPRETER)
        result = lldb.SBCommandReturnObject()

        command_interpreter.HandleCommand("qbert", result)
        self.assertFalse(result.Succeeded())
        self.assertEquals(result.GetError(), "error: 'qbert' is not a valid command.\n")
