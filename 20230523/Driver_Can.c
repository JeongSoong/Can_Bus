/***********************************************************************/
/*Include*/ 
/***********************************************************************/
#include "Driver_Can.h" // Driver_Can 헤더 파일을 포함


/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
AppLedType        g_led;  /* 전역 LED 구성 및 제어 구조체        */


/***********************************************************************/
/*Define*/ 
/***********************************************************************/


/***********************************************************************/
/*Typedef*/ 
/***********************************************************************/


/***********************************************************************/
/*Static Function Prototype*/ 
/***********************************************************************/




/* 인터럽트 서비스 루틴(ISR)을 정의하기 위한 매크로입니다. */

IFX_INTERRUPT(canIsrTxHandler, 0, ISR_PRIORITY_CAN_TX); // CAN TX 인터럽트에 대한 ISR을 정의

/***********************************************************************/
/*Variable*/ 
/***********************************************************************/
App_MulticanBasic g_MulticanBasic; /**< \brief 데모 정보 */
volatile CanRxMsg rec; // 수신된 CAN 메시지를 저장하기 위한 변수
int a[8]={0,0x1,0x2,0x3,0x4,0x5,0x6,0x7}; // 전송할 데이터 배열
 
/***********************************************************************/
/*Function*/ 
/***********************************************************************/

void Driver_Can_Init(void) /* CAN 드라이버 초기화 함수. */
{
    /* 모듈 구성 생성 */
    IfxMultican_Can_Config canConfig;
    IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);

    /* 인터럽트 구성 */
    canConfig.nodePointer[TX_INTERRUPT_SRC_ID].priority = ISR_PRIORITY_CAN_TX;

    /* 모듈 초기화 */
    IfxMultican_Can_initModule(&g_MulticanBasic.drivers.can, &canConfig);

    /* CAN 노드 구성 생성 */
    IfxMultican_Can_NodeConfig canNodeConfig;
    IfxMultican_Can_Node_initConfig(&canNodeConfig, &g_MulticanBasic.drivers.can);

    canNodeConfig.baudrate = 500000UL;  // CAN 통신 속도 500kbps로 설정
    {
        canNodeConfig.nodeId    = IfxMultican_NodeId_0;  // CAN 노드 ID를 0으로 설정
        canNodeConfig.rxPin     = &IfxMultican_RXD0B_P20_7_IN;  // 수신 핀 설정
        canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
        canNodeConfig.txPin     = &IfxMultican_TXD0_P20_8_OUT;  // 송신 핀 설정
        canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

        IfxMultican_Can_Node_init(&g_MulticanBasic.drivers.canSrcNode, &canNodeConfig);  // CAN 노드 초기화
    }

    /* 메시지 객체 구성 생성 */
    IfxMultican_Can_MsgObjConfig canMsgObjConfig;  // CAN 메시지 객체 구성
    IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_MulticanBasic.drivers.canSrcNode);

    canMsgObjConfig.msgObjId              = 0; // 메시지 객체 ID를 0으로 설정
    canMsgObjConfig.messageId             = 0x100; // 메시지 ID를 0x100으로 설정
    canMsgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;
    canMsgObjConfig.frame                 = IfxMultican_Frame_transmit; // 프레임 타입을 송신으로 설정                                    // CAN TX�� ����
    canMsgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8; // 데이터 길이를 8바이트로 설정                                  // Data ���̴� 8
    canMsgObjConfig.control.extendedFrame = FALSE; // 확장 ID를 사용하지 않음                                                         // Extended ID ��� ����
    canMsgObjConfig.control.matchingId    = TRUE;

    canMsgObjConfig.txInterrupt.enabled = TRUE;
    canMsgObjConfig.txInterrupt.srcId = TX_INTERRUPT_SRC_ID;

    /* 메시지 객체 초기화 */
    IfxMultican_Can_MsgObj_init(&g_MulticanBasic.drivers.canSrcMsgObj, &canMsgObjConfig);

    /* IO 포트 설정 */
    IfxPort_setPinModeOutput(STB, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    /* CAN 칩의 STB 핀 설정 (Active Low) */
    IfxPort_setPinLow(STB);
}


