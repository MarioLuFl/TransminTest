#include <qwt_sampling_thread.h>

class SamplingThread: public QwtSamplingThread
{
    Q_OBJECT

public:
    SamplingThread( QObject *parent = NULL );

    double frequency() const;
    double amplitude() const;

public Q_SLOTS:
    void setAmplitude( double );
    void setFrequency( double );

    void start();
    void stop();
    void setOffset(double offset);

protected:
    virtual void sample( double elapsed );

private:
    virtual double value( double timeStamp ) const;

    double d_frequency;
    double d_amplitude;

    double d_elapsedOffset = 0.0; // segundos acumulados antes del último start
    double d_lastElapsed = 0.0;

    double d_offset = 0.0;
};
