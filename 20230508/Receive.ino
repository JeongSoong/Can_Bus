#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

MCP_CAN CAN0(10);

void setup()
{
    Serial.begin(115200);
    pinMode(2, INPUT);       // 2번 핀에 INT핀이 연결되었습니다.
    pinMode(3, OUTPUT);
    if(CAN0.begin(CAN_500KBPS,MCP_8MHz) == CAN_OK) Serial.print("can init ok!!\r\n");
    else Serial.print("Can init fail!!\r\n");
}

void loop()
{
    if (!digitalRead(2))     // 데이터가 수신되면 2번핀이 LOW상태가 됩니다.
    {                                 
        CAN0.readMsgBuf(&len, rxBuf);  // 데이터의 길이와 데이터를 읽습니다.
        rxId = CAN0.getCanId();        // 데이터의 ID(식별자)를 읽습니다.
        Serial.print("ID: ");
        Serial.print(rxId, HEX);       // ID를 출력합니다.
        Serial.print("  Data: ");
        Serial.println(rxBuf[0], HEX); // 데이터를 출력합니다.
        if (rxBuf[0] == 1) {
            digitalWrite(3, HIGH);
        }
        else if (rxBuf[0] == 0) {
            digitalWrite(3, LOW);
        }
    }
}
