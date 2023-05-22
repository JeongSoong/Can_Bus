#ifndef DRIVER_CAN
#define DRIVER_CAN

/***********************************************************************/
/* Include */
/***********************************************************************/
#include "Ifx_Types.h"          // Ifx 데이터 타입 정의 헤더
#include "IfxCpu.h"             // IfxCpu API 헤더
#include "IfxMultican_Can.h"    // IfxMultican_Can API 헤더
#include "IfxPort.h"            // IfxPort API 헤더
#include "Bsp.h"                // Bsp API 헤더


/***********************************************************************/
/* Define */
/***********************************************************************/

#define STB         &MODULE_P20,6    // STB 핀 정의

#define TX_INTERRUPT_SRC_ID     IfxMultican_SrcId_0    // 전송 인터럽트 서비스 요청 ID 정의
#define ISR_PRIORITY_CAN_TX     2                       // CAN TX 인터럽트 우선순위 정의
#define PIN5                    5                       // LED1은 이 핀에 연결되어 있음
#define PIN6                    6                       // LED2는 이 핀에 연결되어 있음


#define WAIT_TIME_1ms           1           // 1밀리초 대기 시간 정의
#define WAIT_TIME_5ms           5           // 5밀리초 대기 시간 정의
#define WAIT_TIME_10ms          10          // 10밀리초 대기 시간 정의
#define WAIT_TIME_20ms          20          // 20밀리초 대기 시간 정의
#define WAIT_TIME_50ms          50          // 50밀리초 대기 시간 정의
#define WAIT_TIME_100ms         100         // 100밀리초 대기 시간 정의

#define LED1         &MODULE_P00,5         // LED1이 연결된 핀 정의
#define LED2         &MODULE_P00,6         // LED2가 연결된 핀 정의


/***********************************************************************/
/* Typedef */
/***********************************************************************/
typedef struct
{
    struct
    {
        IfxMultican_Can        can;                // CAN 드라이버 핸들
        IfxMultican_Can_Node   canSrcNode;         // CAN 소스 노드
        IfxMultican_Can_MsgObj canSrcMsgObj;       // CAN 소스 메시지 객체
    } drivers;
} App_MulticanBasic;

typedef struct
{
    unsigned long ID;           // CAN 메시지 ID
    unsigned char IDE;          // CAN 메시지 IDE
    unsigned char DLC;          // CAN 데이터 길이
    unsigned char Data[8];      // CAN 데이터
} CanRxMsg;

typedef struct
{
    IfxPort_Pin_Config led1;     // LED1 설정 구조체
    IfxPort_Pin_Config led2;     // LED2 설정 구조체
} AppLedType;


/***********************************************************************/
/* External Variable */
/***********************************************************************/
IFX_EXTERN App_MulticanBasic g_MulticanBasic;    // 외부 변수 선언


/***********************************************************************/
/* Global Function Prototype */
/***********************************************************************/
extern void Driver_Can_Init(void);        // CAN 드라이버 초기화 함수
extern void Driver_Can_TxTest(void);      // CAN 메시지 송신 테스트 함수
extern void CAN_send(CanRxMsg *message);  // 수신된 CAN 메시지를 송신하는 함수
extern void CAN_TEST(void);               // CAN 테스트 함수
void initLed(void);                       // LED 초기화 함수
void blinkLED1(void);                     // LED1을 깜박이는 함수
void blinkLED2(void);                     // LED2를 깜박이는 함수
#endif /* DRIVER_CAN */
