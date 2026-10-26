/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2026 Valerio Perticone
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "TarArchiverStrategy.h"
#include "Utility.h"

using namespace QComicBook;
using Utility::which;

// "ustar" marks POSIX and GNU tar archives, in the first header block
TarArchiverStrategy::TarArchiverStrategy()
    : ArchiverStrategy("tar", FileSignature(257, "ustar", 5))
{
}

TarArchiverStrategy::~TarArchiverStrategy()
{
}

void TarArchiverStrategy::configure()
{
    addExtension(".tar");
    addExtension(".cbt");
    setExecutables("tar");

    if (which("tar") != QString())
    {
        setExtractArguments("tar -xvf @F");
        setListArguments("tar -tf @F");
        setSupported();
    }
}