void Driver_Can_TxTest(void)
{
    /* CAN TX 테스트 함수 */
    const uint32 dataLow  = 0x12340000;
    const uint32 dataHigh = 0x9abc0000;

    /* 데이터 전송 */
    {
        IfxMultican_Message msg;
        IfxMultican_Message_init(&msg, 0x100, dataLow, dataHigh, IfxMultican_DataLengthCode_8);

        while (IfxMultican_Can_MsgObj_sendMessage(&g_MulticanBasic.drivers.canSrcMsgObj, &msg) == IfxMultican_Status_notSentBusy)
        {}
    }
}


void CAN_send(CanRxMsg *message)
{
    /* CAN 메시지를 전송하는 함수 */

    IfxMultican_Message msg;

    const unsigned dataLow = message->Data[0]|(message->Data[1]<<8)|(message->Data[2]<<16)|(message->Data[3]<<24);
    const unsigned dataHigh = message->Data[4]|(message->Data[5]<<8)|(message->Data[6]<<16)|(message->Data[7]<<24);


    IfxMultican_Message_init(&msg,message->ID,dataLow,dataHigh,message->DLC); // CAN 메시지 초기화

    while (IfxMultican_Can_MsgObj_sendMessage(&g_MulticanBasic.drivers.canSrcMsgObj, &msg) == IfxMultican_Status_notSentBusy)
    {
        // 메시지 전송이 진행 중인 동안 대기
    }
}


void CAN_TEST(void)
{
    /* CAN 테스트 함수 */
    CanRxMsg MES;
    int i=0;
    MES.ID=0x890; // CAN 메시지의 ID 설정
    MES.IDE=0; // 표준 ID 사용
    MES.DLC=8; // 데이터 길이 8바이트로 설정
    for(i=0; i<8; i++)
    {
        MES.Data[i]=a[i]; // CAN 메시지의 데이터 설정
    }
    CAN_send(&MES); // CAN 메시지 전송 함수 호출
}


/* TX 인터럽트가 발생한 후 호출되는 인터럽트 서비스 루틴 (ISR).
 LED1을 켜서 CAN 메시지 전송이 성공했음을 나타냄.
 */
void canIsrTxHandler(void)
{
    // LED1을 켜서 CAN 메시지가 전송되었음을 나타냄 */
    blinkLED1();

    //IfxPort_setPinLow(g_led.led1.port, g_led.led1.pinIndex);
}


void initLed(void)
{
    /* ======================================================================
     * LED에 연결된 핀의 설정:
     * ======================================================================
     * - GPIO 포트 정의
     * - LED에 연결된 GPIO 핀 정의
     * - 일반 GPIO 핀 사용으로 설정
     * - 패드 드라이버 강도 설정
     * ======================================================================
     */
    g_led.led1.port      = &MODULE_P00;
    g_led.led1.pinIndex  = PIN5;
    g_led.led1.mode      = IfxPort_OutputIdx_general;
    g_led.led1.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    g_led.led2.port      = &MODULE_P00;
    g_led.led2.pinIndex  = PIN6;
    g_led.led2.mode      = IfxPort_OutputIdx_general;
    g_led.led2.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    /* LED에 연결된 핀의 초기 상태를 "높음(HIGH)"으로 설정하여 기본적으로 LED를 꺼둠 */
    IfxPort_setPinHigh(g_led.led1.port, g_led.led1.pinIndex);
    IfxPort_setPinHigh(g_led.led2.port, g_led.led2.pinIndex);

    /* LED에 연결된 핀의 입출력 모드 설정 */
    IfxPort_setPinModeOutput(g_led.led1.port, g_led.led1.pinIndex, IfxPort_OutputMode_pushPull, g_led.led1.mode);
    IfxPort_setPinModeOutput(g_led.led2.port, g_led.led2.pinIndex, IfxPort_OutputMode_pushPull, g_led.led2.mode);

    /* LED에 연결된 핀의 패드 드라이버 모드 설정 */
    IfxPort_setPinPadDriver(g_led.led1.port, g_led.led1.pinIndex, g_led.led1.padDriver);
    IfxPort_setPinPadDriver(g_led.led2.port, g_led.led2.pinIndex, g_led.led2.padDriver);
}


void blinkLED1(void)
{
    //IfxPort_togglePin(LED1);

    IfxPort_setPinHigh(LED1); // LED1을 켜서 LED를 깜박이도록 설정
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_1ms));     // 1밀리초 동안 대기
    IfxPort_setPinLow(LED1);  // LED1을 끄고 LED를 깜박이는 동작 완료
}
void blinkLED2(void)
{
    IfxPort_togglePin(LED2);  // LED2 상태를 Toggle
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_10ms));    //10밀리초 동안 대기
}
