#include "plumbingpage.h"
#include "ui_plumbingpage.h"

PlumbingPage::PlumbingPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlumbingPage)
{
    ui->setupUi(this);

    setFixedSize(400, 420);

    minerActive = false;

    minerProcess = new QProcess(this);
    minerProcess->setProcessChannelMode(QProcess::MergedChannels);

    readTimer = new QTimer(this);

    acceptedShares = 0;
    rejectedShares = 0;

    roundAcceptedShares = 0;
    roundRejectedShares = 0;

    initThreads = 0;

    connect(readTimer, SIGNAL(timeout()), this, SLOT(readProcessOutput()));

    connect(ui->startButton, SIGNAL(pressed()), this, SLOT(startPressed()));
    connect(ui->typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
    connect(ui->debugCheckBox, SIGNAL(toggled(bool)), this, SLOT(debugToggled(bool)));
    connect(minerProcess, SIGNAL(started()), this, SLOT(minerStarted()));
    connect(minerProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(minerError(QProcess::ProcessError)));
    connect(minerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(minerFinished()));
    connect(minerProcess, SIGNAL(readyRead()), this, SLOT(readProcessOutput()));
}

PlumbingPage::~PlumbingPage()
{
    minerProcess->kill();

    delete ui;
}

void PlumbingPage::setModel(ClientModel *model)
{
    this->model = model;

    loadSettings();

    bool pool = model->getPlumbingType() == ClientModel::PoolPlumbing;
    ui->threadsBox->setValue(model->getPlumbingThreads());
    ui->typeBox->setCurrentIndex(pool ? 1 : 0);
//    if (model->getPlumbingStarted())
//        startPressed();
}

void PlumbingPage::startPressed()
{
    initThreads = ui->threadsBox->value();

    if (minerActive == false)
    {
        saveSettings();

        if (getPlumbingType() == ClientModel::SoloPlumbing)
            minerStarted();
        else
            startPoolPlumbing();
    }
    else
    {
        if (getPlumbingType() == ClientModel::SoloPlumbing)
            minerFinished();
        else
            stopPoolPlumbing();
    }
}

void PlumbingPage::startPoolPlumbing()
{
    QStringList args;
    QString url = ui->serverLine->text();
    if (!url.contains("http://"))
        url.prepend("http://");
    QString urlLine = QString("%1:%2").arg(url, ui->portLine->text());
    QString userpassLine = QString("%1:%2").arg(ui->usernameLine->text(), ui->passwordLine->text());
    args << "--algo" << "scrypt";
    args << "--scantime" << ui->scantimeBox->text().toAscii();
    args << "--url" << urlLine.toAscii();
    args << "--userpass" << userpassLine.toAscii();
    args << "--threads" << ui->threadsBox->text().toAscii();
    args << "--retries" << "-1"; // Retry forever.
    args << "-P"; // This is needed for this to work correctly on Windows. Extra protocol dump helps flush the buffer quicker.

    threadSpeed.clear();

    acceptedShares = 0;
    rejectedShares = 0;

    roundAcceptedShares = 0;
    roundRejectedShares = 0;

    // If minerd is in current path, then use that. Otherwise, assume minerd is in the path somewhere.
    QString program = QDir::current().filePath("minerd");
    if (!QFile::exists(program))
        program = "minerd";

    if (ui->debugCheckBox->isChecked())
        ui->list->addItem(args.join(" ").prepend(" ").prepend(program));

    ui->mineSpeedLabel->setText("Speed: N/A");
    ui->shareCount->setText("Accepted: 0 - Rejected: 0");
    minerProcess->start(program,args);
    minerProcess->waitForStarted(-1);

    readTimer->start(500);
}

void PlumbingPage::stopPoolPlumbing()
{
    ui->mineSpeedLabel->setText("");
    minerProcess->kill();
    readTimer->stop();
}

void PlumbingPage::saveSettings()
{
    model->setPlumbingDebug(ui->debugCheckBox->isChecked());
    model->setPlumbingScanTime(ui->scantimeBox->value());
    model->setPlumbingServer(ui->serverLine->text());
    model->setPlumbingPort(ui->portLine->text());
    model->setPlumbingUsername(ui->usernameLine->text());
    model->setPlumbingPassword(ui->passwordLine->text());
}

void PlumbingPage::loadSettings()
{
    ui->debugCheckBox->setChecked(model->getPlumbingDebug());
    ui->scantimeBox->setValue(model->getPlumbingScanTime());
    ui->serverLine->setText(model->getPlumbingServer());
    ui->portLine->setText(model->getPlumbingPort());
    ui->usernameLine->setText(model->getPlumbingUsername());
    ui->passwordLine->setText(model->getPlumbingPassword());
}

void PlumbingPage::readProcessOutput()
{
    QByteArray output;

    minerProcess->reset();

    output = minerProcess->readAll();

    QString outputString(output);

    if (!outputString.isEmpty())
    {
        QStringList list = outputString.split("\n", QString::SkipEmptyParts);
        int i;
        for (i=0; i<list.size(); i++)
        {
            QString line = list.at(i);

            // Ignore protocol dump
            if (!line.startsWith("[") || line.contains("JSON protocol") || line.contains("HTTP hdr"))
                continue;

            if (ui->debugCheckBox->isChecked())
            {
                ui->list->addItem(line.trimmed());
                ui->list->scrollToBottom();
            }

            if (line.contains("(yay!!!)"))
                reportToList("Share accepted", SHARE_SUCCESS, getTime(line));
            else if (line.contains("(booooo)"))
                reportToList("Share rejected", SHARE_FAIL, getTime(line));
            else if (line.contains("LONGPOLL detected new block"))
                reportToList("LONGPOLL detected a new block", LONGPOLL, getTime(line));
            else if (line.contains("Supported options:"))
                reportToList("Miner didn't start properly. Try checking your settings.", ERROR, NULL);
            else if (line.contains("The requested URL returned error: 403"))
                reportToList("Couldn't connect. Please check your username and password.", ERROR, NULL);
            else if (line.contains("HTTP request failed"))
                reportToList("Couldn't connect. Please check pool server and port.", ERROR, NULL);
            else if (line.contains("JSON-RPC call failed"))
                reportToList("Couldn't communicate with server. Retrying in 30 seconds.", ERROR, NULL);
            else if (line.contains("thread ") && line.contains("khash/s"))
            {
                QString threadIDstr = line.at(line.indexOf("thread ")+7);
                int threadID = threadIDstr.toInt();

                int threadSpeedindx = line.indexOf(",");
                QString threadSpeedstr = line.mid(threadSpeedindx);
                threadSpeedstr.chop(8);
                threadSpeedstr.remove(", ");
                threadSpeedstr.remove(" ");
                threadSpeedstr.remove('\n');
                double speed=0;
                speed = threadSpeedstr.toDouble();

                threadSpeed[threadID] = speed;

                updateSpeed();
            }
        }
    }
}

void PlumbingPage::minerError(QProcess::ProcessError error)
{
    if (error == QProcess::FailedToStart)
    {
        reportToList("Miner failed to start. Make sure you have the minerd executable and libraries in the same directory as toakrona-qt.", ERROR, NULL);
    }
}

void PlumbingPage::minerFinished()
{
    if (getPlumbingType() == ClientModel::SoloPlumbing)
        reportToList("Solo plumbing stopped.", ERROR, NULL);
    else
        reportToList("Miner exited.", ERROR, NULL);
    ui->list->addItem("");
    minerActive = false;
    resetPlumbingButton();
    model->setPlumbing(getPlumbingType(), false, initThreads, 0);
}

void PlumbingPage::minerStarted()
{
    if (!minerActive)
        if (getPlumbingType() == ClientModel::SoloPlumbing)
            reportToList("Solo plumbing started.", ERROR, NULL);
        else
            reportToList("Miner started. You might not see any output for a few minutes.", STARTED, NULL);
    minerActive = true;
    resetPlumbingButton();
    model->setPlumbing(getPlumbingType(), true, initThreads, 0);
}

void PlumbingPage::updateSpeed()
{
    double totalSpeed=0;
    int totalThreads=0;

    QMapIterator<int, double> iter(threadSpeed);
    while(iter.hasNext())
    {
        iter.next();
        totalSpeed += iter.value();
        totalThreads++;
    }

    // If all threads haven't reported the hash speed yet, make an assumption
    if (totalThreads != initThreads)
    {
        totalSpeed = (totalSpeed/totalThreads)*initThreads;
    }

    QString speedString = QString("%1").arg(totalSpeed);
    QString threadsString = QString("%1").arg(initThreads);

    QString acceptedString = QString("%1").arg(acceptedShares);
    QString rejectedString = QString("%1").arg(rejectedShares);

    QString roundAcceptedString = QString("%1").arg(roundAcceptedShares);
    QString roundRejectedString = QString("%1").arg(roundRejectedShares);

    if (totalThreads == initThreads)
        ui->mineSpeedLabel->setText(QString("Speed: %1 khash/sec - %2 thread(s)").arg(speedString, threadsString));
    else
        ui->mineSpeedLabel->setText(QString("Speed: ~%1 khash/sec - %2 thread(s)").arg(speedString, threadsString));

    ui->shareCount->setText(QString("Accepted: %1 (%3) - Rejected: %2 (%4)").arg(acceptedString, rejectedString, roundAcceptedString, roundRejectedString));

    model->setPlumbing(getPlumbingType(), true, initThreads, totalSpeed*1000);
}

void PlumbingPage::reportToList(QString msg, int type, QString time)
{
    QString message;
    if (time == NULL)
        message = QString("[%1] - %2").arg(QTime::currentTime().toString(), msg);
    else
        message = QString("[%1] - %2").arg(time, msg);

    switch(type)
    {
        case SHARE_SUCCESS:
            acceptedShares++;
            roundAcceptedShares++;
            updateSpeed();
            break;

        case SHARE_FAIL:
            rejectedShares++;
            roundRejectedShares++;
            updateSpeed();
            break;

        case LONGPOLL:
            roundAcceptedShares = 0;
            roundRejectedShares = 0;
            break;

        default:
            break;
    }

    ui->list->addItem(message);
    ui->list->scrollToBottom();
}

// Function for fetching the time
QString PlumbingPage::getTime(QString time)
{
    if (time.contains("["))
    {
        time.resize(21);
        time.remove("[");
        time.remove("]");
        time.remove(0,11);

        return time;
    }
    else
        return NULL;
}

void PlumbingPage::enablePlumbingControls(bool enable)
{
    ui->typeBox->setEnabled(enable);
    ui->threadsBox->setEnabled(enable);
    ui->scantimeBox->setEnabled(enable);
    ui->serverLine->setEnabled(enable);
    ui->portLine->setEnabled(enable);
    ui->usernameLine->setEnabled(enable);
    ui->passwordLine->setEnabled(enable);
}

void PlumbingPage::enablePoolPlumbingControls(bool enable)
{
    ui->scantimeBox->setEnabled(enable);
    ui->serverLine->setEnabled(enable);
    ui->portLine->setEnabled(enable);
    ui->usernameLine->setEnabled(enable);
    ui->passwordLine->setEnabled(enable);
}

ClientModel::PlumbingType PlumbingPage::getPlumbingType()
{
    if (ui->typeBox->currentIndex() == 0)  // Solo Plumbing
    {
        return ClientModel::SoloPlumbing;
    }
    else if (ui->typeBox->currentIndex() == 1)  // Pool Plumbing
    {
        return ClientModel::PoolPlumbing;
    }
    return ClientModel::SoloPlumbing;
}

void PlumbingPage::typeChanged(int index)
{
    if (index == 0)  // Solo Plumbing
    {
        enablePoolPlumbingControls(false);
    }
    else if (index == 1)  // Pool Plumbing
    {
        enablePoolPlumbingControls(true);
    }
}

void PlumbingPage::debugToggled(bool checked)
{
    model->setPlumbingDebug(checked);
}

void PlumbingPage::resetPlumbingButton()
{
    ui->startButton->setText(minerActive ? "Stop Plumbing" : "Start Plumbing");
    enablePlumbingControls(!minerActive);
}
