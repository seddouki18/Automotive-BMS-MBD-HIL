"""Compile and run the actual CAN callback against minimal host stubs."""
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[1]
source = (root / "Firmware_STM32_Controller/BMS_STM32_HIL/Core/Src/main.c").read_text()
callback = source.split("/* USER CODE BEGIN 4 */", 1)[1].split("/* USER CODE END 4 */", 1)[0]
template = "#include <stdint.h>\n#include <stdbool.h>\n#include <assert.h>\ntypedef struct {int unused;} CAN_HandleTypeDef;\ntypedef struct {uint32_t IDE,RTR,StdId,DLC;} CAN_RxHeaderTypeDef;\n#define CAN_ID_STD 0\n#define CAN_RTR_DATA 0\n#define CAN_RX_FIFO0 0\n#define HAL_OK 0\n#define GPIO_PIN_SET 1\n#define GPIO_PIN_RESET 0\n#define RELAY_SAFETY_GPIO_Port 0\n#define RELAY_SAFETY_Pin 0\ntypedef float real32_T;\nCAN_RxHeaderTypeDef rxHeader, incoming;\nuint8_t rxData[8], payload[8]={0x10,0xCC,0x0D,0xAC,60};\nuint32_t last_can_rx_tick, dwt_cycles;\nfloat execution_time_us;\nstruct {float V_cell,I,T;} Controller_BMS_U;\nstruct {bool relay;} Controller_BMS_Y;\nstruct {uint32_t CYCCNT;} counter;\n#define DWT (&counter)\nint calls, writes, receive_ok=1;\nint HAL_CAN_GetRxMessage(CAN_HandleTypeDef*h,int f,CAN_RxHeaderTypeDef*r,uint8_t*d){*r=incoming;for(int i=0;i<8;i++)d[i]=payload[i];return receive_ok?0:1;}\nuint32_t HAL_GetTick(void){return 123;}\nvoid Controller_BMS_step(void){calls++;Controller_BMS_Y.relay=false;}\nvoid HAL_GPIO_WritePin(int p,int n,int v){writes++;}\nCALLBACK_SOURCE\nint main(void){\n CAN_HandleTypeDef h;\n incoming=(CAN_RxHeaderTypeDef){0,0,0x100,5};\n HAL_CAN_RxFifo0MsgPendingCallback(&h);\n assert(calls==1 && writes==1 && last_can_rx_tick==123);\n assert(Controller_BMS_U.I==35.0f && Controller_BMS_U.T==60.0f);\n for(int k=0;k<5;k++){\n  incoming=(CAN_RxHeaderTypeDef){0,0,0x100,5};\n  if(k==0)incoming.IDE=1;\n  if(k==1)incoming.RTR=1;\n  if(k==2)incoming.StdId=0x101;\n  if(k==3)incoming.DLC=4;\n  if(k==4)receive_ok=0;\n  last_can_rx_tick=0;\n  HAL_CAN_RxFifo0MsgPendingCallback(&h);\n  assert(calls==1 && writes==1 && last_can_rx_tick==0);\n }\n return 0;\n}\n"
with tempfile.TemporaryDirectory() as directory:
    work = Path(directory)
    (work / "test.c").write_text(template.replace("CALLBACK_SOURCE", callback))
    subprocess.run(["cc", "-std=c11", str(work / "test.c"), "-o", str(work / "test")], check=True)
    subprocess.run([str(work / "test")], check=True)
print("CAN callback regression checks passed")
