/***************************************************************************
 *   Copyright (C) 2010 by Simon Andreas Eugster (simon.eu@gmail.com)      *
 *   This file is part of kdenlive. See www.kdenlive.org.                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VECTORSCOPE_H
#define VECTORSCOPE_H

#include <QtCore>
#include "ui_vectorscope_ui.h"
#include "abstractgfxscopewidget.h"

class ColorPlaneExport;
class ColorTools;

class Vectorscope_UI;
class VectorscopeGenerator;

enum BACKGROUND_MODE { BG_NONE = 0, BG_YUV = 1, BG_CHROMA = 2, BG_YPbPr = 3 };

/**
   \brief Displays the vectorscope of a frame.

   \see VectorscopeGenerator for more details about the vectorscope.
   */
class Vectorscope : public AbstractGfxScopeWidget {
    Q_OBJECT

public:
    Vectorscope(QWidget *parent = 0);
    ~Vectorscope();

    QString widgetName() const;

protected:
    ///// Implemented methods /////
    QRect scopeRect();
    QImage renderHUD(uint accelerationFactor);
    QImage renderGfxScope(uint accelerationFactor, const QImage);
    QImage renderBackground(uint accelerationFactor);
    bool isHUDDependingOnInput() const;
    bool isScopeDependingOnInput() const;
    bool isBackgroundDependingOnInput() const;
    virtual void readConfig();

    ///// Other /////
    void writeConfig();

private:
    Ui::Vectorscope_UI *ui;

    ColorTools *m_colorTools;
    ColorPlaneExport *m_colorPlaneExport;

    QActionGroup *m_agColorSpace;
    QAction *m_aColorSpace_YUV;
    QAction *m_aColorSpace_YPbPr;
    QAction *m_aExportBackground;
    QAction *m_aAxisEnabled;
    QAction *m_a75PBox;
    QAction *m_aIQLines;

    VectorscopeGenerator *m_vectorscopeGenerator;

    /** How to represent the pixels on the scope (green, original color, ...) */
    int iPaintMode;

    /** Custom scaling of the vectorscope */
    float m_gain;

    QPoint m_centerPoint, pR75, pG75, pB75, pCy75, pMg75, pYl75;
    QPoint qR75, qG75, qB75, qCy75, qMg75, qYl75;
    /** Unlike the scopeRect, this rect represents the overall visible rectangle
        and not only the square touching the Vectorscope's circle. */
    QRect m_visibleRect;

    /** Updates the dimension. Only necessary when the widget has been resized. */
    void updateDimensions();
    int cw;


private slots:
    void slotGainChanged(int);
    void slotBackgroundChanged();
    void slotExportBackground();
    void slotColorSpaceChanged();
};

#endif // VECTORSCOPE_H
