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
// 서버와 클라이언트의 D-Bus 이름을 상수로 선언
const char *const SERVER_BUS_NAME = "in.softprayog.add_server";
const char *const CLIENT_BUS_NAME = "in.softprayog.add_client";
// 서버와 클라이언트의 D-Bus 오브젝트 경로를 상수로 선언
const char *const SERVER_OBJECT_PATH_NAME = "/in/softprayog/adder";
const char *const CLIENT_OBJECT_PATH_NAME = "/in/softprayog/add_client";
// 메소드 이름을 상수로 선언
const char *const METHOD_NAME = "add_numbers";

DBusError dbus_error;  // D-Bus 에러를 처리하기 위한 DBusError 인스턴스
void print_dbus_error (char *str);  // D-Bus 에러 출력 함수

int main (int argc, char **argv)
{
    DBusConnection *conn;  // D-Bus 연결을 위한 포인터
    int ret;  // 함수 호출 결과를 저장하는 변수
    char speed[80], temperature[80];  // 사용자 입력을 받기 위한 문자열

    dbus_error_init (&dbus_error);  // dbus_error를 초기화

    conn = dbus_bus_get (DBUS_BUS_SESSION, &dbus_error);  // D-Bus에 연결하고 그 연결을 가리키는 포인터 얻기

    if (dbus_error_is_set (&dbus_error))  // D-Bus 연결 중에 오류가 발생하면 에러 메시지를 출력하고 종료
        print_dbus_error ("Connecting D-Bus Error");

    if (!conn) // D-Bus의 반환값이 정당한지 확인. 에러처리와 반환값 검증은 다른것
        exit(1);

    // 이후 부분은 D-Bus를 통해 서버에 메시지를 전송하고 응답을 기다리는 루프를 실행
    // 이 코드에서는 "속도"와 "온도"라는 두 가지 파라미터를 서버에 전송하니, 원하는 데이터가 있다면 여기서 바꾸기
    while (1) {
        printf ("Please type speed: ");  // 사용자에게 속도 값을 입력 받기
        fgets (speed, 78, stdin);

        printf ("Please type temperature: ");  // 사용자에게 온도 값을 입력 받기
        fgets (temperature, 78, stdin);

        // 엔터를 기준으로 speed와 temperature가 나뉨
        speed[strcspn(speed, "\n")] = 0;
        temperature[strcspn(temperature, "\n")] = 0;

        char message[160];
        // 입력받은 속도와 온도 값을 문자열 형태로 포매팅하기
        snprintf(message, sizeof(message), "speed=%s temperature=%s", speed, temperature);

        // 같은 이름의 CLIENT_BUS_NAME이 있는지 확인. 없다면 통과, 같은게 있다면 계속해서 시도
        while (1) {
            ret = dbus_bus_request_name (conn, CLIENT_BUS_NAME, 0, &dbus_error);  // D-Bus에서 이름을 요청

            if (ret == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) 
                break;  // 이름 요청이 성공하면 루프 종료

            if (ret == DBUS_REQUEST_NAME_REPLY_IN_QUEUE) {
                fprintf (stderr, "Waiting for the bus ... \n");
                sleep (1);  // 요청이 대기열에 있으면 1초 동안 대기
                continue;
            }
            if (dbus_error_is_set (&dbus_error))
                print_dbus_error ("CLIENT_BUS_NAME Fail"); // 지금까지 에러가 있는지 확인
        }
    
        // 이후 부분에서는 D-Bus를 통해 서버에 메시지를 전송하고 응답을 기다리는 과정이 이루어집니다.
        // DBus message는 메세지에 해당하는 이름들만 저장
        DBusMessage *request;

        // dbus_message_new_method_call 함수를 사용하여 D-Bus 메시지를 생성해서 request에 저장. -> 아직 dbus에 안들어감
        // 이 메시지는 특정 버스 이름, 객체 경로, 인터페이스, 그리고 메소드 이름을 메세지에 적용
        if ((request = dbus_message_new_method_call (SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, 
                           INTERFACE_NAME, METHOD_NAME)) == NULL) {
            fprintf (stderr, "Error in dbus_message_new_method_call\n");
            exit (1);
        }

        // 실제 보내고싶은 speed와 temperature는 DBusMessageIter를 통해 보내고 있음
        DBusMessageIter iter;
        // 메시지에 매개변수를 첨부하기 위해 dbus_message_iter_init_append 함수를 호출
        dbus_message_iter_init_append (request, &iter);
        char *ptr = message;

        // dbus_message_iter_append_basic 함수를 사용하여 메시지에 문자열 매개변수를 첨부. 즉 실제 데이터는 여기서 Message에  추가되고 있음
        if (!dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &ptr)) {
            fprintf (stderr, "Error in dbus_message_iter_append_basic\n");
            exit (1);
        }

        DBusPendingCall *pending_return;
        // dbus_connection_send_with_reply 함수의 호출 성공 여부
        // 여기서 conn과 request가 묶임
        if (!dbus_connection_send_with_reply (conn, request, &pending_return, -1)) {
            fprintf (stderr, "Error in dbus_connection_send_with_reply\n");
            exit (1);
        }

        // 호출된 함수의 객체 성공 여부
        if (pending_return == NULL) {
            fprintf (stderr, "pending return is NULL");
            exit (1);
        }

        dbus_connection_flush (conn);  // 모든 변경사항을 즉시 D-Bus에 전송 -> conn은 연결에 관여
                
        dbus_message_unref (request);  // 메시지를 unref하고, 더 이상 필요하지 않을 때 메모리를 해제 -> request는 정보에 관여

        // 응답 메세지가 에러가 아닐 경우에만 받은 데이터를 작업하도록 설정: 즉 응답 메세지가 에러라면 해당 데이터 작업 못하게끔
        dbus_pending_call_block (pending_return);  // pending_return가 준비될 때까지 블록

        DBusMessage *reply;
        // dbus_pending_call_steal_reply 함수를 사용하여 응답 메시지 받기
        if ((reply = dbus_pending_call_steal_reply (pending_return)) == NULL) {
            fprintf (stderr, "Error in dbus_pending_call_steal_reply");
            exit (1);
        }

        dbus_pending_call_unref (pending_return);  // 응답 메세지가 NULL이 아니면 작업 승인 & 대기 중인 호출에 대한 참조를 제거

        char *s;
        // dbus_message_get_args 함수를 사용하여 응답 메시지의 인수를 가져오기 : 응답 메세지 있다면 출력
        if (dbus_message_get_args (reply, &dbus_error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
            printf ("%s\n", s);
        }
        else {
             fprintf (stderr, "Did not get arguments in reply\n");
             exit (1);
        }

        dbus_message_unref (reply);  // 응답 메시지를 unref하고, 더 이상 필요하지 않을 때 메모리를 해제합

        // dbus_bus_release_name 함수를 호출하여 D-Bus에서 이전에 요청한 CLIENT_BUS_NAME 이름을 해제
        if (dbus_bus_release_name (conn, CLIENT_BUS_NAME, &dbus_error) == -1) {
             fprintf (stderr, "Error in dbus_bus_release_name\n");
             exit (1);
        }
    }

    return 0;
}


// D-Bus 에러 출력 함수
void print_dbus_error (char *str) 
{
    fprintf (stderr, "%s: %s\n", str, dbus_error.message);  // 에러 메시지를 표준 에러에 출력
    dbus_error_free (&dbus_error);  // dbus_error를 초기화
}
