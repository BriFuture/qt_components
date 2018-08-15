#include <QString>
#include <QtTest>
#include <QDebug>

#define protected public
#define private public
#include "commfactory.h"
#include "commglobal.h"
#undef protected
#undef private

class CommFactoryTest : public QObject
{
    Q_OBJECT

public:
    CommFactoryTest();
    ~CommFactoryTest();

private Q_SLOTS:
    void testCreateComm();
    void testCommInfo();
    void testSupportHalfDuplex();
    void testCommClassName();
private:
    CommFactory *cf;
};

CommFactoryTest::CommFactoryTest()
{
    Comm::init();
    cf = Comm::factory;
    qDebug() << "CommFactoryTest: " << &Comm::factory << cf<< cf->commInfo().size();
}

CommFactoryTest::~CommFactoryTest()
{
    Comm::destroy();
}

void CommFactoryTest::testCreateComm()
{
    qDebug() << "before Test Create Comm";
    AbstractComm *ac  = cf->createComm("ComFullDuplex");
    AbstractComm *ac2 = cf->createComm( "ComHalfDuplex" );
    AbstractComm *ac3 = cf->createComm("ComFullDuplex");
    QVERIFY2( ac != 0, "Failure");
    QVERIFY2( ac2 != 0, "Failure");
    QVERIFY2( ac2 != ac, "Failure");
    QVERIFY2( ac3 != 0, "Failure");
    QVERIFY2( ac3 == ac, "Failure" );
    QVERIFY2( ac3 != ac2, "Failure" );
    qDebug() << ac << ac2 << ac3;

    ac->deleteLater();
}

void CommFactoryTest::testCommInfo()
{
    QListIterator<CommInfo> it(cf->commInfo());
    QVERIFY2( cf->commInfo().size() > 0, "Failure");
    while( it.hasNext() ) {
        const CommInfo &ci = it.next();
        qDebug() << ci.type << ci.desc;
    }
}

void CommFactoryTest::testSupportHalfDuplex()
{
    bool half = cf->supportHalfDuplex("SerialPort");
    QVERIFY2( half == true, "Failure");
    half = cf->supportHalfDuplex("Network");
    QVERIFY2( half == false, "Failure");

}

void CommFactoryTest::testCommClassName()
{
    QString className = cf->commClassName("SerialPort", true); // ComHalfDuplex
    QVERIFY2(className == "ComHalfDuplex", "Failure");
}

QTEST_APPLESS_MAIN(CommFactoryTest)

#include "tst_commfactorytest.moc"
