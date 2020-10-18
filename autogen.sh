# This file is part of SmallBASIC
#
# Copyright(C) 2001-2020 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#

git submodule init
git submodule update

rm -rf autom4te.cache aclocal.m4
mkdir -p m4
touch NEWS README AUTHORS ChangeLog
autoreconf -vfi
