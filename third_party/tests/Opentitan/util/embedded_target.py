#!/usr/bin/env python3
# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0
"""Script used to generate additional output files for Meson embedded targets.

This script takes the input ELF file generated by the build system, and
generates BIN and VMEM files. The BIN file is generated using objcopy, whereas
the VMEM file is generated using srec_cat.
"""
import argparse
import os
import subprocess
import sys


def run_objdump(objdump, input, basename, outdir):
    filename = basename + '.dis'
    output = os.path.join(outdir, filename)
    f = open(output, "w")
    cmd = [objdump, '-SDhl', input, ]
    subprocess.run(cmd, stdout=f, check=True)
    return output

def run_objcopy(objcopy, input, basename, outdir):
    filename = basename + '.bin'
    output = os.path.join(outdir, filename)
    cmd = [objcopy, '-O', 'binary', input, output]
    subprocess.run(cmd, check=True)
    return output


def run_srec_cat(srec_cat, input, basename, outdir):
    # TODO: Replace command for objcopy once this bug is fixed and released.
    # https://github.com/lowRISC/opentitan/issues/1107
    filename = basename + '.vmem'
    output = os.path.join(outdir, filename)
    cmd = [
        srec_cat, input, '-binary', '-offset', '0x0', '-byte-swap', '4', '-o',
        output, '-vmem'
    ]
    subprocess.run(cmd, check=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--basename',
        '-b',
        required=True,
        help='File basename used as preffix in output filenames.')
    parser.add_argument(
        '--input',
        '-i',
        required=True,
        help="Input file")
    parser.add_argument(
        '--objcopy',
        '-c',
        required=True,
        help='Absolute path to objcopy tool.')
    parser.add_argument(
        '--objdump',
        '-p',
        required=True,
        help='Absolute path to objdump tool.')
    parser.add_argument(
        '--outdir',
        '-d',
        required=True,
        help="Output directory.")
    parser.add_argument(
        '--srec_cat',
        '-s',
        required=True,
        help='Absolute path to srec_cat tool.')
    args = parser.parse_args()

    if not os.path.exists(args.outdir):
        os.makedirs(args.outdir)

    bin_file = run_objcopy(args.objcopy, args.input, args.basename, args.outdir)
    run_objdump(args.objdump, args.input, args.basename, args.outdir)
    run_srec_cat(args.srec_cat, bin_file, args.basename, args.outdir)


if __name__ == "__main__":
    main()