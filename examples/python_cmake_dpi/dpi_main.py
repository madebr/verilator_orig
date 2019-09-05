#!/usr/bin/env python3

# DESCRIPTION: Verilator: Verilog example python module using DPI functions
#
# This file ONLY is placed into the Public Domain, for any use,
# without warranty, 2019 by Wilson Snyder.
#======================================================================

import ctypes
import os
import sys
import time

# Search the build directory
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), "build"))

import Vtop


class MyDpi(Vtop.Vtop.DpiMetaClass):

    didDump = False

    def __init__(self):
        Vtop.Vtop.DpiMetaClass.__init__(self)

    def __del__(self):
        print('MyDpi destructor')

    @classmethod
    def dpii_f_bit(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_f_bit", i, res)
        return res

    @classmethod
    def dpii_f_bit8(cls, i: Vtop.svBitVecVal):
        res = ~i
        return res

    @classmethod
    def dpii_f_bit9(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_f_bit9", i, res)
        return res

    @classmethod
    def dpii_f_bit16(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_f_bit16", i, res)
        return res

    @classmethod
    def dpii_f_bit17(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_f_bit17", i, res)
        return res

    @classmethod
    def dpii_f_bit32(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_f_bit32", i, res)
        return res

    @classmethod
    def dpii_f_bit33(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_f_bit33", i, res)
        return res

    @classmethod
    def dpii_f_bit64(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_f_bit64", i, res)
        return res

    @classmethod
    def dpii_f_int(cls, i: int):
        res = ~i
        print("in dpii_f_int {0} ({0:x}) -> {1} ({1:x})".format(i, res))
        return res

    @classmethod
    def dpii_f_byte(cls, i: int):
        res = ~i
        print("in dpii_f_byte {0} ({0:x}) -> {1} ({1:x})".format(i, res))
        return res

    @classmethod
    def dpii_f_shortint(cls, i: int):
        res = ~i
        print("in dpii_f_shortint {0} ({0:x}) -> {1} ({1:x})".format(i, res))
        return res

    @classmethod
    def dpii_f_longint(cls, i: int):
        res = ~i
        print("in dpii_f_longint {0} ({0:x}) -> {1} ({1:x})".format(i, res))
        return res

    @classmethod
    def dpii_f_chandle(cls, i: 'ctypes.c_void_p'):
        res = i
        print("in dpii_f_chandle {0} -> {1}".format(i, res))
        return res

    @classmethod
    def dpii_f_string(cls, i: str):
        res = i
        print("in dpii_f_string {0!r} -> {1!r}".format(i, res))
        return res

    @classmethod
    def dpii_f_real(cls, i: float):
        res = i + 1.5
        print("in dpii_f_real {0!r} -> {1!r}".format(i, res))
        return res

    @classmethod
    def dpii_v_bit(cls, i: Vtop.svBitVecVal):
        res = Vtop.svBitVecVal(i)
        res[0] = not i[0]
        print("in dpii_v_bit", i, res)
        return res

    @classmethod
    def dpii_v_int(cls, i: int):
        res = ~i
        print("in dpii_v_int", i, res)
        return res

    @classmethod
    def dpii_v_byte(cls, i: int):
        res = ~i
        print("in dpii_v_byte", i, res)
        return res

    @classmethod
    def dpii_v_shortint(cls, i: int):
        res = ~i
        print("in dpii_v_shortint", i, res)
        return res

    @classmethod
    def dpii_v_longint(cls, i: int):
        res = ~i
        print("in dpii_v_longint", i, res)
        return res

    @classmethod
    def dpii_v_uint(cls, i: int):
        res = i ^ 0Xffff_ffff
        print("in dpii_v_uint", i, res)
        return res

    @classmethod
    def dpii_v_ushort(cls, i: int):
        res = i ^ 0xffff
        print("in dpii_v_ushort", i, res)
        return res

    @classmethod
    def dpii_v_ulong(cls, i: int):
        res = i ^ 0xffff_ffff_ffff_ffff
        print("in dpii_v_ulong", i, res)
        return res

    @classmethod
    def dpii_v_struct(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_v_struct", i, res)
        return res

    @classmethod
    def dpii_v_substruct(cls, i: Vtop.svBitVecVal):
        # FIXME: pass structs as object
        a1 = i[:len(i)//2]
        a2 = i[len(i)//2:]
        res = int(a2) - int(a1)
        print("in dpii_v_substruct", i, res)
        return res

    @classmethod
    def dpii_v_chandle(cls, i: 'ctypes.c_void_p'):
        res = i
        print("in dpii_v_chandle", i, res)
        return res

    @classmethod
    def dpii_v_string(cls, i: str):
        res = i
        print("in dpii_v_string", i, res)
        return res

    @classmethod
    def dpii_v_real(cls, i: float):
        res = i + 1.5
        print("in dpii_v_real", i, res)
        return res

    @classmethod
    def dpii_v_bit64(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_v_bit64", i, res)
        return res

    @classmethod
    def dpii_v_bit95(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_v_bit95", i, res)
        return res

    @classmethod
    def dpii_v_bit96(cls, i: Vtop.svBitVecVal):
        res = ~i
        print("in dpii_v_bit96", i, res)
        return res

    @classmethod
    def dpii_v_reg(cls, i: Vtop.svLogicVecVal):
        res = ~i
        print("in dpii_v_reg", i, res)
        return res

    @classmethod
    def dpii_v_reg15(cls, i: Vtop.svLogicVecVal):
        res = ~i
        print("in dpii_v_reg15", i, res)
        return res

    @classmethod
    def dpii_v_reg95(cls, i: Vtop.svLogicVecVal):
        res = ~i
        print("in dpii_v_reg95", i, res)
        return res

    @classmethod
    def dpii_v_integer(cls, i: Vtop.svLogicVecVal):
        res = ~i
        print("in dpii_v_integer", i, res)
        return res

    @classmethod
    def dpii_v_time(cls, i: Vtop.svLogicVecVal):
        res = ~i
        print("in dpii_v_time", i, res)
        return res

    @classmethod
    def dpii_f_strlen(cls, i: str):
        res = len(i);
        print("in dpii_f_strlen", repr(i), res)
        return res

    @classmethod
    def dpii_f_void(cls):
        print("in dpii_f_void")

    @classmethod
    def dpii_t_void(cls):
        print("in dpii_t_void")
        try:
            print('callerInfo', cls.get_caller_info())
            assert False
        except TypeError:
            pass
        try:
            print('scope', cls.get_current_scope())
            assert False
        except TypeError:
            pass
        try:
            print('instance', cls.get_instance())
            assert False
        except TypeError:
            pass
        try:
            scope = Vtop.svScope.from_name('Vtest.top')
            print('found scope using from_name', scope, scope.name)
        except RuntimeError:
            assert False
        return 0

    def dpii_t_void_context(self):
        res = 0
        print("in dpii_t_void_context", res)
        print('callerInfo', self.get_caller_info())
        scope = self.get_current_scope()
        print('scope', scope, scope.name)
        print('instance', self.get_instance())
        return res

    @classmethod
    def dpii_t_int(cls, i):
        res = 0, i ^ 0xffff_ffff
        print("in dpii_t_int", i, res)
        return res

    @classmethod
    def dpii_fa_bit(cls, i):
        res = i ^ 0xffff_ffff
        print("in dpii_fa_bit", i, res)
        return res

    def dpix_run_tests(self):
        print("dpix_run_tests:")
        if not self.didDump:
            Vtop.Verilated.dump_internals()
            self.didDump = True

        dpi_version = self.get_dpi_version()
        print('version:', repr(dpi_version))
        assert '1800-2005' in dpi_version

        dpix_int123 = self.dpix_int123()
        assert isinstance(dpix_int123, int)
        assert dpix_int123 == 0x123

        dpix_t_int = self.dpix_t_int(0x456)
        assert isinstance(dpix_t_int, int)
        assert dpix_t_int == ~0x456

        dpix_t_renamed = self.dpix_t_renamed(0x456)
        assert isinstance(dpix_t_renamed, int)
        assert dpix_t_renamed == 0x456+2

        dpix_f_bit = self.dpix_f_bit(Vtop.svBitVecVal('1'))
        assert isinstance(dpix_f_bit, Vtop.svBitVecVal)
        assert dpix_f_bit == Vtop.svBitVecVal('0')

        dpix_f_bit2 = self.dpix_f_bit(Vtop.svBitVecVal('0'))
        assert isinstance(dpix_f_bit2, Vtop.svBitVecVal)
        assert dpix_f_bit2 == Vtop.svBitVecVal('1')

        #FIXME: fix this!!!
        # dpix_f_bit15_input = Vtop.svBitVecVal('000000000010000')
        # dpix_f_bit15 = self.dpix_f_bit15(dpix_f_bit15_input)
        # assert isinstance(dpix_f_bit15, Vtop.svBitVecVal)
        # assert dpix_f_bit15 == ~dpix_f_bit15_input

        dpix_f_int = self.dpix_f_int(1)
        assert isinstance(dpix_f_int, int)
        assert dpix_f_int == ~1

        dpix_f_byte = self.dpix_f_byte(1)
        assert isinstance(dpix_f_byte, int)
        assert dpix_f_byte == ~1

        dpix_f_shortint = self.dpix_f_shortint(1)
        assert isinstance(dpix_f_shortint, int)
        assert dpix_f_shortint == ~1

        dpix_f_longint = self.dpix_f_longint(1)
        assert isinstance(dpix_f_longint, int)
        assert dpix_f_longint == ~1

        dpix_f_chandle = self.dpix_f_chandle(ctypes.c_void_p(12345))
        assert isinstance(dpix_f_chandle, ctypes.c_void_p)
        assert dpix_f_chandle.value == ctypes.c_void_p(12345).value

        val = 0x8a413bd9ab782a12f2912312
        binval = bin(val)[2:]

        dpix_t_bit48 = self.dpix_t_bit48(Vtop.svBitVecVal(binval[:48]))
        assert isinstance(dpix_t_bit48, Vtop.svBitVecVal)
        assert dpix_t_bit48 == ~Vtop.svBitVecVal(binval[:48])

        dpix_t_bit95 = self.dpix_t_bit95(Vtop.svBitVecVal(binval[:95]))
        assert isinstance(dpix_t_bit95, Vtop.svBitVecVal)
        assert dpix_t_bit95 == ~Vtop.svBitVecVal(binval[:95])

        dpix_t_bit96 = self.dpix_t_bit96(Vtop.svBitVecVal(binval[:96]))
        assert isinstance(dpix_t_bit96, Vtop.svBitVecVal)
        assert dpix_t_bit96 == ~Vtop.svBitVecVal(binval[:96])

        dpix_t_reg = self.dpix_t_reg(Vtop.svLogicVecVal('0'))
        assert isinstance(dpix_t_reg, Vtop.svLogicVecVal)
        assert dpix_t_reg == Vtop.svLogicVecVal('1')

        dpix_t_reg2 = self.dpix_t_reg(Vtop.svLogicVecVal('1'))
        assert isinstance(dpix_t_reg2, Vtop.svLogicVecVal)
        assert dpix_t_reg2 == Vtop.svLogicVecVal('0')

        dpix_t_reg15 = self.dpix_t_reg15(Vtop.svLogicVecVal(binval[:15]))
        assert isinstance(dpix_t_reg15, Vtop.svLogicVecVal)
        assert dpix_t_reg15 == ~Vtop.svLogicVecVal(binval[:15])

        dpix_t_reg95 = self.dpix_t_reg95(Vtop.svLogicVecVal(binval[:95]))
        assert isinstance(dpix_t_reg95, Vtop.svLogicVecVal)
        assert dpix_t_reg95 == ~Vtop.svLogicVecVal(binval[:95])

        w = 64 if sys.maxsize > 2**32 else 32
        dpix_t_time = self.dpix_t_time(Vtop.svLogicVecVal(binval[:w]))
        assert isinstance(dpix_t_time, Vtop.svLogicVecVal)
        assert dpix_t_time == ~Vtop.svLogicVecVal(binval[:w])

        self.check_sub('Vtest.top.a', 1)
        self.check_sub('Vtest.top.b', 2)

        return -1

    def check_sub(self, scopeName, i):
        scope = Vtop.svScope.from_name(scopeName)
        assert isinstance(scope, Vtop.svScope)
        print('svGetScopeFromName({!r}) -> {!r}'.format(scopeName, scope))
        currentScope = self.get_current_scope()
        assert isinstance(currentScope, Vtop.svScope)
        prevScope = self.set_current_scope(scope)
        assert isinstance(prevScope, Vtop.svScope)
        assert currentScope == prevScope

        assert scope == self.get_current_scope()

        dpix_sub_inst = self.dpix_sub_inst(100 * i)
        assert isinstance(dpix_sub_inst, int)
        assert dpix_sub_inst == 100*i+i


def main():
    # See a similar example walkthrough in the verilator manpage.
    Vtop.Verilated.parse_arguments(sys.argv[1:])
    Vtop.set_time_stamp(time.time())

    Vtop.Vtop.set_dpi_functions(MyDpi)

    # This is intended to be a minimal example.  Before copying this to start a
    # real project, it is better to start with a more complete example,
    # e.g. examples/c_tracing.

    # print('before construction', Vtop.svScope.from_name('Vtop.top'))
    # Construct the Verilated model
    top = Vtop.Vtop("Vtest")

    # Simulate until $finish
    while not Vtop.Verilated.finished:
        top.eval()

    # Final model cleanup
    top.final()

    # Destroy model
    del top

    # Fin
    sys.exit(0);


if __name__ == "__main__":
    main()
    print('finished!')
