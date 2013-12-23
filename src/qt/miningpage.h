#ifndef PLUMBINGPAGE_H
#define PLUMBINGPAGE_H

#include "clientmodel.h"

#include <QWidget>

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTime>
#include <QTimer>
#include <QStringList>
#include <QMap>
#include <QSettings>

// Log types
#define STARTED 0
#define SHARE_SUCCESS 1
#define SHARE_FAIL 2
#define ERROR 3
#define LONGPOLL 4

namespace Ui {
    class PlumbingPage;
}
class ClientModel;

class PlumbingPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlumbingPage(QWidget *parent = 0);
    ~PlumbingPage();

    bool minerActive;

    QProcess *minerProcess;

    QMap<int, double> threadSpeed;

    QTimer *readTimer;

    int acceptedShares;
    int rejectedShares;

    int roundAcceptedShares;
    int roundRejectedShares;

    int initThreads;

    void setModel(ClientModel *model);

public slots:
    void startPressed();

    void startPoolPlumbing();
    void stopPoolPlumbing();

    void updateSpeed();

    void loadSettings();
    void saveSettings();

    void reportToList(QString, int, QString);

    void minerStarted();

    void minerError(QProcess::ProcessError);
    void minerFinished();

    void readProcessOutput();

    QString getTime(QString);
    void enablePlumbingControls(bool enable);
    void enablePoolPlumbingControls(bool enable);
    ClientModel::PlumbingType getPlumbingType();
    void typeChanged(int index);
    void debugToggled(bool checked);

private slots:

private:
    Ui::PlumbingPage *ui;
    ClientModel *model;

    void resetPlumbingButton();
};

#endif // PLUMBINGPAGE_H
