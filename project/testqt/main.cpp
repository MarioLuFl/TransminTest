
/*
  Soy Mario Lupaca y estoy interesado en la postulación. Este ejemplo fue implementado a partir de la
  plantilla brindada en GitHub (QWT/oscilloscope). Lo resolví utilizando las
  herramientas indicadas y siguiendo las instrucciones publicadas en su sitio web.
  El objetivo fue demostrar integración de QWT con Qt (UI, threading, captura de
  imágenes, exportación y leyendas embebidas) y la adaptación solicitada.
*/

#include <QtWidgets/QApplication>
#include "../../qwt-6.1.3/examples/oscilloscope/mainwindow.h"
#include "../../qwt-6.1.3/examples/oscilloscope/samplingThread.h"

int main(int argc, char *argv[])
{
     QApplication app( argc, argv );
    app.setPalette( Qt::darkGray );

    MainWindow window;
    window.resize( 800, 400 );

    SamplingThread samplingThread;
    samplingThread.setFrequency( window.frequency() );
    samplingThread.setAmplitude( window.amplitude() );
    samplingThread.setInterval( window.signalInterval() );

    window.connect( &window, SIGNAL( frequencyChanged( double ) ),
        &samplingThread, SLOT( setFrequency( double ) ) );
    window.connect( &window, SIGNAL( amplitudeChanged( double ) ),
        &samplingThread, SLOT( setAmplitude( double ) ) );
    window.connect( &window, SIGNAL( signalIntervalChanged( double ) ),
        &samplingThread, SLOT( setInterval( double ) ) );
    window.connect(&window, SIGNAL(dcOffsetChanged(double)),
        &samplingThread, SLOT(setOffset(double)));

    // Stop
    QObject::connect(&window, &MainWindow::stopRequested, &samplingThread, &SamplingThread::stop);
    QObject::connect(&window, &MainWindow::runRequested, [&]() { samplingThread.start(); });

    window.show();

    samplingThread.start();
    window.start();

    bool ok = app.exec();

    samplingThread.stop();
    samplingThread.wait( 1000 );

    return ok;
}
