#include "virtualcom.h"

#include <QDebug>
#include "commglobal.h"

/*!
 * \class  VirtualCom
 * \ingroup comm
 * \inherits AbstractComm
 * \author BriFuture
 * \date 2018.07.19
 *
 * \brief  虚拟串口用于模拟下位机发送数据
 *
 */
const CommInfo VirtualCom::commInfo = {QObject::tr("Simulator"), QObject::tr("Simulator Test Mode")};

VirtualCom::VirtualCom() : AbstractComm()
{

}


void VirtualCom::init() {
    m_timer = new QTimer( this );
    m_timer->setInterval( 30 );
    m_timer->setSingleShot( false );
    connect( m_timer, &QTimer::timeout, this, &VirtualCom::onRead );
}

void VirtualCom::close() {
    m_timer->stop();
    m_state.opened = false;
    emit stateChanged( m_state );
//    close();
}

void VirtualCom::openComm(const QString &name) {
    m_state.deviceName = name.isEmpty() ? "Simulator" : name;
    qInfo() << "VirtualCom open: " << m_state.deviceName;
    m_state.opened   = true;
    emit stateChanged( m_state );
    m_timer->start();
}

void VirtualCom::write(const QByteArray &rawdata) {
    QByteArray data = rawdata + LineSeparator;
    emit sendData( data );

    if( rawdata == "#PI_MODE?*7a" ) {
        emit recvLine( "#AC+DC*2E\r\n" );
    }
    else if( rawdata == "#BUZZER?*3f" ) {
        emit recvLine( "#OFF*4F\r\n" );
    }
    else if( rawdata == "#SAG?*6a") {
        emit recvLine( "#+601.00000*02\r\n" );
        emit recvLine( "#AC+DCE*2E\r\n" );
    }
    else if( rawdata == "#SDG?*6f" ) {
        emit recvLine( "#+9999.0000*05\r\n" );
    }
    else if( rawdata.startsWith( "#SDG=") ) {
        emit recvLine( QString("#%0*05\r\n").arg( (m_counter + index) ).toLatin1() );
    }
    else if( rawdata.startsWith( "#SAG=") ) {
        emit recvLine( QString("#%0*05\r\n").arg( (m_counter + index) ).toLatin1() );
    }
    else if( rawdata.startsWith( "#TESTSELF") ) {
        emit recvLine( QByteArray("#OK*a3\r\n") );
    }
}

void VirtualCom::setCommProperty(const QString &key, const QVariant &value) {
    Q_UNUSED( key );
    Q_UNUSED( value );
    if( key == "baudRate" ) {
        m_state.baudRate = value.toInt();
    }
}


void VirtualCom::onRead() {
    QByteArray rawData;
    if( index == 0 ) {
        rawData = "$DATA, ";
    }
    else if( index < 9 ) {
        rawData = QString::number( - rand() % 50 + 80 * sin( (m_counter + index) * 3.1415 / 180 ) * ( index % 2 == 0 ? 1 : -1 ) ).toLatin1().append( ", " );
    }
    else {
        rawData = QString::number( - rand() % 50 + 80 * cos( (m_counter + index) * 3.1415 / 180 ) ).toLatin1().append( "*0f" ).append( QByteArray( LineSeparator ) );
    }

    index++;
    emit recvRawData( rawData );
    m_data.append( rawData );
    if( index == 10 ) {
        index = 0;
        emit recvLine( m_data );
        m_data.clear();
    }

    m_counter ++ ;
    if( m_counter == 500 ) {
        m_counter = 0;
        m_timer->stop();
        QTimer::singleShot( 3000, m_timer, static_cast<void (QTimer::*)()>(&QTimer::start) );
    }
}
