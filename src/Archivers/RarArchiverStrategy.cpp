/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "RarArchiverStrategy.h"
#include "ArchiversConfiguration.h"
#include "Utility.h"
#include <QTextStream>
#include <QString>
#include <QRegularExpression>

using namespace QComicBook;
using Utility::which;

RarArchiverStrategy::RarArchiverStrategy()
    : ArchiverStrategy("rar", FileSignature(0, "\x52\x61\x72\x21", 4)), free_unrar(false)
{
}

RarArchiverStrategy::~RarArchiverStrategy()
{
}

bool RarArchiverStrategy::isNonfreeUnrar()
{
    FILE *f;
    bool nonfree = false;
    //
    // determine which unrar it is - free or non-free
    if ((f = popen("unrar", "r")) != NULL)
    {
        QRegularExpression regexp("^UNRAR.+freeware");
        for (QTextStream s(f); !s.atEnd(); )
        {
            if (regexp.match(s.readLine()).hasMatch())
            {
                nonfree = true;
                break;
            }
        }
        pclose(f);
    }
    return nonfree;
}

void RarArchiverStrategy::configure()
{
    addExtension(".rar");
    addExtension(".cbr");
    setExecutables("rar", "unrar", "7zz");

    const bool unrar = which("unrar") != QString();

    if (which("rar") != QString())
    {
        setExtractArguments("rar x @F");
        setListArguments("rar lb @F");
        setSupported();
    }
    else if (unrar && isNonfreeUnrar())
    {
        setExtractArguments("unrar x @F");
        setListArguments("unrar lb @F");
        setSupported();
    }
    else if (which("7zz") != QString()) // 7-Zip, e.g. Homebrew's sevenzip on macOS
    {
        setExtractArguments("7zz x @F");
        setListArguments("7zz l @F");
        setSupported();
    }
    else if (unrar)
    {
        setExtractArguments("unrar -x @F");
        setListArguments("unrar -t @F");
        setSupported();
        free_unrar = true;
    }
    else if (which("unrar-free") != QString()) //some distros rename free unrar like this
    {
        setExtractArguments("unrar-free -x @F");
        setListArguments("unrar-free -t @F");
        setSupported();
        free_unrar = true;
    }
}

QList<ArchiverHint> RarArchiverStrategy::getHints() const
{
    QList<ArchiverHint> hints;
    if (free_unrar)
    {
        hints.append(ArchiverHint(
                         ArchiversConfiguration::tr("Free (opensource) version of unrar was detected. "
                                                    "This version has problems with many rar archives. "
                                                    "It is recommended to install the non-free unrar and restart QComicBook."),
                         ArchiverHint::Warning));
    }
    return hints;
}
