#include <QString>
#include <QtTest>
#include <QDebug>

#define private public
#define protected public

#include "commglobal.h"
#include "abstractprotocol.h"
#include "commnetwork.h"

class InvalidProtocol : public AbstractProtocol {
    Q_OBJECT
public:
    InvalidProtocol(QObject *parent = 0);

    virtual const int remainCmdCount() {
        return 1;
    }
    virtual const QByteArray firstQueryContent() {
        return "test";
    }
    virtual const int totalCmdCount()  {
        return 1;
    }
    virtual void stopRemainCmd() {

    }
    virtual void startTiming() {

    }
public slots:
    // AbstractProtocol interface
    virtual void stopQuery() {

    }
    virtual bool processData(const QByteArray &data, const int index=0) {
        qDebug() << data << index;
        return false;
    }

    void lastQueryFailed() {

    }

public:
    void clearCmdCount() override {

    }

    CommandObject *getFirstCmd() override {
        return 0;
    }
};

InvalidProtocol::InvalidProtocol(QObject *parent) : AbstractProtocol(parent) {

}

class CommManagerTest : public QObject
{
    Q_OBJECT

public:
    CommManagerTest();

public slots:
    void onCmdCountChange(const int r, const int a);
    void onStateChanged(const CommState &state);
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testResetMode();
    void testAddProtocol();
    void testRecvData();
    void testCurrentCommInfo();
    void testProtocol();

private:
    CommManager *mgr;
    InvalidProtocol *ip;
};

CommManagerTest::CommManagerTest()
{
    Comm::init();
    mgr = Comm::manager;
    connect(mgr, &CommManager::cmdCountChanged, this, &CommManagerTest::onCmdCountChange);
    connect(mgr, &CommManager::stateChanged, this, &CommManagerTest::onStateChanged);
    ip  = new InvalidProtocol;
    ip->setObjectName("InValid");
}

void CommManagerTest::onCmdCountChange(const int r, const int a)
{
    qDebug() << r << a;
}

void CommManagerTest::onStateChanged(const CommState &state)
{
    qDebug() << "state change: " << state; // state can't be changed becuase QEventLoop does not exist
}

void CommManagerTest::initTestCase()
{
    mgr->addProtocol(ip);

    mgr->openDevice("COM1");
    QVERIFY2( mgr->protocolList().size() > 0, "Failure");
    QVERIFY2( mgr->thread()!= mgr->m_abstractComm->thread(), "Failure");

}

void CommManagerTest::cleanupTestCase()
{
    Comm::destroy();
    ip->deleteLater();
}

void CommManagerTest::testResetMode()
{
    AbstractComm *ac1 = mgr->m_abstractComm;
    QVERIFY2(ac1 != 0, "Failure");
    mgr->setCommProperty("baudRate", 38400);
    mgr->openDevice();
    CommState state = mgr->state();
    qDebug() << state;

    mgr->resetMode("VirtualCom");
    AbstractComm *ac2 = mgr->m_abstractComm;
    QVERIFY2(ac2 != 0, "Failure");
    QVERIFY2(ac2 != ac1, "Failure");
    state = mgr->state();
    qDebug() << state;

    mgr->resetMode("ComFullDuplex");
    AbstractComm *ac3 = mgr->m_abstractComm;
    QVERIFY2(ac1 == ac3, "Failure");
}

void CommManagerTest::testAddProtocol()
{
    mgr->delProtocol(ip);
    QVERIFY2( mgr->protocolList().size() == 0, "Failure");
    mgr->startQuery();
    mgr->addProtocol(ip);
    QVERIFY2( mgr->protocolList().size() > 0, "Failure");
}

void CommManagerTest::testRecvData()
{
    for(int i = 0; i < 10; i++) {
//        mgr->onRecvLineData("test");
        QMetaObject::invokeMethod( mgr, "onRecvLineData", Q_ARG(QByteArray, "test") );
    }

}

void CommManagerTest::testCurrentCommInfo()
{
    mgr->resetMode("CommNetwork");
    const CommInfo &ci = mgr->currentCommInfo();
    qDebug() << ci;
    QVERIFY2( !ci.type.isEmpty(), "Type Failure" );
    QVERIFY2( !ci.desc.isEmpty(), "Type Failure" );
//    QVERIFY2( ci.isHalfDuplex == nci.isHalfDuplex, "Type Failure" );
}

void CommManagerTest::testProtocol()
{
    AbstractProtocol *ap = mgr->protocol("InValid");
    QVERIFY2(ap = ip, "Protocol Faliure");
    InvalidProtocol *ip2 = mgr->getProtocol<InvalidProtocol *>("InValid");
    qDebug() << ip2;
    QVERIFY2(ip2 == ip, "Cast");
}

QTEST_APPLESS_MAIN(CommManagerTest)

#include "tst_commmanagertest.moc"
