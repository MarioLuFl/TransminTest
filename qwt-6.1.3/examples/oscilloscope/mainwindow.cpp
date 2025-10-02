#include "mainwindow.h"
#include "plot.h"
#include "knob.h"
#include "wheelbox.h"
#include <qwt_scale_engine.h>

#include <qlabel.h>
#include <qlayout.h>
#include <QPixmap>
#include <QFileDialog>
#include <QColorDialog> 

MainWindow::MainWindow( QWidget *parent ):
    QWidget( parent )
{
    const double intervalLength = 10.0; // seconds

    //COLUMNA IZQUIERDA
    d_plot = new Plot( this );
    d_plot->setIntervalLength( intervalLength );

    d_amplitudeKnob = new Knob( "Amplitude", 0.0, 200.0, this );
    d_amplitudeKnob->setValue( 160.0 );
    connect(d_amplitudeKnob, SIGNAL(valueChanged(double)), SIGNAL(amplitudeChanged(double)));
    connect(d_amplitudeKnob, SIGNAL(valueChanged(double)),this, SLOT(refreshLegendText()));

    d_frequencyKnob = new Knob( "Frequency [Hz]", 0.1, 20.0, this );
    d_frequencyKnob->setValue( 17.8 );
    connect(d_frequencyKnob, SIGNAL(valueChanged(double)),SIGNAL(frequencyChanged(double)));
    connect(d_frequencyKnob, SIGNAL(valueChanged(double)), this, SLOT(refreshLegendText()));

    d_intervalWheel = new WheelBox( "Displayed [s]", 1.0, 100.0, 1.0, this );
    d_intervalWheel->setValue( intervalLength );
    connect(d_intervalWheel, SIGNAL(valueChanged(double)), d_plot, SLOT(setIntervalLength(double)));
    //He editado el sample interval para que el mínimo sea 0.1 ms, ya que 0 ms sería una frecuencia infinita de muestreo lo cuál no es coherente.
    d_timerWheel = new WheelBox( "Sample Interval [ms]", 0.1, 20.0, 0.1, this );
    d_timerWheel->setValue( 10.0 );
    connect(d_timerWheel, SIGNAL(valueChanged(double)), SIGNAL(signalIntervalChanged(double)));

    QVBoxLayout* vLayout1 = new QVBoxLayout();
    vLayout1->addWidget( d_intervalWheel );
    vLayout1->addWidget( d_timerWheel );
    vLayout1->addStretch( 10 );
    vLayout1->addWidget( d_amplitudeKnob );
    vLayout1->addWidget( d_frequencyKnob );

    //COLUMNA BOTONES
    d_stopButton = new QPushButton(tr("Stop"), this);
    d_stopButton->setMinimumWidth(140);
    connect(d_stopButton, SIGNAL(clicked()), this, SLOT(onStopClicked()));

    d_snapshotButton = new QPushButton(tr("Foto Actual"), this);
    d_snapshotButton->setMinimumWidth(140);
    connect(d_snapshotButton, SIGNAL(clicked()), this, SLOT(onSnapshotClicked()));

    d_colorButton = new QPushButton(tr("Change Color"), this);
    d_colorButton->setMinimumWidth(140);
    connect(d_colorButton, SIGNAL(clicked()), this, SLOT(onColorClicked()));

    d_legendCheck = new QCheckBox(tr("Show Legend"), this);
    d_legendCheck->setChecked(false);
    connect(d_legendCheck, &QCheckBox::toggled, this, &MainWindow::onLegendToggled);

    d_offsetWheel = new WheelBox("Mover verticalmente", -100.0, 100.0, 1.0, this);
    d_offsetWheel->setValue(0.0);
    connect(d_offsetWheel, SIGNAL(valueChanged(double)), this, SLOT(refreshLegendText()));
    connect(d_offsetWheel, SIGNAL(valueChanged(double)), SIGNAL(dcOffsetChanged(double)));

    QVBoxLayout* vbotones = new QVBoxLayout();
    vbotones->addWidget(d_stopButton);
    vbotones->addWidget(d_snapshotButton);
    vbotones->addWidget(d_colorButton);
    vbotones->addStretch(1);
    vbotones->addWidget(d_legendCheck);
    vbotones->addWidget(d_offsetWheel);
    vbotones->addStretch(1);

    //COLUMNA QLista
    QLabel* listTitle = new QLabel(tr("QListWidget"), this);
    d_listWidget = new QListWidget(this);
    d_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(d_listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onItemChanged(int)));

    QVBoxLayout* vQLista = new QVBoxLayout();
    vQLista->addWidget(listTitle);
    vQLista->addWidget(d_listWidget, 1);
    vQLista->addStretch(1);

    //COLUMNA Captura
    d_infoLabel = new QLabel(tr("Imagen: Item -,  Amplitude: -,  Frequency: -."), this);
    d_exportBtn = new QPushButton(tr("Export Image"), this);
    QObject::connect(d_exportBtn, &QPushButton::clicked, this, &MainWindow::onExportClicked);

    QHBoxLayout* infoRow = new QHBoxLayout();
    infoRow->addWidget(d_infoLabel, 1);
    infoRow->addWidget(d_exportBtn, 0, Qt::AlignRight);

    d_previewTitle = new QLabel(tr("Widget Vista de imagen:"), this);

    d_preview = new QLabel(this);
    d_preview->setMinimumSize(340, 220);
    d_preview->setFrameShape(QFrame::StyledPanel);
    d_preview->setAlignment(Qt::AlignCenter);
    d_preview->setText(tr("Preview"));

    QVBoxLayout* vCaptura = new QVBoxLayout();
    vCaptura->addLayout(infoRow);
    vCaptura->addWidget(d_previewTitle);
    vCaptura->addWidget(d_preview, 1);

    //ACUMULADOR DE LAS COLUMNAS VISUALES
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->addWidget( d_plot, 10 );
    layout->addLayout( vLayout1 );
    layout->addLayout( vbotones );
    layout->addLayout( vQLista );
    layout->addLayout( vCaptura );
}

