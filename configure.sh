#!/bin/sh

mkdir -p gen
python3 -B ./share/build/make.py generate
python3 -B ./share/build/make.py make
python3 -B ./share/build/make.py ninja

