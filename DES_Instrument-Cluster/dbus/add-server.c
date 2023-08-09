#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <dbus/dbus.h>


// 이름 정하기
// D-Bus 인터페이스 이름을 상수로 선언
const char *const INTERFACE_NAME = "in.softprayog.dbus_example";
// 서버의 D-Bus 이름을 상수로 선언
const char *const SERVER_BUS_NAME = "in.softprayog.add_server";
// 서버의 D-Bus 오브젝트 경로를 상수로 선언
const char *const SERVER_OBJECT_PATH_NAME = "/in/softprayog/adder";
// 메소드 이름을 상수로 선언
const char *const METHOD_NAME = "add_numbers";

DBusError dbus_error; // D-Bus 에러를 처리하기 위한 DBusError 인스턴스
void print_dbus_error (char *str); // D-Bus 에러 출력 함수 선언
// 서버 메세지 핸들러 함수 선언
DBusHandlerResult server_handle_message (DBusConnection *conn, DBusMessage *message, void *user_data);

// 서버 버추얼 테이블 초기화
DBusObjectPathVTable server_vtable = {
    .message_function = server_handle_message // 메세지 핸들러 함수 설정
};

int main (int argc, char **argv)
{
    DBusConnection *conn; // D-Bus 연결을 위한 포인터
    int ret; // 함수 호출 결과를 저장하는 변수

    dbus_error_init (&dbus_error); // dbus_error 초기화

    conn = dbus_bus_get (DBUS_BUS_SESSION, &dbus_error); // D-Bus에 연결하고 그 연결을 가리키는 포인터 얻기

    if (dbus_error_is_set (&dbus_error)) // D-Bus 연결 중에 오류가 발생하면 에러 메시지를 출력하고 종료
        print_dbus_error ("dbus_bus_get");

    if (!conn) // D-Bus의 반환값이 정당한지 확인. 에러처리와 반환값 검증은 다른것
        exit (1);

    // 같은 이름의 SERVER_BUS_NAME이 있는지 확인. 없다면 통과, 같은게 있다면 계속해서 시도
    while (1) {
        ret = dbus_bus_request_name (conn, SERVER_BUS_NAME, 0, &dbus_error); // D-Bus에서 이름을 요청

        if (ret == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) 
           break; // 이름 요청이 성공하면 루프 종료

        if (ret == DBUS_REQUEST_NAME_REPLY_IN_QUEUE) {
           fprintf (stderr, "Waiting for the bus ... \n");
           sleep (1); // 요청이 대기열에 있으면 1초 대기
           continue;
        }
        if (dbus_error_is_set (&dbus_error))
           print_dbus_error ("dbus_bus_request_name"); // 지금까지 에러가 있는지 확인
    }

    if (dbus_connection_register_object_path (conn, SERVER_OBJECT_PATH_NAME, 
            &server_vtable, NULL) == false) {
        fprintf (stderr, "Error in dbus_connection_register_object_path\n");
        exit (1);
    }

    // listen to messages from dbus
    while (1) {
        dbus_connection_read_write_dispatch (conn, -1);
    }

    return 0;
}

// D-Bus 에러 출력 함수
void print_dbus_error (char *str) 
{
    fprintf (stderr, "%s: %s\n", str, dbus_error.message);
    dbus_error_free (&dbus_error);
}

DBusHandlerResult server_handle_message (DBusConnection *conn, DBusMessage *message, void *user_data) // 서버 메시지 핸들러 함수 정의
{
    // 받아온 메세지, INTERFACCE_NAME, METHOD_NAME이 같다면
    if (dbus_message_is_method_call (message, INTERFACE_NAME, METHOD_NAME)) {
        DBusMessageIter iter; // DBusMessageIter 객체 선언 -> add-client 확인
        char *msg; // 문자열 포인터 선언

        dbus_message_iter_init (message, &iter); // 메시지 반복자 초기화

        if (dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_STRING) { // 반복자가 가리키는 인자의 타입이 문자열이면
            dbus_message_iter_get_basic (&iter, &msg); // 인자 값을 가져와 msg에 할당
        }

        printf("Received: %s\n", msg); // 받은 메시지 출력

        DBusMessage *reply; // DBusMessage 객체 포인터 선언

        if ((reply = dbus_message_new_method_return (message)) == NULL) { // 메서드 반환 메시지를 생성하는데 실패하면
            fprintf (stderr, "Error in dbus_message_new_method_return\n"); // 에러 메시지 출력
            exit (1); // 프로그램 종료
        }

        if (!dbus_message_append_args (reply, DBUS_TYPE_STRING, &msg, DBUS_TYPE_INVALID)) { // 메시지에 인자를 추가하는데 실패하면
            fprintf (stderr, "Error in dbus_message_append_args\n"); // 에러 메시지 출력
            exit (1); // 프로그램 종료
        }

        if (!dbus_connection_send (conn, reply, NULL)) { // 메시지를 전송하는데 실패하면
            fprintf (stderr, "Error in dbus_connection_send\n"); // 에러 메시지 출력
            exit (1); // 프로그램 종료
        }
        dbus_connection_flush (conn); // 모든 펜딩 중인 메시지를 버스로 전송

        dbus_message_unref (reply); // 메시지에 대한 참조를 해제

        return DBUS_HANDLER_RESULT_HANDLED; // 메시지 처리 완료를 반환
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED; // 메시지 처리되지 않음을 반환
}
