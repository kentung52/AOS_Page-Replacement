#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_FRAMES 100              // 最大Frame number設定
#define REFERENCE_STRING_LENGTH 120000  //  reference string的長度
#define MAX_RANGE 1200              //  reference string的數值範圍

// 定義頁面的結構體
typedef struct {
    int pageNumber;  // Page Number    
    bool referenced;  // 參考位，用於 Enhanced Second-Chance 算法
    bool modified;    // 修改位
    int frequency;    // 訪問頻率計數      
    int lastAccessTime; // 上次訪問的時間
    
} Page;

// 函數原型
void generate_random_reference_string(int referenceString[], int length, int range);
void generate_locality_reference_string(int referenceString[], int length, int range);
void generate_myself_reference_string(int referenceString[], int length, int range);

void reset_frames(Page frames[], int frameCount) {
    for (int j = 0; j < frameCount; j++) {
        frames[j].pageNumber = -1;  // 重置頁面號碼
        frames[j].modified = false;  
        frames[j].referenced = false; // 參考位初始為 false
    }
}

int fifo(Page frames[], int frameCount, int referenceString[], int length);
int optimal(Page frames[], int frameCount, int referenceString[], int length);
int enhanced_second_chance(Page frames[], int frameCount, int referenceString[], int length);
int customize(Page frames[], int frameCount, int referenceString[], int length);

int main() {
    srand(time(NULL));  // 確保每次執行都生成不同的隨機數
    int referenceString[REFERENCE_STRING_LENGTH];  // 存儲生成的 reference string
    int m;  // Mode Number
    printf("請選擇String Mode\n");
    printf("1. generate_random_reference_string\n");
    printf("2. generate_locality_reference_string\n");
    printf("3. generate_myself_reference_string\n");
    scanf("%d",&m);
        
    if(m == 1){
    	generate_random_reference_string(referenceString, REFERENCE_STRING_LENGTH, MAX_RANGE);  // 生成 random reference string

	} else if(m == 2){
		generate_locality_reference_string(referenceString, REFERENCE_STRING_LENGTH, MAX_RANGE);  // 生成 random reference string

	}else if(m == 3){
		generate_myself_reference_string(referenceString, REFERENCE_STRING_LENGTH, MAX_RANGE);  // 生成 locality reference string

	} else {
		printf("Input mode is incorre");
		exit(0);
	}

    

    // 為每個算法分配 frame的頁面結構數組
    Page frames[MAX_FRAMES] = {0};

    // 測試不同的 frame number：從 10 到 MAX_FRAMES，每次增加 10
    for (int frameCount = 10; frameCount <= MAX_FRAMES; frameCount += 10) {
        // 重置幀中的頁面號碼和參考位
        for (int j = 0; j < frameCount; j++) {
            frames[j].pageNumber = -1;  // 初始頁面號碼設為 -1，表示空幀
            frames[j].referenced = false; // 參考位初始為 false
            frames[j].modified = false; 
            
        }
        
        printf("Frame Count: %d\n", frameCount);
        printf("----------------------------\n");

        // 計算 FIFO 算法的頁面錯誤數
        int pageFaultsFIFO = fifo(frames, frameCount, referenceString, REFERENCE_STRING_LENGTH);
        
        // 重置幀
//        reset_frames(frames, frameCount);  // 重置幀
        
        // 計算 Optimal 算法的頁面錯誤數
        int pageFaultsOptimal = optimal(frames, frameCount, referenceString, REFERENCE_STRING_LENGTH);
        
        // 重置幀
//        reset_frames(frames, frameCount);  // 重置幀
        
        // 計算 Enhanced Second-Chance 算法的頁面錯誤數
        int pageFaultsESC = enhanced_second_chance(frames, frameCount, referenceString, REFERENCE_STRING_LENGTH);
        
        // 重置幀
//        reset_frames(frames, frameCount);  // 重置幀
                
         // 計算 Customize 算法的頁面錯誤數
        int pageFaultsCUS = customize(frames, frameCount, referenceString, REFERENCE_STRING_LENGTH);

       
    }

    return 0;  // 程序正常結束
}

// 隨機生成參考字符串的函數
void generate_random_reference_string(int referenceString[], int length, int range) {
	if (length <= 0 || range <= 0) {
        printf("Length and range must be positive integers.\n");
        return; // 參數檢查
    }
    
    for (int i = 0; i < length; i++) {
        referenceString[i] = rand() % range + 1;  // 隨機生成範圍為 1~1200 的數字
    }
    
}

