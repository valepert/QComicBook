# QComicBook

A Qt 6 viewer for comic book archives and PDF files. This repository is a fork
of the unmaintained stolowski/QComicBook; work goes to `valepert/QComicBook`,
never upstream.

## Build

Requirements: CMake 3.16 or later, a C++17 compiler, Qt 6.4 or later
(Core, Widgets, PrintSupport, LinguistTools) and poppler-qt6.

Linux (Debian/Ubuntu package names):

    sudo apt install build-essential cmake pkg-config qt6-base-dev qt6-tools-dev \
        qt6-tools-dev-tools qt6-l10n-tools libpoppler-qt6-dev
    cmake -S . -B build
    cmake --build build -j"$(nproc)"

macOS (Homebrew):

    brew install pkgconf qtbase qttools qttranslations poppler-qt6
    export PKG_CONFIG_PATH="$(brew --prefix poppler-qt6)/lib/pkgconfig"
    cmake -S . -B build
    cmake --build build -j"$(sysctl -n hw.ncpu)"
    sh data/macos/make-dmg.sh build    # self-contained app and .dmg

The binary is `build/src/qcomicbook` on Linux and
`build/src/QComicBook.app/Contents/MacOS/QComicBook` on macOS.

- Sources are collected with `GLOB_RECURSE` in `src/CMakeLists.txt`: rerun
  cmake after adding or removing a file.
- `-DCMAKE_BUILD_TYPE=Debug` defines `DEBUG`, which adds the Debug menu and
  debug output. Other build types define `QT_NO_DEBUG_OUTPUT`.
- The build has no compiler warnings; keep it that way.

## Code layout

- `src/` the app. `ComicMainWindow` is the main window; `ComicBookSettings`
  reads and writes the settings.
- `src/Sink/` page sources: folders of images, archives and PDF files
  (`ImgSinkFactory` picks one by path).
- `src/Archivers/` one `*ArchiverStrategy` per archive format, wrapping an
  external tool (unzip, rar/unrar, 7zz/7z, tar, unace). They are registered in
  `ArchiversConfiguration.cpp`; files are recognized by extension and by the
  signature in `FileSignature`.
- `src/FrameDetect/` frame detection for the Frame view.
- `data/` icons, `qcomicbook.desktop`, and `data/macos/` (Info.plist, dmg script).
- `i18n/` translations (`.ts`, compiled to `.qm` by the build); `help/qcomicbook.1`
  the man page.

## Tests

There is no unit test suite. Checks run in GitHub Actions:

- `.github/workflows/linux.yml` builds on Ubuntu 24.04 (Qt 6.4, the minimum)
  and 26.04, checks the installed files and translations, and runs headless
  smoke tests (`QT_QPA_PLATFORM=offscreen`) on a folder of images, a PDF and a
  `.cbt`.
- `.github/workflows/macos.yml` builds on macOS 15, opens sample `.cbr`,
  `.cb7` and `.cbt` comics, and builds the `.dmg`.

The smoke tests start the app with a comic, wait a few seconds, check that it
is still running, and count the pages extracted into `TMPDIR`. Run the same
steps locally when changing archive or page loading code. Test comics come from
outside the repo; don't commit sample files.

After changing `data/qcomicbook.desktop`, check it with `desktop-file-validate`
(from desktop-file-utils); it must print nothing.

## Conventions

- One goal per pull request, against `master` of `valepert/QComicBook`, split
  into logical commits. `master` is protected: changes go through a PR, and the
  checks `build (ubuntu-24.04)`, `build (ubuntu-26.04)` and `dmg` must pass.
- Commit subjects are short and imperative ("Open CBT comics with tar"); the
  body explains why.
- Documentation (README.md, the man page, ChangeLog) is updated with small
  edits in its existing style. A rename or format change goes in its own commit.
- New user-visible strings use `tr()`.

## Releases

Tags are plain `X.Y.Z` (no `v`). For a release:

1. Set `VERSION` in the top-level `CMakeLists.txt` and add a ChangeLog entry
   (`<date>:  <version>  -- ...`) with no empty lines inside it.
2. After the PR is merged, tag `master` with the same version and push the tag.

The macOS workflow then checks that the tag matches `VERSION` and is on
`master`, and creates a draft GitHub Release with the `.dmg` and the ChangeLog
entry as notes.
