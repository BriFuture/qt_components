#include <QString>
#include <QtTest>
#include <QFile>
#include <QDebug>

#define protected public
#define private public
#include "abstractcomm.h"
#include "comfullduplex.h"
#include "virtualcom.h"
#undef protected
#undef private

class AbstractCommTest : public QObject
{
    Q_OBJECT

public:
    AbstractCommTest();

public:
    void onRecvLine(const QByteArray &data) {
        lineCount++;
        qDebug() << "Test Recv:"<< data;
    }

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testState();
    void testReadDevice();
    void testSplitData_data();
    void testSplitData();

private:
    ComFullDuplex *cfd;
    VirtualCom    *vc;
    QFile f;
    int lineCount = 0;
};

AbstractCommTest::AbstractCommTest()
{
    cfd = new ComFullDuplex;
    vc = new VirtualCom;

    connect(cfd, &AbstractComm::recvLine, this, &AbstractCommTest::onRecvLine);

    f.setFileName("./test.txt");
    f.open(QIODevice::ReadOnly);
}

void AbstractCommTest::initTestCase()
{

}

void AbstractCommTest::cleanupTestCase()
{

}

void AbstractCommTest::testState()
{
    QVERIFY2(cfd->state().baudRate == 9600, "Failure");
    QVERIFY2(cfd->state().baudRate == vc->state().baudRate, "Failure");
    CommState s = cfd->state();
    s.baudRate = 38400;
    cfd->setState(s);

    QVERIFY2(vc->state().baudRate == 38400, "Failure");

    QVERIFY2(cfd->state().deviceName == vc->state().deviceName, "Failure");
    QVERIFY2(cfd->state().opened == vc->state().opened, "Failure");
    QVERIFY2(&cfd->state() == &vc->state(), "Failure");
}

void AbstractCommTest::testReadDevice()
{
    if( !f.isOpen() ) {
        QSKIP("File Not Found!");
    }
    lineCount = 0;
    QMetaObject::invokeMethod(cfd, "readDevice", Q_ARG(QIODevice *, &f) );
    QVERIFY2(lineCount > 0, "Failure");
}

void AbstractCommTest::testSplitData_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<int>("line");
    QTest::newRow("3NewLines") << "3abcdef\r\n3bcdefa\r\n\r\n" << 3;
    QTest::newRow("2NewLines") << "2cdefab\r\n\r\n2defabc" << 2;
    QTest::newRow("1NewLines") << "1abcdef\r\n1bcdefa" << 1;
}

void AbstractCommTest::testSplitData()
{
    QFETCH(QString, data);
    QFETCH(int, line);

    lineCount = 0;
    cfd->m_recvData = data.toLatin1();
    QMetaObject::invokeMethod(cfd, "splitData" );
    QCOMPARE( lineCount, line);
}

QTEST_APPLESS_MAIN(AbstractCommTest)

#include "tst_abstractcommtest.moc"
