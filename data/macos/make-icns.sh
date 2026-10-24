#!/bin/sh
# Builds a macOS .icns icon from a PNG image, using sips and iconutil
# (both part of macOS).
#
# usage: make-icns.sh <source.png> <output.icns>
set -e

src=$1
out=$2
tmp=$(mktemp -d "${TMPDIR:-/tmp}/qcomicbook-icns.XXXXXX")
iconset=$tmp/QComicBook.iconset
mkdir "$iconset"

for size in 16 32 128 256 512; do
    sips -z $size $size "$src" --out "$iconset/icon_${size}x${size}.png" > /dev/null
    sips -z $((size * 2)) $((size * 2)) "$src" --out "$iconset/icon_${size}x${size}@2x.png" > /dev/null
done

iconutil -c icns "$iconset" -o "$out"
# only ever remove the temporary directory created above
case "$tmp" in
    */qcomicbook-icns.??????) rm -rf -- "$tmp" ;;
esac
