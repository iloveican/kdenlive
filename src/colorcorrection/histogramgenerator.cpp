/***************************************************************************
 *   Copyright (C) 2010 by Simon Andreas Eugster (simon.eu@gmail.com)      *
 *   This file is part of kdenlive. See www.kdenlive.org.                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <algorithm>
#include <math.h>
#include <QImage>
#include <QPainter>
#include "histogramgenerator.h"

HistogramGenerator::HistogramGenerator()
{
}

QImage HistogramGenerator::calculateHistogram(const QSize &paradeSize, const QImage &image, const int &components,
                                              HistogramGenerator::Rec rec, const bool &unscaled, const uint &accelFactor) const
{
    if (paradeSize.height() <= 0 || paradeSize.width() <= 0 || image.width() <= 0 || image.height() <= 0) {
        return QImage();
    }

    bool drawY = (components & HistogramGenerator::ComponentY) != 0;
    bool drawR = (components & HistogramGenerator::ComponentR) != 0;
    bool drawG = (components & HistogramGenerator::ComponentG) != 0;
    bool drawB = (components & HistogramGenerator::ComponentB) != 0;
    bool drawSum = (components & HistogramGenerator::ComponentSum) != 0;

    int r[256], g[256], b[256], y[256], s[766];
    // Initialize the values to zero
    std::fill(r, r+256, 0);
    std::fill(g, g+256, 0);
    std::fill(b, b+256, 0);
    std::fill(y, y+256, 0);
    std::fill(s, s+766, 0);

    const uint iw = image.bytesPerLine();
    const uint ih = image.height();
    const uint ww = paradeSize.width();
    const uint wh = paradeSize.height();
    const uint byteCount = iw*ih;
    const uint stepsize = 4*accelFactor;

    const uchar *bits = image.bits();
    QRgb *col;


    // Read the stats from the input image
    for (uint i = 0; i < byteCount; i += stepsize) {
        col = (QRgb *)bits;

        r[qRed(*col)]++;
        g[qGreen(*col)]++;
        b[qBlue(*col)]++;
        if (drawY) {
            // Use if branch to avoid expensive multiplication if Y disabled
            if (rec == HistogramGenerator::Rec_601) {
                y[(int)floor(.299*qRed(*col) + .587*qGreen(*col) + .114*qBlue(*col))]++;
            } else {
                y[(int)floor(.2125*qRed(*col) + .7154*qGreen(*col) + .0721*qBlue(*col))]++;
            }
        }
        if (drawSum) {
            // Use an if branch here because the sum takes more operations than rgb
            s[qRed(*col)]++;
            s[qGreen(*col)]++;
            s[qBlue(*col)]++;
        }

        bits += stepsize;
    }


    const int nParts = (drawY ? 1 : 0) + (drawR ? 1 : 0) + (drawG ? 1 : 0) + (drawB ? 1 : 0) + (drawSum ? 1 : 0);
    if (nParts == 0) {
        // Nothing to draw
        return QImage();
    }

    const int d = 20; // Distance for text
    const int partH = (wh-nParts*d)/nParts;
    const float scaling = (float)partH/(byteCount >> 7);
    const int dist = 40;

    int wy = 0; // Drawing position

    QImage histogram(paradeSize, QImage::Format_ARGB32);
    QPainter davinci(&histogram);
    davinci.setPen(QColor(220, 220, 220, 255));
    histogram.fill(qRgba(0, 0, 0, 0));

    if (drawY) {
        drawComponentFull(&davinci, y, scaling, QRect(0, wy, ww, partH + dist), QColor(220, 220, 210, 255), dist, unscaled, 256);

        wy += partH + d;
    }

    if (drawSum) {
        drawComponentFull(&davinci, s, scaling/3, QRect(0, wy, ww, partH + dist), QColor(220, 220, 210, 255), dist, unscaled, 256);

        wy += partH + d;
    }

    if (drawR) {
        drawComponentFull(&davinci, r, scaling, QRect(0, wy, ww, partH + dist), QColor(255, 128, 0, 255), dist, unscaled, 256);

        wy += partH + d;
    }

    if (drawG) {
        drawComponentFull(&davinci, g, scaling, QRect(0, wy, ww, partH + dist), QColor(128, 255, 0, 255), dist, unscaled, 256);
        wy += partH + d;
    }

    if (drawB) {
        drawComponentFull(&davinci, b, scaling, QRect(0, wy, ww, partH + dist), QColor(0, 128, 255, 255), dist, unscaled, 256);

        wy += partH + d;
    }

    return histogram;
}

QImage HistogramGenerator::drawComponent(const int *y, const QSize &size, const float &scaling, const QColor &color,
                                         const bool &unscaled, const uint &max) const
{
    QImage component(max, size.height(), QImage::Format_ARGB32);
    component.fill(qRgba(0, 0, 0, 0));
    Q_ASSERT(scaling != INFINITY);

    const int partH = size.height();
    int partY;

    for (uint x = 0; x < max; x++) {
        // Calculate the height of the curve at position x
        partY = scaling*y[x];

        // Invert the y axis
        if (partY > partH-1) { partY = partH-1; }
        partY = partH-1 - partY;

        for (int k = partH-1; k >= partY; k--) {
            component.setPixel(x, k, color.rgba());
        }
    }
    if (unscaled && size.width() >= component.width()) {
        return component;
    } else {
        return component.scaled(size, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    }
}

void HistogramGenerator::drawComponentFull(QPainter *davinci, const int *y, const float &scaling, const QRect &rect,
                                        const QColor &color, const int &textSpace, const bool &unscaled, const uint &max) const
{
    QImage component = drawComponent(y, rect.size() - QSize(0, textSpace), scaling, color, unscaled, max);
    davinci->drawImage(rect.topLeft(), component);

    int min = 0;
    for (uint x = 0; x < max; x++) {
        min = x;
        if (y[x] > 0) {
            break;
        }
    }
    int maxVal = max-1;
    for (int x = max-1; x >= 0; x--) {
        maxVal = x;
        if (y[x] > 0) {
            break;
        }
    }

    const int textY = rect.bottom()-textSpace+15;
    const int dist = 40;
    const int cw = component.width();

    davinci->drawText(0,            textY, "min");
    davinci->drawText(dist,         textY, QString::number(min, 'f', 0));

    davinci->drawText(cw-dist-30,   textY, "max");
    davinci->drawText(cw-30,        textY, QString::number(maxVal, 'f', 0));
}
