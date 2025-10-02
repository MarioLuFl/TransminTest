#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_system_clock.h>
#include <qwt_plot_legenditem.h>

class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget * = NULL );
    virtual ~Plot();

    void start();
    void stop();
    virtual void replot();

    void toggleLegend(bool on);
    void setLegendText(const QString& text);

    void setCurveColor(const QColor& c);
    QColor curveColor() const;

    virtual bool eventFilter( QObject *, QEvent * );

public Q_SLOTS:
    void setIntervalLength( double );

protected:
    virtual void showEvent( QShowEvent * );
    virtual void resizeEvent( QResizeEvent * );
    virtual void timerEvent( QTimerEvent * );

private:
    void updateCurve();
    void incrementInterval();

    QwtPlotMarker *d_origin;
    QwtPlotCurve *d_curve;
    int d_paintedPoints;

    QwtPlotDirectPainter *d_directPainter;

    QwtInterval d_interval;
    int d_timerId;

    QwtSystemClock d_clock;

    double d_elapsedOffsetSec = 0.0; // segs totales hasta el último STOP
    double d_lastNowSec = 0.0; // último seg leído del reloj

    QwtPlotLegendItem* d_legendItem = nullptr;
};
