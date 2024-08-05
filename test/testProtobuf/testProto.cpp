#include <iostream>
#include <string>

#include "testProto.pb.h"

using namespace testProtobuf;


int main() {
    // 创建一个 Person 对象并设置其字段
    Person person;
    person.set_id(123);
    person.set_name("John Doe");
    person.set_email("johndoe@example.com");

    Person::PhoneNumber* phone_number = person.add_phones();
    phone_number->set_number("555-4321");
    phone_number->set_type(Person::MOBILE);
    phone_number = person.add_phones();
    phone_number->set_number("555-4322");
    phone_number->set_type(Person::HOME);

    std::string str;
    if(person.SerializeToString(&str)) {
        std::cout << "Serialize success" << std::endl;
        std::cout << "Serialize data: " << str << std::endl;
    } else {
        std::cout << "Serialize failed" << std::endl;
    }

    // 反序列化
    Person person2;
    if(person2.ParseFromString(str)) {
        std::cout << "Parse success" << std::endl;
        std::cout << "Parse data: " << std::endl;
        std::cout << "id: " << person2.id() << std::endl;
        std::cout << "name: " << person2.name() << std::endl;
        std::cout << "email: " << person2.email() << std::endl;
        for(int i = 0; i < person2.phones_size(); i++) {
            const Person::PhoneNumber& phone_number = person2.phones(i);
            std::cout << "phone number: " << phone_number.number() << std::endl;
            std::cout << "phone type: " << phone_number.type() << std::endl;
        }

    } else {
        std::cout << "Parse failed" << std::endl;
    }

}