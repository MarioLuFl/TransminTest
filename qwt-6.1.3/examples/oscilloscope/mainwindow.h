#include <qwidget.h>
#include <qpushbutton.h>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>

class Plot;
class Knob;
class WheelBox;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow( QWidget * = NULL );

    void start();

    double amplitude() const;
    double frequency() const;
    double signalInterval() const;

Q_SIGNALS:
    void amplitudeChanged( double );
    void frequencyChanged( double );
    void signalIntervalChanged( double );
    void stopRequested();
    void runRequested();
    void dcOffsetChanged(double);

private Q_SLOTS:
    void onStopClicked();
    void onSnapshotClicked();
    void onColorClicked();
    void onItemChanged(int row);
    void onExportClicked();
    void onLegendToggled(bool on);
    void refreshLegendText();

private:
    // Plot
    Plot* d_plot;

    // Controles
    Knob *d_frequencyKnob;
    Knob *d_amplitudeKnob;
    WheelBox *d_timerWheel;
    WheelBox *d_intervalWheel;

    // Columna de botones
    QPushButton* d_stopButton;
    QPushButton* d_snapshotButton;
    QPushButton* d_colorButton;
    QCheckBox* d_legendCheck;
    WheelBox* d_offsetWheel;

    // Columna QList
    QListWidget* d_listWidget;

    // Columna Captura
    QLabel* d_infoLabel;    // "Imagen: ..., Amplitude: ..., Frequency: ..."
    QLabel* d_previewTitle; // "Widget Vista de imagen:"
    QLabel* d_preview;      // marco de previsualización (placeholder)
    QPushButton* d_exportBtn;    // "Export Image" (solo visual)

    // Modelo en memoria para las capturas
    struct Snap {
        QString name;
        double amp;
        double freq;
        QImage img;
        QColor color;
    };
    QVector<Snap> d_snaps;

    bool d_running = true;
};