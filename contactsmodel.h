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
        PhoneNumberRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void populate(std::list<Contact> contacts);

    QList<Contact> contactsList;
};

#endif // CONTACTSMODEL_H
