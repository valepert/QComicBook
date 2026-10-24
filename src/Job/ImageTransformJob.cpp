/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2011 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ImageTransformJob.h"
#include "ComicBookDebug.h"
#include <QTransform>
#include <QImage>
#include <QtMath>

using namespace QComicBook;

ImageTransformJob::ImageTransformJob(): m_ratio(1.0), m_matrix(0)
{
}

ImageTransformJob::~ImageTransformJob()
{
    _DEBUG;
    delete m_matrix;
}

void ImageTransformJob::setKey(const JobKey &k)
{
    m_key = k;
}

const JobKey& ImageTransformJob::key() const
{
    return m_key;
}

void ImageTransformJob::setSize(int w, int h)
{
    m_width = w;
    m_height = h;
}

void ImageTransformJob::setDevicePixelRatio(qreal ratio)
{
    m_ratio = ratio;
}

QSize ImageTransformJob::pixelSize(const QSize &size, qreal ratio)
{
    return QSize(qCeil(size.width() * ratio), qCeil(size.height() * ratio));
}

QImage* ImageTransformJob::createResultImage() const
{
    const QSize size(pixelSize(QSize(m_width, m_height), m_ratio));
    QImage *img = new QImage(size, QImage::Format_ARGB32);
    img->setDevicePixelRatio(m_ratio);
    img->fill(0); // this prevents artifcats/garbage in transparent images
    return img;
}

void ImageTransformJob::setMatrix(const QTransform &m)
{
    delete m_matrix;
    m_matrix = new QTransform(m);
}

void ImageTransformJob::setViewProperties(const ViewPropertiesData &props)
{
    m_props = props;
}
