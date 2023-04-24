#include "contactsmodel.h"
#include <QDebug>
#include <QGuiApplication>
#include <QJniObject>
#include <QJsonDocument>
#include <set>

ContactsModel *m_instance = nullptr;

ContactsModel::ContactsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_instance = this;
    auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
    activity.callMethod<void>("getContacts");
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return contactsList.size();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role){
        case NameRole:
            return QVariant(contactsList[index.row()].getName());
        case PhoneNumberRole:
            return QVariant(contactsList[index.row()].getPhoneNumber());
    }

    return QVariant();
}

QHash<int, QByteArray> ContactsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[NameRole] = "name";
    roleNames[PhoneNumberRole] = "phoneNumber";

    return roleNames;
}

bool ContactsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        // Set data in model here. It can also be a good idea to check whether the new value actually differs from the current value
        if (contactsList[index.row()].getPhoneNumber() != value.toString()) {
            contactsList[index.row()].setPhoneNumber(value.toString());
            emit dataChanged(index, index, {role});
            return true;
        }
    }
    return false;
}

void ContactsModel::populate(std::list<Contact> contacts)
{
    if(contactsList.isEmpty()){
        beginResetModel();
        contactsList.clear();
        for(const Contact& contact : contacts){
            contactsList.append(contact);
        }
        endResetModel();
    }else{
        QList<Contact>androidContacts = QList(contacts.begin(), contacts.end());

        // Refresh the removed entries
        refreshRemovedEntries(androidContacts, contactsList);

        // Refresh the added entries
        refreshAddedEntries(androidContacts, contactsList);

        // refresh modified entries
        refreshModifiedEntries(androidContacts, contactsList);
    }
}

void ContactsModel::refreshRemovedEntries(QList<Contact> &l1, QList<Contact> &l2) {
    QList<Contact>::iterator it1 = l1.begin(), it2 = l2.begin();
    int removeIndex = 0;
    QModelIndex qModelIndex = QModelIndex();

    while (it1 != l1.end() && it2 != l2.end()) {
        if (*it1 == *it2) {
            ++it1;
            ++it2;
            ++removeIndex;
        } else if (it1->getName() < it2->getName()) {
            ++it1;
        } else {
            // elements removed
            while (it2 != l2.end()) {
                if (it2->getName() < it1->getName()) {
                    beginRemoveRows(qModelIndex, removeIndex, removeIndex);
                    it2 = l2.erase(it2);
                    endRemoveRows();
                } else {
                    break;
                }
            }
        }
    }

    while (it2 != l2.end()) {
        beginRemoveRows(qModelIndex, removeIndex, removeIndex);
        it2 =l2.erase(it2);
        endRemoveRows();
    }
}

void ContactsModel::refreshAddedEntries(QList<Contact> &l1, QList<Contact> &l2)
{
    QList<Contact>::iterator it1 = l1.begin(), it2 = l2.begin();
    int startIndex = -1;
    int endIndex = -1;
    QModelIndex qModelIndex = QModelIndex();

    while (it1 != l1.end() && it2 != l2.end()) {
        if (startIndex != endIndex) {
            startIndex = endIndex;
        }
        if (*it1 == *it2) {
            ++it1;
            ++it2;
            startIndex = endIndex + 1;
            endIndex = startIndex;
        }  else if (it1->getName() > it2->getName()) {
            ++it2;
        } else {
            // elements added
            while (it1 != l1.end()) {
                if (it2->getName() > it1->getName()) {
                int insertIndex = ++endIndex;
                   beginInsertRows(qModelIndex, insertIndex, insertIndex);
                   l2.insert(it2, *it1);
                   endInsertRows();
                } else {
                    break;
                }
                ++it1;
            }
        }
    }

    while (it1 != l1.end()) {
        int insertIndex = l1.size() - 1;
        beginInsertRows(qModelIndex, insertIndex, insertIndex);
        l2.insert(it2, *it1);
        endInsertRows();
        it1++;
        insertIndex++;
    }
}

void ContactsModel::refreshModifiedEntries(QList<Contact> &l1, QList<Contact> &l2)
{
    int elemRowIndex = 0;

    // Both lists are of same size
    for (QList<Contact>::iterator it1 = l1.begin(), it2 = l2.begin(); it1 != l1.end(); ++it1, ++it2, ++elemRowIndex) {
            // if number is not same, update it
        if(it1->getPhoneNumber() != it2->getPhoneNumber())
            setData(index(elemRowIndex, 0, QModelIndex()), QVariant(it1->getPhoneNumber()), PhoneNumberRole);
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_test_ContactsActivity_onContactsReceived(JNIEnv* env, jobject obj, jstring contactsJsonStr) {

    QVariantList qJsonDoc = QJsonDocument::fromJson(env->GetStringUTFChars(contactsJsonStr,0)).toVariant().toList();

    std::list<Contact> contacts;
    QVariantList::iterator it;
    for(it = qJsonDoc.begin(); it != qJsonDoc.end(); ++it)
    {
        QVariantMap contactMap = (*it).toMap();

        QString name = contactMap["name"].toString();
        QString phoneNumber = contactMap["phoneNumber"].toString();

       contacts.push_back(Contact(name, phoneNumber));
    }

    m_instance->populate(contacts);
}
