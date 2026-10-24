#!/bin/sh
# Makes QComicBook.app self-contained and packs it into a .dmg disk image.
#
# usage: make-dmg.sh <build dir> [output dir]
#
# Needs macdeployqt and qtpaths from the Qt used for the build in PATH
# (Homebrew's qtbase links them into "$(brew --prefix)/bin"). The image is
# written to the output directory (default: the build directory) as
# QComicBook-<version>-macOS-<arch>.dmg.
set -e

build=$1
out=${2:-$build}
app=$build/src/QComicBook.app

if [ ! -d "$app" ]; then
    echo "error: $app not found, build QComicBook first" >&2
    exit 1
fi

version=$(/usr/libexec/PlistBuddy -c "Print CFBundleShortVersionString" "$app/Contents/Info.plist")
dmg=$out/QComicBook-$version-macOS-$(uname -m).dmg

# copy the Qt frameworks and plugins, Poppler and their dependencies into
# the bundle, and point the binaries at the copies
macdeployqt "$app"

# load @rpath libraries (e.g. libpoppler, used by libpoppler-qt6) from the
# bundle, and never from the build machine's library directories
exe=$app/Contents/MacOS/QComicBook
rpaths=$(otool -l "$exe" | awk '/cmd LC_RPATH/ { getline; getline; print $2 }')
for rpath in $rpaths; do
    case "$rpath" in
        /*) install_name_tool -delete_rpath "$rpath" "$exe" ;;
    esac
done
case " $(echo $rpaths) " in
    *" @executable_path/../Frameworks "*) ;;
    *) install_name_tool -add_rpath @executable_path/../Frameworks "$exe" ;;
esac

# macdeployqt also copies plugins of Qt modules QComicBook doesn't use (e.g.
# QtSvg, QtPdf) without their frameworks: remove the ones that can't load
for plugin in "$app"/Contents/PlugIns/*/*.dylib; do
    for framework in $(otool -L "$plugin" | awk '{ print $1 }' | sed -n 's|^@rpath/\([^/]*\.framework\)/.*|\1|p'); do
        if [ ! -d "$app/Contents/Frameworks/$framework" ]; then
            echo "removing $plugin (needs $framework)"
            rm -f "$plugin"
            break
        fi
    done
done

# some copied libraries keep their build machine path as install name
for lib in "$app"/Contents/Frameworks/*.dylib; do
    case "$(otool -D "$lib" | sed -n 2p)" in
        /opt/homebrew/*|/usr/local/*) install_name_tool -id "@executable_path/../Frameworks/$(basename "$lib")" "$lib" ;;
    esac
done

# the bundle must not depend on anything outside it but macOS itself
leftovers=$(find "$app/Contents" -type f \( -perm -u+x -o -name '*.dylib' \) -exec otool -L {} \; 2>/dev/null \
    | grep -E '^[[:space:]]+(/opt/homebrew|/usr/local)/' || true)
if [ -n "$leftovers" ]; then
    echo "error: the bundle still links libraries outside it:" >&2
    echo "$leftovers" >&2
    exit 1
fi

# Qt's own translations (standard dialogs and buttons): the qt_<lang>.qm
# catalogs load qtbase_<lang>.qm and qtmultimedia_<lang>.qm, and fail if
# those are missing. qt.conf paths are relative to the bundle's Contents
qt_translations=$(qtpaths --query QT_INSTALL_TRANSLATIONS)
translations=$app/Contents/Resources/translations
mkdir -p "$translations"
cp "$qt_translations"/qt_*.qm "$qt_translations"/qtbase_*.qm "$qt_translations"/qtmultimedia_*.qm "$translations"
qtconf=$app/Contents/Resources/qt.conf
if [ ! -f "$qtconf" ]; then
    printf '[Paths]\n' > "$qtconf"
fi
printf 'Translations = Resources/translations\n' >> "$qtconf"

# the app needs the newest macOS any of its binaries (e.g. Homebrew's
# libraries) was built for: declare it, so older systems refuse it cleanly
minos=$(find "$app/Contents" -type f \( -perm -u+x -o -name '*.dylib' \) -exec otool -l {} \; 2>/dev/null \
    | awk '/cmd LC_BUILD_VERSION/ { f = 1 } f && $1 == "minos" { print $2; f = 0 }' \
    | sort -t. -k1,1n -k2,2n -k3,3n | tail -1)
plutil -replace LSMinimumSystemVersion -string "$minos" "$app/Contents/Info.plist"

# macdeployqt changes the binaries, which breaks their signatures: sign
# everything again with an ad-hoc signature (required on Apple silicon)
codesign --force --deep --sign - "$app"
codesign --verify --deep --strict "$app"

# disk image with the app and a link to /Applications, to install by drag and drop
staging=$(mktemp -d "${TMPDIR:-/tmp}/qcomicbook-dmg.XXXXXX")
cp -R "$app" "$staging/"
ln -s /Applications "$staging/Applications"
hdiutil create -volname QComicBook -srcfolder "$staging" -ov -format UDZO "$dmg"
# only ever remove the temporary directory created above
case "$staging" in
    */qcomicbook-dmg.??????) rm -rf -- "$staging" ;;
esac

echo "$dmg"
