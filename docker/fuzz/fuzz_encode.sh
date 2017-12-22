#!/bin/bash
#
# Script to run afl-fuzz on `oftr encode` with yaml input.

AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 afl-fuzz -i encode_in -o encode_out -- ./afl-oftr encode @@
