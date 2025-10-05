// Vị trí: /src/repositories/channelrepository.cpp
// Phiên bản: 1.2 (Thêm chức năng kiểm tra link tồn tại)
// Mô tả: Triển khai logic cho ChannelRepository.

#include "channelrepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

ChannelRepository::ChannelRepository(QSqlDatabase &db) : m_db(db) {}

QList<Channel> ChannelRepository::loadAllChannels()
{
    QList<Channel> channels;
    QSqlQuery query("SELECT id, name, link, display_order, is_hidden FROM channels ORDER BY display_order ASC, name ASC", m_db);

    if (!query.exec()) {
        qCritical() << "Failed to load channels:" << query.lastError().text();
        return channels;
    }

    while (query.next()) {
        Channel ch;
        ch.id = query.value(0).toInt();
        ch.name = query.value(1).toString();
        ch.link = query.value(2).toString();
        ch.displayOrder = query.value(3).toInt();
        ch.isHidden = query.value(4).toBool();
        channels.append(ch);
    }
    return channels;
}

QList<Channel> ChannelRepository::loadVisibleChannels()
{
    QList<Channel> channels;
    QSqlQuery query("SELECT id, name, link, display_order, is_hidden FROM channels WHERE is_hidden = 0 ORDER BY display_order ASC, name ASC", m_db);

    if (!query.exec()) {
        qCritical() << "Failed to load visible channels:" << query.lastError().text();
        return channels;
    }

    while (query.next()) {
        Channel ch;
        ch.id = query.value(0).toInt();
        ch.name = query.value(1).toString();
        ch.link = query.value(2).toString();
        ch.displayOrder = query.value(3).toInt();
        ch.isHidden = query.value(4).toBool();
        channels.append(ch);
    }
    return channels;
}


Channel ChannelRepository::getChannelById(int id)
{
    Channel ch;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, link, display_order, is_hidden FROM channels WHERE id = ?");
    query.addBindValue(id);

    if (query.exec() && query.next()) {
        ch.id = query.value(0).toInt();
        ch.name = query.value(1).toString();
        ch.link = query.value(2).toString();
        ch.displayOrder = query.value(3).toInt();
        ch.isHidden = query.value(4).toBool();
    }
    return ch;
}


bool ChannelRepository::addChannel(const QString& name, const QString& link)
{
    QSqlQuery query(m_db);
    query.exec("SELECT MAX(display_order) FROM channels");
    int maxOrder = 0;
    if (query.next()) {
        maxOrder = query.value(0).toInt();
    }
    
    query.prepare("INSERT INTO channels (name, link, display_order, is_hidden) VALUES (?, ?, ?, 0)");
    query.addBindValue(name);
    query.addBindValue(link);
    query.addBindValue(maxOrder + 1);

    if (!query.exec()) {
        qCritical() << "Failed to add channel:" << query.lastError().text();
        return false;
    }
    return true;
}

bool ChannelRepository::updateChannel(const Channel& channel)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE channels SET name = ?, link = ?, is_hidden = ? WHERE id = ?");
    query.addBindValue(channel.name);
    query.addBindValue(channel.link);
    query.addBindValue(channel.isHidden);
    query.addBindValue(channel.id);

    if (!query.exec()) {
        qCritical() << "Failed to update channel:" << query.lastError().text();
        return false;
    }
    return true;
}

bool ChannelRepository::deleteChannel(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM channels WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        qCritical() << "Failed to delete channel:" << query.lastError().text();
        return false;
    }
    return true;
}

bool ChannelRepository::updateChannelsOrder(const QList<Channel>& channels)
{
    m_db.transaction();
    QSqlQuery query(m_db);
    query.prepare("UPDATE channels SET display_order = ? WHERE id = ?");

    for (int i = 0; i < channels.size(); ++i) {
        query.bindValue(0, i);
        query.bindValue(1, channels.at(i).id);
        if (!query.exec()) {
            qCritical() << "Failed to update channel order for id" << channels.at(i).id << " Error:" << query.lastError().text();
            m_db.rollback();
            return false;
        }
    }
    return m_db.commit();
}

bool ChannelRepository::setChannelHidden(int channelId, bool isHidden)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE channels SET is_hidden = ? WHERE id = ?");
    query.addBindValue(isHidden);
    query.addBindValue(channelId);

    if (!query.exec()) {
        qCritical() << "Failed to update channel hidden state:" << query.lastError().text();
        return false;
    }
    return true;
}

bool ChannelRepository::channelLinkExists(const QString &link) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT 1 FROM channels WHERE link = ?");
    query.addBindValue(link);
    if (query.exec() && query.next()) {
        return true;
    }
    return false;
}

