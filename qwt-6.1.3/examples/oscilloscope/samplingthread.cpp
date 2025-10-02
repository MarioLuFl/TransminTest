#include "samplingthread.h"
#include "signaldata.h"
#include <qwt_math.h>
#include <math.h>

#if QT_VERSION < 0x040600
#define qFastSin(x) ::sin(x)
#endif

SamplingThread::SamplingThread( QObject *parent ):
    QwtSamplingThread( parent ),
    d_frequency( 5.0 ),
    d_amplitude( 20.0 )
{
}

void SamplingThread::setFrequency( double frequency )
{
    d_frequency = frequency;
}

double SamplingThread::frequency() const
{
    return d_frequency;
}

void SamplingThread::setAmplitude( double amplitude )
{
    d_amplitude = amplitude;
}

double SamplingThread::amplitude() const
{
    return d_amplitude;
}

void SamplingThread::start()
{
    // Al reanudar, reinicia el reloj interno
    // pero mantiene d_elapsedOffset para que el tiempo se reanude
    d_lastElapsed = 0.0;
    QwtSamplingThread::start();
}

void SamplingThread::stop()
{
    // Acumula el último tramo transcurrido antes de parar
    d_elapsedOffset += d_lastElapsed;
    QwtSamplingThread::stop();
}

void SamplingThread::sample(double elapsed)
{
    // elapsed = segs desde el último start(); le sumamos el offset acumulado
    d_lastElapsed = elapsed;
    const double t = d_elapsedOffset + elapsed;

    if (d_frequency > 0.0)
    {
        const QPointF s(t, value(t)); // usar t para X y para la fase
        SignalData::instance().append(s);
    }
}

double SamplingThread::value(double timeStamp) const
{
    const double period = 1.0 / d_frequency;
    const double x = ::fmod(timeStamp, period);
    const double v = d_amplitude * qFastSin(x / period * 2 * M_PI);

    return d_offset + v;
}

void SamplingThread::setOffset(double offset)
{
    d_offset = offset;
}

