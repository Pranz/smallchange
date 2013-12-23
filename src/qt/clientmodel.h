#ifndef CLIENTMODEL_H
#define CLIENTMODEL_H

#include <QObject>

class OptionsModel;
class AddressTableModel;
class TransactionTableModel;
class CWallet;

QT_BEGIN_NAMESPACE
class QDateTime;
class QTimer;
QT_END_NAMESPACE

/** Model for Toakrona network client. */
class ClientModel : public QObject
{
    Q_OBJECT
public:
    explicit ClientModel(OptionsModel *optionsModel, QObject *parent = 0);
    ~ClientModel();

    enum PlumbingType
    {
        SoloPlumbing,
        PoolPlumbing
    };

    OptionsModel *getOptionsModel();

    int getNumConnections() const;
    int getNumBlocks() const;
    int getNumBlocksAtStartup();
    PlumbingType getPlumbingType() const;
    int getPlumbingThreads() const;
    bool getPlumbingStarted() const;

    bool getPlumbingDebug() const;
    void setPlumbingDebug(bool debug);
    int getPlumbingScanTime() const;
    void setPlumbingScanTime(int scantime);
    QString getPlumbingServer() const;
    void setPlumbingServer(QString server);
    QString getPlumbingPort() const;
    void setPlumbingPort(QString port);
    QString getPlumbingUsername() const;
    void setPlumbingUsername(QString username);
    QString getPlumbingPassword() const;
    void setPlumbingPassword(QString password);

    int getHashrate() const;
    double GetDifficulty() const;

    QDateTime getLastBlockDate() const;

    //! Return true if client connected to testnet
    bool isTestNet() const;
    //! Return true if core is doing initial block download
    bool inInitialBlockDownload() const;
    //! Return conservative estimate of total number of blocks, or 0 if unknown
    int getNumBlocksOfPeers() const;
    //! Return warnings to be displayed in status bar
    QString getStatusBarWarnings() const;

    void setPlumbing(PlumbingType type, bool plumbing, int threads, int hashrate);

    QString formatFullVersion() const;
    QString formatBuildDate() const;
    QString clientName() const;
    QString formatClientStartupTime() const;

private:
    OptionsModel *optionsModel;

    int cachedNumBlocks;
    int cachedNumBlocksOfPeers;
    int cachedHashrate;

    PlumbingType plumbingType;
    int plumbingThreads;
    bool plumbingStarted;
    bool plumbingDebug;
    int plumbingScanTime;
    QString plumbingServer;
    QString plumbingPort;
    QString plumbingUsername;
    QString plumbingPassword;

    int numBlocksAtStartup;

    QTimer *pollTimer;

    void subscribeToCoreSignals();
    void unsubscribeFromCoreSignals();
signals:
    void numConnectionsChanged(int count);
    void numBlocksChanged(int count, int countOfPeers);
    void plumbingChanged(bool plumbing, int count);

    //! Asynchronous error notification
    void error(const QString &title, const QString &message, bool modal);

public slots:
    void updateTimer();
    void updateNumConnections(int numConnections);
    void updateAlert(const QString &hash, int status);
};

#endif // CLIENTMODEL_H