// locality生成參考字符串的函數
void generate_locality_reference_string(int referenceString[], int length, int range) {
    int totalLength = 0;
    int localityRange = range / 10;  // 設置局部性範圍較小的值
    
    while (totalLength < length) {
        // 選擇一個隨機的局部範圍中心點
        int localityCenter = rand() % range;
        
        // 設置當前片段的長度
        int callLength = rand() % (length / 100) + (length / 200);
        
        // 確保不超過整體參考字串長度
        if (totalLength + callLength > length) {
            callLength = length - totalLength;
        }
        
        // 在局部範圍內生成隨機數字
        for (int i = 0; i < callLength; i++) {
            referenceString[totalLength + i] = (localityCenter + rand() % localityRange) % range + 1;
        }

        totalLength += callLength;
    }
    
}
void generate_myself_reference_string(int referenceString[], int length, int range) {
	int index = 0;

    while (index < length) {
        int value = (rand() % range) + 1;  // 隨機數字範圍在 1 到 1200
        int count = rand() % 10 + 1;  // 隨機生成連續相同數字的長度（1到10）

        // 確保不超過最大長度
        for (int j = 0; j < count && index < length; j++) {
            referenceString[index++] = value;
        }
    }
}



// FIFO 算法的實作
int fifo(Page frames[], int frameCount, int referenceString[], int length) {
    int pageFaults = 0;          // 頁面錯誤數量
    int queue[MAX_FRAMES];       // 用於 FIFO 的隊列，儲存幀的索引
    int front = 0, rear = 0;     // FIFO 指針，front 為最早插入的位置，rear 為下一個插入的位置
    int numPagesInFrames = 0;    // 當前幀中頁面的數量
    bool found;                 // 標記頁面是否找到
    bool modified[MAX_FRAMES] = {false}; // 用於標記每個頁面是否被修改
    int interrupt = 0;           // 中斷計數
    int disk_writes = 0;         // 寫入磁碟計數

    // 初始化隊列（設置為 -1，表示未使用）
    for (int i = 0; i < MAX_FRAMES; i++) {
        queue[i] = -1;
       
    }

    // 遍歷每個參考字符串
    for (int i = 0; i < length; i++) {
        found = false;  // 每次迭代重置為 false

        // 檢查頁面是否在幀中
        for (int j = 0; j < numPagesInFrames; j++) {
            if (frames[queue[j]].pageNumber == referenceString[i]) {
                found = true;  // 找到頁面                 
            	frames[queue[j]].modified = true;  // 標記該頁面為已修改       	 	               
                break;  // 結束內部循環
            }
        }

        if (!found) {  // 如果頁面不在幀中，則頁面錯誤
            pageFaults++;  // 增加頁面錯誤計數
            interrupt++;   //Page Fault Interrupt
            

            // 如果幀未滿，則直接插入頁面
            if (numPagesInFrames < frameCount) {
                frames[numPagesInFrames].pageNumber = referenceString[i];
                // 新插入的頁面是否為修改（假設偶數頁面是寫操作）       
            	frames[numPagesInFrames].modified = false; // 沒有修改
        	
                queue[rear] = numPagesInFrames;  // 記錄插入頁面的位置
                rear = (rear + 1) % frameCount;  // 更新 rear 指針
                numPagesInFrames++;  // 增加幀中頁面的數量
            } else {
                // 如果幀已滿，替換最早進入的頁面
                int replaceIndex = queue[front];  // 找到最早的頁面

                // 檢查是否需要寫入磁碟
                if (frames[replaceIndex].modified) {
                    disk_writes++;  // 增加寫入磁碟計數
                }

                interrupt++;  // Page Replacement Interrupt
                
                frames[replaceIndex].pageNumber = referenceString[i];  // 替換頁面
                frames[replaceIndex].modified = (rand() % 2 == 0); // 被替換的頁面有機率被修改
                interrupt++;
                queue[front] = replaceIndex;  // 更新隊列
                front = (front + 1) % frameCount;  // 更新 front 指針
                rear = (rear + 1) % frameCount;  // 更新 rear 指針
            }
        }
    }

   
    printf("FIFO PageFaults: %d\n", pageFaults);   // 頁面錯誤次數
    printf("FIFO Interrupts: %d\n", interrupt);    // 中斷次數
    printf("FIFO Disk Writes: %d\n", disk_writes);  // 磁碟寫入次數
    printf("\n");
    
     return 0; // 返回 0 表示正常結束
      
}

