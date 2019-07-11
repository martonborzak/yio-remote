#include <QtDebug>
#include <QJsonDocument>
#include <QJsonArray>

#include "homeassistant.h"
#include "../../entities/entity.h"
#include "math.h"

HomeAssistant::HomeAssistant()
{
    m_webSocketId = 4;

    m_websocketReconnect.setSingleShot(true);
    m_websocketReconnect.setInterval(2000);
    m_websocketReconnect.stop();

    QObject::connect(&m_socket, SIGNAL(textMessageReceived(const QString &)), this, SLOT(onTextMessageReceived(const QString &)));
    QObject::connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    QObject::connect(&m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));

    QObject::connect(&m_websocketReconnect, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void HomeAssistant::initialize(int integrationId, const QVariantMap& config, QObject* entities)
{
    setIntegrationId(integrationId);
    for (QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter) {
        if (iter.key() == "data") {
            QVariantMap map = iter.value().toMap();
            m_ip = map.value("ip").toString();
            m_token = map.value("token").toString();
        }
        else if (iter.key() == "type")
            setType(iter.value().toString());
        else if (iter.key() == "friendly_name")
            setFriendlyName(iter.value().toString());
    }
    m_entities = qobject_cast<EntitiesInterface *>(entities);
}

void HomeAssistant::connect()
{
    setState(CONNECTING);

    // reset the reconnnect trial variable
    m_tries = 0;

    // turn on the websocket connection
    QString url = QString("ws://").append(m_ip).append("/api/websocket");
    m_socket.open(QUrl(url));
}

void HomeAssistant::disconnect()
{
    // turn off the socket
    m_socket.close();

    // turn of the reconnect try
    m_websocketReconnect.stop();

    setState(DISCONNECTED);

}

void HomeAssistant::sendCommand(const QString& type, const QString& entity_id, const QString& command, const QVariant& param)
{
    if (type == "light") {
        if (command == "TOGGLE")
            webSocketSendCommand(type, "toggle", entity_id, NULL);
        else if (command == "ON")
            webSocketSendCommand(type, "turn_on", entity_id, NULL);
        else if (command == "OFF")
            webSocketSendCommand(type, "turn_off", entity_id, NULL);
        else if (command == "BRIGHTNESS") {
            QVariantMap data;
            data.insert("brightness_pct", param);
            webSocketSendCommand(type, "turn_on", entity_id, &data);
        }
        else if (command == "COLOR") {
            QColor color = param.value<QColor>();
            QVariantMap data;
            QVariantList list;
            list.append(color.red());
            list.append(color.green());
            list.append(color.blue());
            data.insert("rgb_color", list);
            webSocketSendCommand(type, "turn_on", entity_id, &data);
        }
    }
    if (type == "blind") {
    }
}

void HomeAssistant::onTextMessageReceived(const QString &message)
{
    QJsonParseError parseerror;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseerror);
    if (parseerror.error != QJsonParseError::NoError) {
        qDebug() << "JSON error : " << parseerror.errorString();
        return;
    }
    QVariantMap map = doc.toVariant().toMap();

    QString m = map.value("error").toString();
    if (m.length() > 0) {
        qDebug() << "error : " << m;
    }

    QString type = map.value("type").toString();
    int id = map.value("id").toInt();
    bool success = map.value("id").toBool();

    if (type == "auth_required") {
        QString auth = QString("{ \"type\": \"auth\", \"access_token\": \"%1\" }\n").arg(m_token);
        m_socket.sendTextMessage(auth);
        return;
    }

    if (type == "auth_ok") {
        qDebug() << "Connection successful";
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // FETCH STATES
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        m_socket.sendTextMessage("{\"id\": 2, \"type\": \"get_states\"}\n");
    }

    if (success && id == 2) {
        QVariantList list = map.value("result").toJsonArray().toVariantList();
        for (int i = 0; i < list.size(); i++) {
            QVariantMap result = list.value(i).toMap();
            updateEntity(result.value("entity_id").toString(), result);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // SUBSCRIBE TO EVENTS IN HOME ASSISTANT
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        m_socket.sendTextMessage("{\"id\": 3, \"type\": \"subscribe_events\", \"event_type\": \"state_changed\"}\n");
    }

    if (success && type == "result" && id == 3) {
        setState(CONNECTED);
        qDebug() << "Subscribed to state changes";
    }

    if (success && id == m_webSocketId) {
        qDebug() << "Command successful";
    }

    if (type == "event" && id == 3) {
        QVariantMap data = map.value("event").toMap().value("data").toMap();
        QVariantMap newState = data.value("new_state").toMap();
        updateEntity(data.value("entity_id").toString(), newState);
    }
}

void HomeAssistant::onStateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::UnconnectedState) {
        setState(DISCONNECTED);
        m_websocketReconnect.start();
    }
}