void MainWindow::start()
{
    d_plot->start();
    d_running = true;
    d_stopButton->setText("STOP");
}

double MainWindow::frequency() const
{
    return d_frequencyKnob->value();
}

double MainWindow::amplitude() const
{
    return d_amplitudeKnob->value();
}

double MainWindow::signalInterval() const
{
    return d_timerWheel->value();
}

void MainWindow::onStopClicked()
{
    if (d_running) // Pasar a estado detenido
    {
        if (d_plot) d_plot->stop();     // congela tiempo
        emit stopRequested();           // detiene el hilo
        d_stopButton->setText("RUN");
        d_running = false;
    }
    else // Pasar a estado en ejecución
    {
        emit signalIntervalChanged(d_timerWheel->value());
        emit runRequested();            // arranca el hilo
        if (d_plot) d_plot->start();
        d_stopButton->setText("STOP");
        d_running = true;
    }
}

void MainWindow::onSnapshotClicked()
{
    // Render del plot a imagen (en memoria)
    QPixmap px(d_plot->size());
    d_plot->render(&px);
    QImage img = px.toImage();

    // Metadata (nombre, amplitud, frecuencia)
    const int n = d_snaps.size() + 1;
    Snap s;
    s.name = QString("Item %1").arg(n);
    s.amp = amplitude();
    s.freq = frequency();
    s.img = img;
    s.color = d_plot->curveColor();

    // Guardar en memoria y añadir a la lista
    d_snaps.push_back(s);
    auto* it = new QListWidgetItem(s.name, d_listWidget);
    it->setData(Qt::UserRole, n - 1);   // guardo el índice del vector

    // Seleccionar el recién agregado (dispara onItemChanged)
    d_listWidget->setCurrentRow(d_listWidget->count() - 1);
}

void MainWindow::onColorClicked()
{
    const QColor initial = d_plot->curveColor();
    QColor c = QColorDialog::getColor(initial, this, tr("Seleccionar color de trazo"));
    if (!c.isValid())
        return;
    d_plot->setCurveColor(c);
}

void MainWindow::onItemChanged(int row)
{
    if (row < 0 || row >= d_snaps.size()) {
        d_infoLabel->setText(tr("Imagen: -,  Amplitude: -,  Frequency: -."));
        d_preview->setText(tr("Preview"));
        d_preview->setPixmap(QPixmap());
        return;
    }

    const Snap& s = d_snaps[row];

    // Texto superior
    d_infoLabel->setText(
        tr("Imagen: %1 ,  Amplitude: %2 ,  Frequency: %3 Hz.")
        .arg(s.name)
        .arg(s.amp, 0, 'f', 0)
        .arg(s.freq, 0, 'f', 1)
    );

    // Mostrar imagen
    QPixmap pm = QPixmap::fromImage(s.img);
    d_preview->setPixmap(pm.scaled(d_preview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    d_preview->setText(QString());
}

void MainWindow::onExportClicked()
{
    int row = d_listWidget->currentRow();
    if (row < 0 || row >= d_snaps.size())
        return; // nada seleccionado

    const Snap& s = d_snaps[row];

    // nombre sugerido: Item N.png
    QString suggested = s.name + ".png";
    QString fn = QFileDialog::getSaveFileName(
        this,
        tr("Exportar imagen"),
        suggested,
        tr("PNG Image (*.png)")
    );
    if (fn.isEmpty())
        return;

    // fuerza extensión .png si el usuario la quitó
    if (!fn.endsWith(".png", Qt::CaseInsensitive))
        fn += ".png";

    // guarda como PNG (en tamaño original de la captura)
    s.img.save(fn, "PNG");
}

void MainWindow::onLegendToggled(bool on)
{
    d_plot->toggleLegend(on);
    refreshLegendText();
}

void MainWindow::refreshLegendText()
{
    if (!d_legendCheck->isChecked()) return;

    const double amp = amplitude();
    const double freq = frequency();
    const double dc = d_offsetWheel ? d_offsetWheel->value() : 0.0;

    const QString txt = QString("Amplitude: %1\nFrequency: %2 Hz\nDC Offset: %3")
        .arg(amp, 0, 'f', 0)
        .arg(freq, 0, 'f', 1)
        .arg(dc, 0, 'f', 1);
    d_plot->setLegendText(txt);
}