// Optimal 算法的實作
int optimal(Page frames[], int frameCount, int referenceString[], int length) {
    int pageFaults = 0;  // 頁面錯誤數量
    int interrupt = 0;   // 中斷計數
    int disk_writes = 0; // 磁碟寫入計數

    // 初始化頁面框架
    for (int i = 0; i < frameCount; i++) {
        frames[i].pageNumber = -1;  // 設置頁面號碼為空（初始為-1）
        frames[i].modified = false; // 初始頁面設置為未修改
    }

    // 遍歷每個參考字符串
    for (int i = 0; i < length; i++) {
        bool found = false;  // 用來標記頁面是否已經在框架中找到

        // 檢查參考字串中的頁面是否在幀中
        for (int j = 0; j < frameCount; j++) {
            if (frames[j].pageNumber == referenceString[i]) {
                found = true;  // 如果找到，將found設置為true
                frames[j].modified = true; // 模擬頁面被修改，設置為true
                break;
            }
        }

        // 如果頁面沒有找到，發生頁面錯誤
        if (!found) {
            pageFaults++;  // 增加頁面錯誤數量
            interrupt++;
            // 查找是否有空幀
            bool emptyFrameFound = false;
            for (int j = 0; j < frameCount; j++) {
                if (frames[j].pageNumber == -1) {  // 找到空幀
                    frames[j].pageNumber = referenceString[i];  // 將新頁面放入該框架
                    frames[j].modified = false;  // 設置為修改過（假設新頁面被寫入）
                    emptyFrameFound = true;  // 設置找到空幀的標誌
                    interrupt++;   // 新頁面載入引發 I/O 中斷
                    break;
                }
            }

            // 如果沒有空幀，則需要進行頁面替換
            if (!emptyFrameFound) {
                int indexToReplace = -1;  // 要替換的頁面索引
                int farthest = -1;        // 最遠使用的頁面位置

                // 找到需要被替換的頁面（即未來最晚使用的頁面）
                for (int j = 0; j < frameCount; j++) {
                    int nextUse = -1;  // 記錄下一次使用該頁面的時間

                    // 查找該頁面下一次在參考字符串中的使用時間
                    for (int k = i + 1; k < length; k++) {
                        if (frames[j].pageNumber == referenceString[k]) {
                            nextUse = k;  // 找到下一次使用的位置
                            break;
                        }
                    }

                    // 如果該頁面不再使用，則直接選擇這個頁面替換
                    if (nextUse == -1) {
                        indexToReplace = j;
                        break;
                    }

                    // 更新最遠使用的頁面
                    if (nextUse > farthest) {
                        farthest = nextUse;
                        indexToReplace = j;
                    }
                }

                // 檢查被替換的頁面是否被修改過，如果是則增加disk_writes
                if (frames[indexToReplace].modified) {
                    disk_writes++;  // 如果該頁面被修改過，則需要寫入磁碟
                }
                
                interrupt++;
                // 替換頁面
                frames[indexToReplace].pageNumber = referenceString[i];  // 替換頁面
                frames[indexToReplace].modified = (rand() % 2 == 0);  // 假設新載入的頁面被修改
                interrupt++;   // 替換頁面會引發I/O中斷
            }
        }
    }

    // 輸出結果
    printf("Optimal PageFaults: %d\n", pageFaults);   // 頁面錯誤次數
    printf("Optimal Interrupts: %d\n", interrupt);    // 中斷次數
    printf("Optimal Disk Writes: %d\n", disk_writes);  // 磁碟寫入次數
    printf("\n");

    return 0;  // 返回0表示程式正常結束
}
   


// Enhanced Second-Chance 算法的實作
int enhanced_second_chance(Page frames[], int frameCount, int referenceString[], int length) {
    int pageFaults = 0;  // 頁面錯誤數量
    int interrupt = 0;     // 中斷計數
    int disk_writes = 0;   // 寫入磁碟計數
    int pointer = 0;  // 指向當前檢查的頁面
    int filledFrames = 0; // 當前填滿的幀數
    
    for (int j = 0; j < frameCount; j++) {
    	frames[j].modified = false; // 初始為未修改
    	frames[j].referenced = false; // 初始為未修改
        frames[j].pageNumber = -1;  // 重置頁面號碼
    }
    
    

    // 遍歷每個參考字符串
    for (int i = 0; i < length; i++) {
        bool found = false;  // 標記頁面是否找到
        // 檢查頁面是否在幀中
        for (int j = 0; j < frameCount; j++) {
            if (frames[j].pageNumber == referenceString[i]) {
                found = true;  // 找到頁面
                frames[j].referenced = true;  // 更新參考位為 true
                frames[j].modified = true;     // 假設頁面被修改
                break;  // 結束內部循環
            }
        }

        if (!found) {  // 如果頁面不在幀中，則頁面錯誤
            pageFaults++;  // 增加頁面錯誤計數
            interrupt++;   //Page Fault Interrupt
            if (filledFrames < frameCount) {
                // 如果幀未滿，直接放入新頁面
                frames[filledFrames].pageNumber = referenceString[i]; // 將新頁面放入幀中
                frames[filledFrames].referenced = false; // 初始參考位設為 false
                frames[filledFrames].modified = false; // 初始修改位設為 false
                filledFrames++; // 增加已填滿幀數
            } else {
                // 如果幀已滿，則需要替換頁面
                while (true) {  // 循環直到找到可以替換的頁面
                    if (!frames[pointer].referenced) {
                        // 如果參考位為 false，則可以替換
                        
                         if (frames[pointer].modified) {
                        	// 寫回磁碟
                        	disk_writes++;  // 增加寫入磁碟計數
					}
                            
                            
                        
                        
                        interrupt++; // Page Replacement Interrupt
                        frames[pointer].pageNumber = referenceString[i];  // 將新頁面放入幀中
                        frames[pointer].referenced = false; // 初始參考位設為 false
                        frames[pointer].modified = (rand() % 2 == 0); // 被替換的 
                        
						interrupt++;
                        pointer = (pointer + 1) % frameCount;  // 更新指針以指向下一個頁面
                        break;  // 退出循環
                    } else {
                        // 如果參考位為 true，則給予第二次機會，並將參考位設為 false
                        frames[pointer].referenced = false; 
                        pointer = (pointer + 1) % frameCount;  // 更新指針以指向下一個頁面
                    }

                    
                }
            }
        }
    }
    printf("ESC PageFaults: %d\n", pageFaults);   // 頁面錯誤次數
    printf("ESC Interrupts: %d\n", interrupt);    // 中斷次數
    printf("ESC Disk Writes: %d\n", disk_writes);  // 磁碟寫入次數
    printf("\n");
    
    return 0; // 返回 0 表示正常結束
    
}
    // Customize 算法的實作 || LFU-LRU Hybrid Replacement 
