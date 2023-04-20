#include "contactsmodel.h"
#include <QDebug>
#include <QGuiApplication>
#include <QJniObject>
#include <QJsonDocument>

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

void ContactsModel::populate(std::list<Contact> contacts)
{
    beginResetModel();
    contactsList.clear();
    for(const Contact& contact : contacts){
        contactsList.append(contact);
    }
    endResetModel();
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
