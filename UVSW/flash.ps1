cls
Write-Output "----------------------------------------- Develtron Belid Flasher -----------------------------------------"
while(($inp = Read-Host -Prompt "Select a command F Flash Q Quit") ){
switch($inp){
   F {
    Clear-History
    Write-Host "Starting Flash"-ForegroundColor White
    
        idf.py.exe flash  | Tee-Object -file log.txt
       if($LASTEXITCODE -eq 0){
            Write-Host "Firmware uploaded" -ForegroundColor Green
       }else{
            Write-Host "Somthing went wrong"  -ForegroundColor Red
       }
       $mac =  Get-Content -Path .\log.txt -Raw
       $mac = $mac.Substring($mac.IndexOf("MAC:")+5, 17)
       $mac = $mac.Replace(":","_")
       if(($item = Get-Item -Path  $mac".txt" -ErrorAction SilentlyContinue)) {
            Remove-Item -Path $item
        } 
        git branch  |  Tee-Object -Variable 'gitstatus'
        Add-Content .\log.txt "GIT:"
        Add-Content .\log.txt $gitstatus
        git rev-parse HEAD  |  Tee-Object -Variable 'gitstatus'
        
        Add-Content .\log.txt $gitstatus
       Rename-Item -Path "log.txt" -NewName $mac".txt"
       Write-Host  $mac  -ForegroundColor Magenta
       Write-Host  "Press any key to continue"-ForegroundColor White
       [Console]::ReadKey()
       cls
    }
    T{
        $testtr =   "Executing action: flash Serial port COM3 Connecting.... Detecting chip type... Unsupported detection protocol, switching and trying again... Connecting..... Detecting chip type... ESP32 Running ninja in directory f:\esp_flash\belid\esp32controller\build Executing ninja flash... [1/5] cmd.exe /C cd /D F:\ESP_Flash\Belid\esp32controller\build\esp-idf\esptool_py && F:\ESP_Flash\Espressif\python_env\idf4.4_py3.8_env\Scripts\python.exe F:/ESP_Flash/Espressif/frameworks/esp-idf-v4.4/components/partition_table/check_sizes.py --offset 0x8000 partition --type app F:/ESP_Flash/Belid/esp32controller/build/partition_table/partition-table.bin F:/ESP_Flash/Belid/esp32controller/build/hello-world.bin hello-world.bin binary size 0xd47a0 bytes. Smallest app partition is 0x140000 bytes. 0x6b860 bytes (34%) free. [2/5] Performing build step for 'bootloader' [1/1] cmd.exe /C cd /D F:\ESP_Flash\Belid\esp32controller\build\bootloader\esp-idf\esptool_py && F:\ESP_Flash\Espressif\python_env\idf4.4_py3.8_env\Scripts\python.exe F:/ESP_Flash/Espressif/frameworks/esp-idf-v4.4/components/partition_table/check_sizes.py --offset 0x8000 bootloader 0x1000 F:/ESP_Flash/Belid/esp32controller/build/bootloader/bootloader.bin Bootloader binary size 0x56a0 bytes. 0x1960 bytes (23%) free. [2/3] cmd.exe /C cd /D F:\ESP_Flash\Espressif\frameworks\esp-idf-v4.4\components\esptool_py && F:\ESP_Flash\Espressif\tools\cmake\3.20.3\bin\cmake.exe -D IDF_PATH=F:/ESP_Flash/Espressif/frameworks/esp-idf-v4.4 -D SERIAL_TOOL=F:/ESP_Flash/Espressif/python_env/idf4.4_py3.8_env/Scripts/python.exe F:/ESP_Flash/Espressif/frameworks/esp-idf-v4.4/components/esptool_py/esptool/esptool.py --chip esp32 -D SERIAL_TOOL_ARGS=--before=default_reset --after=hard_reset write_flash @flash_args -D WORKING_DIRECTORY=F:/ESP_Flash/Belid/esp32controller/build -P F:/ESP_Flash/Espressif/frameworks/esp-idf-v4.4/components/esptool_py/run_serial_tool.cmake esptool.py esp32 -p COM3 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size 4MB 0x1000 bootloader/bootloader.bin 0x10000 hello-world.bin 0x8000 partition_table/partition-table.bin 0xe000 ota_data_initial.bin esptool.py v3.2-dev Serial port COM3 Connecting..... Chip is ESP32-D0WD-V3 (revision 3) Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None Crystal is 40MHz MAC: 34:94:54:e6:69:c0 Uploading stub... Running stub... Stub running... Changing baud rate to 460800 Changed. Configuring flash size... Flash will be erased from 0x00001000 to 0x00006fff... Flash will be erased from 0x00010000 to 0x000e4fff... Flash will be erased from 0x00008000 to 0x00008fff... Flash will be erased from 0x0000e000 to 0x0000ffff... Compressed 22176 bytes to 14193... Writing at 0x00001000... (100 %) Wrote 22176 bytes (14193 compressed) at 0x00001000 in 0.7 seconds (effective 241.5 kbit/s)... Hash of data verified. Compressed 870304 bytes to 515969... Writing at 0x00010000... (3 %) Writing at 0x0001bdbf... (6 %) Writing at 0x00027946... (9 %) Writing at 0x000341bc... (12 %) Writing at 0x0003ff29... (15 %) Writing at 0x0004a785... (18 %) Writing at 0x0004fde1... (21 %) Writing at 0x00055d3f... (25 %) Writing at 0x0005b3ef... (28 %) Writing at 0x000614eb... (31 %) Writing at 0x00066c0b... (34 %) Writing at 0x0006c677... (37 %) Writing at 0x00071c64... (40 %) Writing at 0x000773f6... (43 %) Writing at 0x0007cd92... (46 %) Writing at 0x000826ae... (50 %) Writing at 0x00088251... (53 %) Writing at 0x0008d567... (56 %) Writing at 0x00092936... (59 %) Writing at 0x00097b8f... (62 %) Writing at 0x0009d266... (65 %) Writing at 0x000a2a6a... (68 %) Writing at 0x000a8675... (71 %) Writing at 0x000ade6c... (75 %) Writing at 0x000b3f24... (78 %) Writing at 0x000ba62c... (81 %) Writing at 0x000bff0f... (84 %) Writing at 0x000c85c0... (87 %) Writing at 0x000d0ae8... (90 %) Writing at 0x000d688f... (93 %) Writing at 0x000dc683... (96 %) Writing at 0x000e1a6c... (100 %) Wrote 870304 bytes (515969 compressed) at 0x00010000 in 12.3 seconds (effective 563.8 kbit/s)... Hash of data verified. Compressed 3072 bytes to 128... Writing at 0x00008000... (100 %) Wrote 3072 bytes (128 compressed) at 0x00008000 in 0.1 seconds (effective 292.3 kbit/s)... Hash of data verified. Compressed 8192 bytes to 31... Writing at 0x0000e000... (100 %) Wrote 8192 bytes (31 compressed) at 0x0000e000 in 0.2 seconds (effective 433.6 kbit/s)... Hash of data verified.  Leaving... Hard resetting via RTS pin... DoneP"
        
       $testtr = $testtr.Substring($testtr.IndexOf("MAC:")+5, 17)
       $testtr = $testtr.Replace(":","_")
       Write-Host  $testtr  -ForegroundColor Magenta
    }
   Q {
       "Tak for denne gang"
       exit 
    }   
   default {"Invalid entry"}
   }
}

