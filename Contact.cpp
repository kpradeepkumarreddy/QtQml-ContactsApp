#include <QAbstractListModel>

class Contact{
public:
    Contact(){}
    Contact(const QString &name, const QString &phoneNumber){
        this->name = name;
        this->phoneNumber = phoneNumber;
    }
    QString getName() const{
        return name;
    }
    void setName(const QString &newName){
        name = newName;
    }

    QString getPhoneNumber() const{
        return phoneNumber;
    }
    void setPhoneNumber(const QString &newPhoneNumber){
        phoneNumber = newPhoneNumber;
    }
    bool operator< (const Contact& contact) const{
        return this->name < contact.name;
    }
    bool operator== (const Contact& contact) const{
        return (this->name == contact.name);
    }
private:
    QString name;
    QString phoneNumber;
};
