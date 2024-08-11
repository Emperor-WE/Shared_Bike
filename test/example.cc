#include "bike.pb.h"
#include <iostream>
#include <string>

using namespace tutorial;

/*
int main(void) {
    std::string data;   //存储序列化信息

    //客户端发送的请求
    {
        mobile_request mr;
        mr.set_mobile("12345678901");

        mr.SerializePartialToString(&data);

        std::cout << "序列化后的数据[" << data.length() << "]:" << data << std::endl;
        std::cout << "type:" << std::hex << (int)*((char*)data.c_str()) << std::endl;
        std::cout << "length:" <<std::hex << (int)*((char*)data.c_str()+1) << std::endl;
    }

    //服务端接受请求
    {
        mobile_request mr;
        mr.ParseFromString(data);
        std::cout << "客户端手机号码:" << mr.mobile() << std::endl;
    }

    return 0;
}
*/

int main(void) {
    std::string data;   //存储序列化信息

    //客户端发送的请求
    {
        list_account_records_response larr;
        larr.set_code(200);
        larr.set_desc("ok");

        for(int i = 0; i < 5; i++) {
            list_account_records_response_account_record * ar = larr.add_records();
            ar->set_type(0);
            ar->set_limit(i * 100);
            ar->set_timestamp(time(NULL));
        }

        printf("recoreds size : %d\n", larr.records_size());

        larr.SerializeToString(&data);
    }

    //服务端接受请求
    {
        list_account_records_response larr;
        larr.ParseFromString(data);

        printf("records size: %d\n", larr.records_size());
        printf("code: %d\n", larr.code());

        for(int i = 0; i < larr.records_size(); i++) {
            const list_account_records_response_account_record &ar = larr.records(i);
            printf("limit: %d, timestamp: %ld\n", ar.limit(), ar.timestamp());
        }
    }

    return 0;
}