int customize(Page frames[], int frameCount, int referenceString[], int length) {
    int pageFaults = 0;    // 記錄頁面錯誤數量
    int interrupt = 0;     // 中斷計數
    int disk_writes = 0;   // 磁碟寫入計數
    int currentTime = 0;   // 模擬時間
    int usedFrames = 0;    // 當前已使用的幀數量

    // 初始化幀
    for (int i = 0; i < frameCount; i++) {
        frames[i].pageNumber = -1; // 空幀
        frames[i].frequency = 0;    // 訪問頻率
        frames[i].lastAccessTime = -1; // 最近訪問時間
        frames[i].modified = false;  // 初始化為未修改
    }

    for (int i = 0; i < length; i++) {
        bool found = false;
        currentTime++; // 每次迭代時間增加

        // 檢查頁面是否在幀中
        for (int j = 0; j < usedFrames; j++) {
            if (frames[j].pageNumber == referenceString[i]) {
                found = true; // 頁面在幀中
                frames[j].modified = true; // 假設頁面被修改
                frames[j].frequency++; // 增加該頁面的訪問頻率
                frames[j].lastAccessTime = currentTime; // 更新最近訪問時間
                break;
            }
        }

        if (!found) { // 如果頁面不在幀中，發生頁面錯誤
            pageFaults++; // 增加頁面錯誤計數
            interrupt++; // Page Fault Interrupt

            if (usedFrames < frameCount) {
                // 如果幀未滿，直接添加頁面
                frames[usedFrames].pageNumber = referenceString[i];
                frames[usedFrames].frequency = 1; // 初始化頻率
                frames[usedFrames].lastAccessTime = currentTime; // 更新訪問時間
                usedFrames++; // 增加已使用幀數
            } else {
                // 幀已滿，進行替換
                int indexToReplace = 0;
                int minFrequency = frames[0].frequency;
                int oldestTime = frames[0].lastAccessTime;

                // 找到頻率最低且最近最少使用的頁框
                for (int j = 1; j < frameCount; j++) {
                    if (frames[j].frequency < minFrequency ||
                        (frames[j].frequency == minFrequency && frames[j].lastAccessTime < oldestTime)) {
                        indexToReplace = j;
                        minFrequency = frames[j].frequency;
                        oldestTime = frames[j].lastAccessTime;
                    }
                }

                // 檢查是否需要寫入磁碟
                if (frames[indexToReplace].modified) {
                    disk_writes++; // 增加磁碟寫入計數
                }

                interrupt++; // Page Replacement Interrupt
                frames[indexToReplace].pageNumber = referenceString[i]; // 替換頁面
                interrupt++;  //I/O Interrupt  
                frames[indexToReplace].modified = (rand() % 2 == 0);
                frames[indexToReplace].frequency = 1; // 初始化頻率
                frames[indexToReplace].lastAccessTime = currentTime; // 更新訪問時間
            }
        }
    }

    // 輸出結果
    printf("Customize PageFaults: %d\n", pageFaults);   // 頁面錯誤次數
    printf("Customize Interrupts: %d\n", interrupt);    // 中斷次數
    printf("Customize Disk Writes: %d\n", disk_writes);  // 磁碟寫入次數
    printf("\n");

    return 0; // 返回 0 表示正常結束
}


