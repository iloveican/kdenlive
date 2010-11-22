#ifndef AUDIOSPECTRUM_H
#define AUDIOSPECTRUM_H

#include <QtCore>

#include "abstractaudioscopewidget.h"
#include "ui_audiospectrum_ui.h"

class AudioSpectrum_UI;

class AudioSpectrum : public AbstractAudioScopeWidget {
    Q_OBJECT

public:
    AudioSpectrum(Monitor *projMonitor, Monitor *clipMonitor, QWidget *parent = 0);

    // Implemented virtual methods
    QString widgetName() const;

protected:
    ///// Implemented methods /////
    QRect scopeRect();
    QImage renderHUD(uint accelerationFactor);
    QImage renderScope(uint accelerationFactor, const QVector<int16_t> audioFrame, const int freq, const int num_channels, const int num_samples);
    QImage renderBackground(uint accelerationFactor);
    bool isHUDDependingOnInput() const;
    bool isScopeDependingOnInput() const;
    bool isBackgroundDependingOnInput() const;
    virtual void readConfig();

private:
    Ui::AudioSpectrum_UI *ui;
};

#endif // AUDIOSPECTRUM_H