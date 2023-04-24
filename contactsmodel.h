#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QAbstractListModel>
#include "Contact.cpp"

class ContactsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ContactsModel(QObject *parent = nullptr);

    enum{
        NameRole = Qt::UserRole,
        PhoneNumberRole = Qt::EditRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Q_INVOKABLE void populate(std::list<Contact> contacts);

    QList<Contact> contactsList;

private:
    int getIndexOf(const Contact& contact);
    void refreshRemovedEntries(QList<Contact> &l1, QList<Contact> &l2);
    void refreshAddedEntries(QList<Contact> &l1, QList<Contact> &l2);
    void refreshModifiedEntries(QList<Contact> &l1, QList<Contact> &l2);
};

#endif // CONTACTSMODEL_H