void HomeAssistant::onError(QAbstractSocket::SocketError error)
{
    m_socket.close();
    setState(DISCONNECTED);
    m_websocketReconnect.start();
}

void HomeAssistant::onTimeout()
{
    if (m_tries == 1) {
        m_websocketReconnect.stop();

        emit notify();
        disconnect();
        m_tries = 0;
    }
    else {
        m_webSocketId = 4;
        if (state() != CONNECTING)
        {
            setState(CONNECTING);
        }
        QString url = QString("ws://").append(m_ip).append("/api/websocket");
        m_socket.open(QUrl(url));

        m_tries++;
    }
}

void HomeAssistant::webSocketSendCommand(const QString& domain, const QString& service, const QString& entity_id, QVariantMap *data)
{
    // sends a command to home assistant
    m_webSocketId++;

    QVariantMap map;
    map.insert("id", QVariant(m_webSocketId));
    map.insert("type", QVariant("call_service"));
    map.insert("domain", QVariant(domain));
    map.insert("service", QVariant(service));

    if (data == NULL) {
        QVariantMap d;
        d.insert("entity_id", QVariant(entity_id));
        map.insert("service_data", d);
    }
    else {
        data->insert("entity_id", QVariant(entity_id));
        map.insert("service_data", *data);
    }
    QJsonDocument doc = QJsonDocument::fromVariant(map);
    QString message = doc.toJson(QJsonDocument::JsonFormat::Compact);
    m_socket.sendTextMessage(message);

}

int HomeAssistant::convertBrightnessToPercentage(float value)
{
    return int(round(value/255*100));
}

void HomeAssistant::updateEntity(const QString& entity_id, const QVariantMap& attr)
{
    Entity* entity = (Entity*)m_entities->get(entity_id);
    if (entity) {
        if (entity->type() == "light") {
            updateLight(entity, attr);
        }
        //m_entities->update(entity_id, attr);
    }
}

void HomeAssistant::updateLight(Entity* entity, const QVariantMap& attr)
{
    QVariantMap attributes;

    // state
    if (attr.value("state").toString() == "on") {
        attributes.insert("state", true);
    } else {
        attributes.insert("state", false);
    }

    // brightness
    if (entity->supported_features().indexOf("BRIGHTNESS") > -1) {
        if (attr.value("attributes").toMap().value("brightness").toInt()) {
            attributes.insert("brightness", convertBrightnessToPercentage(attr.value("attributes").toMap().value("brightness").toInt()));
        } else {
            attributes.insert("brightness", 0);
        }
    }

    // color
    if (entity->supported_features().indexOf("COLOR") > -1) {
        QVariant color = attr.value("attributes").toMap().value("rgb_color");
        QVariantList cl(color.toList());
        char buffer[10];
        sprintf(buffer, "#%02X%02X%02X", cl.value(0).toInt(), cl.value(1).toInt(), cl.value(2).toInt());
        attributes.insert("color", buffer);
    }

    // color temp
    if (entity->supported_features().indexOf("COLORTEMP") > -1) {

    }

    m_entities->update(entity->entity_id(), attributes);
}