### 27 july
- i have 2 nano's connected to an stm. the stm does dma recieve from one nano -the publisher, and dma transmit to another nano -the subscriber. everything works as intended except for two problems
	- messages are being forwarded to the subscriber continuously instead of being transmitted only when there's a message from the subscriber
	- ![[Pasted image 20250727041258.png]]
	- i have to restart transmit dma even though it's in circular mode. i dont have the same problem with receive dma. i only have to start it once. i am figuring out what the problem is
- ive added git to this project
- i need to figure out permanent communication paths for now. the dynamic ROS-like topic/node structure will take a lot more work. the first step would be to set up a permanent path between the existing modules.
- [DMA circular mode issue](https://chatgpt.com/c/68854b27-3ba8-8007-a93c-30b35e8cbc77)
- [#include <Arduino.h> // #include <HardwareSerial.h> // HardwareSerial...](https://www.perplexity.ai/search/include-arduino-h-include-hard-iZ3fW8_pS2C9Zq5DHbMjaA)
- for the continuous data transfer, both AI say the same fixes:

|Approach|Buffer Mode|Only Sends Once?|Complexity|
|---|---|---|---|
|Normal + Rx Restart|Normal|Yes|Simple|
|Circular + Change Check|Circular|Yes|Simple|
|Circular + IDLE|Circular|Yes, fewer spurious|More code|
- circular + change check doesnt seem usefull. i will do normal + rx_restart for and build the rest of the architecture around transmitting one byte for now, and when im transferring multiple bytes ill switch to circular + IDLE.
---
- okay that was of no use. i changed receive dma to normal mode and restarted it in it's own callback function if there was data received. literally nothing changed. 
- i also have a doubt: if i have to restart the transmit DMA even though it's in circular DMA mode -why does my same code with the restarts not work when i set the transmit DMA to normal mode?

- for now im gonna use the circular + change check ig, because that seems the most fool proof.
---
- that didnt work either. sigh. i have a feeling there's something wrong with the AI's explanation
---
- i think i might have to re-approach this project. the technology is there. instead of trying to make a generic prototype, i need to start getting more specific. since i will be setting up permanent communication paths for now, ill start rebuilding this entire project with the rover as the focus. to be specific:
	- i will make the publisher a LoRa receiver emulation -it will publish the same kind of data the LoRa receiver would publish.
	- the subscribers will be the traversal board and the robotic arm board (which means ill need to add another nano to this prototype board -ill probably switch away from breadboards and use protoboards)
	- ill add an on board computer publisher too. which will also publish emulated traversal data.
	- all the code on the stm32 will be specific to communicating the data between these four modules for now.

### 6 Aug
- made a protoboard circuit from the breadboard for easier prototyping

### 8 Aug
- made the prototype better. made a proper data standard.
- | type [1] | ID [2] | Data [64] |
- created the handleUART function that receives data, and IDs it into different buffers.
### 10 Aug
- the handleUART function is a blocking function -im suspecting that's why it's not working. i am trying it with interrupts
- interrupts doesnt seem to work either. it's almost like the processMessage function never gets called
- i opened a new project file. i tried the absolute basics -receive polling on uart 3 take that data and transmit polling on uart 1. i did this to check the connections. and good news the data transferred so the connections are right
- so i tried interrupt receive in the debug project file too. and it didnt work. and it didnt work because my buffer for receiving was 64 bytes. whereas the sender (nano) was sending only 4 bytes. the IRQ would fire only if all 64 bytes of the buffer are filled -and since there was only 4 bytes coming in the IRQ was never firing
- i changed the buffer to 4 bytes to test this theory (that chatgpt gave me) -and it works. so that was indeed the problem. 
- (the chatgpt chat that helped me find this out: [STM32 UART buffer fix](https://chatgpt.com/c/6897bcb8-a188-8328-9fa1-ae345273b807))
- so now i can either read the data byte by byte and reassemble it -or use DMA with IDLE detection. -im planning to do the latter since obviously reading the data byte by byte is gonna cause a huuge delay
----
- made chatgpt make a serial_latency_measure python file [Time difference measurement](https://chatgpt.com/c/6897c906-991c-8322-bdde-8d0c12395cf4) -will probably put it to real use
- it might not be as reliable as i thought. ill prolly need to use an oscilloscope
---
- im trying DMA with IDLE detection and going into quite a rabbit hole without a goal. the IDLE detection works, now i need to move on from the debugger project file and implement this into the main project file.
---
- when i translated the same logic as the debug project, now suddenly it wont work. and i have no clue why it doesnt.
- i added debug messages absolutely everywhere.
	- the stm32 starts
	- the callback function is called and the right huart is also called

- IMPORTANT: i messed up the connections for the publisher lmao -the rx and tx are connected in reverse. the rx of arduino is going to rx2 of stm32. i did some fixing on that and now uart2 also works.
---
- great news. with more debug messages everywhere, i figured out the progressMessage function is also being called -but there was a problem with the endianness of the incomming message
	- basically, the publisher and subscriber were printing in big endian -they were printing the message type (0x1000 and 0x1100) as it is 0x1000 and 0x1100
	- but the stm32 was reading these message types as little endian -as 0x0010 and 0x0011, so the if conditions inside the processMessage function which were checking for 0x1000 and 0x1100 were never triggered.
	- i added a small fix (using chatgpt's help) to fix this endianness problem
	- the chatgpt chat for all this work: [DMA reception issue](https://chatgpt.com/c/6898bfa5-145c-8327-8132-486add23bba6)
- now i have another problem where the program is not able to find the requested message in the UART records. this should be a simpler fix. im loving the progress im making
	- it was because i hadnt changed the endianness of the message itself -i just took out the type and ID of the message, swapped them and created seperate variables out of it.
	- i assigned these fixed variables back on the message and now it finds the message in the UART records
- now i have the next problem which is that the retrieved and sent back payload is all jumbled. it's not the same message the publisher is publishing
	- the random data was this
`Serial.print("received: ");`
`Serial.print(i);` 
`Serial.println();`
- the "received" was being printed in hex and that was what was confusing me.
---
- everything works well if i set the publishing and subscription speed low. but all hell breaks loose when i remove the artificial delays
- the minimum delay i can put (with all the debug messages in the code) is 100ms. the publisher can only publish once every 100ms but the subscriber can subscribe every 10ms. this delay is kinda huge. i need to bring down this delay to at least around 10ms or less somehow
- maybe the debug is what's causing the delay. tomorrow ill try removing the debug options and then test the fastest transmission speed.
### 11 Aug
- there's another issue where if the publisher stops publishing the subscriber keeps receiving old data. the buffer needs to clear up somehow.
	- i could to clear up the buffer when there's no data from that ID for a while
	- or i could pop elements off the buffer every time it's being transmitted
- also the memcpy operation is prolly taking up a shit ton of resources. i need to get rid of as many copy operations as possible
- need to remove the debug statements and test for delay as well

### 12 Aug
- realized i was overcomplicating the message type and ID transmit by trying to undo little endianness of msg.type and msg.ID. both stm32 and nano process 2 Byte data in little endian format, so i didnt need to worry about switching them around.
- the data being sent to the stm32 by the publisher was not being stored properly.
	- each UART port gets a record of type Message_t. the intention is the publisher on this UART port could publish n messages with different IDs. and all these messages would get stored in the different indexes on the record for later use when sending
	- what was actually happening was the messages were using this record as kind of a buffer. a new message even if it was of the same ID would get stored in index 0 the first time, and then index 1, and then index 2
	- the fix was simple. initially i was checking if the index was empty or not. now i compare the message ID and the record's message ID and if theyre the same i store it in the same index

- there is this problem where my log prints are double for each communication with my modules.
	- like if publisher publishes something, that publishing should be received and the rxcpltcallback should be called only once. but here it's being called twice
	- this happens for both publishing and subscribing.
	- refer to this chat on how i fixed it: [Code printing data twice](https://chatgpt.com/c/689b7961-f1ec-8327-813a-9429bb71bd38)
	- i disabled half transfer interrupts using `__HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);` after each initialization of the rx DMA (including re arming the DMA in callback)

---
- made the first properly working prototype. check the video file named first working prototype