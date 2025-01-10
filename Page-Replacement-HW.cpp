#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_FRAMES 100              // �̤jFrame number�]�w
#define REFERENCE_STRING_LENGTH 120000  //  reference string������
#define MAX_RANGE 1200              //  reference string���ƭȽd��

// �w�q���������c��
typedef struct {
    int pageNumber;  // Page Number    
    bool referenced;  // �ѦҦ�A�Ω� Enhanced Second-Chance ��k
    bool modified;    // �ק��
    int frequency;    // �X���W�v�p��      
    int lastAccessTime; // �W���X�ݪ��ɶ�
    
} Page;

// ��ƭ쫬
void generate_random_reference_string(int referenceString[], int length, int range);
void generate_locality_reference_string(int referenceString[], int length, int range);
void generate_myself_reference_string(int referenceString[], int length, int range);

void reset_frames(Page frames[], int frameCount) {
    for (int j = 0; j < frameCount; j++) {
        frames[j].pageNumber = -1;  // ���m�������X
        frames[j].modified = false;  
        frames[j].referenced = false; // �ѦҦ��l�� false
    }
}

int fifo(Page frames[], int frameCount, int referenceString[], int length);
int optimal(Page frames[], int frameCount, int referenceString[], int length);
int enhanced_second_chance(Page frames[], int frameCount, int referenceString[], int length);
int customize(Page frames[], int frameCount, int referenceString[], int length);

int main() {
    srand(time(NULL));  // �T�O�C�����泣�ͦ����P���H����
    int referenceString[REFERENCE_STRING_LENGTH];  // �s�x�ͦ��� reference string
    int m;  // Mode Number
    printf("�п��String Mode\n");
    printf("1. generate_random_reference_string\n");
    printf("2. generate_locality_reference_string\n");
    printf("3. generate_myself_reference_string\n");
    scanf("%d",&m);
        
    if(m == 1){
    	generate_random_reference_string(referenceString, REFERENCE_STRING_LENGTH, MAX_RANGE);  // �ͦ� random reference string

	} else if(m == 2){
		generate_locality_reference_string(referenceString, REFERENCE_STRING_LENGTH, MAX_RANGE);  // �ͦ� random reference string

	}else if(m == 3){
		generate_myself_reference_string(referenceString, REFERENCE_STRING_LENGTH, MAX_RANGE);  // �ͦ� locality reference string

	} else {
		printf("Input mode is incorre");
		exit(0);
	}

    

    // ���C�Ӻ�k���t frame���������c�Ʋ�
    Page frames[MAX_FRAMES] = {0};

    // ���դ��P�� frame number�G�q 10 �� MAX_FRAMES�A�C���W�[ 10
    for (int frameCount = 10; frameCount <= MAX_FRAMES; frameCount += 10) {
        // ���m�V�����������X�M�ѦҦ�
        for (int j = 0; j < frameCount; j++) {
            frames[j].pageNumber = -1;  // ��l�������X�]�� -1�A��ܪŴV
            frames[j].referenced = false; // �ѦҦ��l�� false
            frames[j].modified = false; 
            
        }
        
        printf("Frame Count: %d\n", frameCount);
        printf("----------------------------\n");

        // �p�� FIFO ��k���������~��
        int pageFaultsFIFO = fifo(frames, frameCount, referenceString, REFERENCE_STRING_LENGTH);
        
        // ���m�V
//        reset_frames(frames, frameCount);  // ���m�V
        
        // �p�� Optimal ��k���������~��
        int pageFaultsOptimal = optimal(frames, frameCount, referenceString, REFERENCE_STRING_LENGTH);
        
        // ���m�V
//        reset_frames(frames, frameCount);  // ���m�V
        
        // �p�� Enhanced Second-Chance ��k���������~��
        int pageFaultsESC = enhanced_second_chance(frames, frameCount, referenceString, REFERENCE_STRING_LENGTH);
        
        // ���m�V
//        reset_frames(frames, frameCount);  // ���m�V
                
         // �p�� Customize ��k���������~��
        int pageFaultsCUS = customize(frames, frameCount, referenceString, REFERENCE_STRING_LENGTH);

       
    }

    return 0;  // �{�ǥ��`����
}

// �H���ͦ��ѦҦr�Ŧꪺ���
void generate_random_reference_string(int referenceString[], int length, int range) {
	if (length <= 0 || range <= 0) {
        printf("Length and range must be positive integers.\n");
        return; // �Ѽ��ˬd
    }
    
    for (int i = 0; i < length; i++) {
        referenceString[i] = rand() % range + 1;  // �H���ͦ��d�� 1~1200 ���Ʀr
    }
    
}

// locality�ͦ��ѦҦr�Ŧꪺ���
void generate_locality_reference_string(int referenceString[], int length, int range) {
    int totalLength = 0;
    int localityRange = range / 10;  // �]�m�����ʽd����p����
    
    while (totalLength < length) {
        // ��ܤ@���H���������d�򤤤��I
        int localityCenter = rand() % range;
        
        // �]�m��e���q������
        int callLength = rand() % (length / 100) + (length / 200);
        
        // �T�O���W�L����ѦҦr�����
        if (totalLength + callLength > length) {
            callLength = length - totalLength;
        }
        
        // �b�����d�򤺥ͦ��H���Ʀr
        for (int i = 0; i < callLength; i++) {
            referenceString[totalLength + i] = (localityCenter + rand() % localityRange) % range + 1;
        }

        totalLength += callLength;
    }
    
}
void generate_myself_reference_string(int referenceString[], int length, int range) {
	int index = 0;

    while (index < length) {
        int value = (rand() % range) + 1;  // �H���Ʀr�d��b 1 �� 1200
        int count = rand() % 10 + 1;  // �H���ͦ��s��ۦP�Ʀr�����ס]1��10�^

        // �T�O���W�L�̤j����
        for (int j = 0; j < count && index < length; j++) {
            referenceString[index++] = value;
        }
    }
}



// FIFO ��k����@
int fifo(Page frames[], int frameCount, int referenceString[], int length) {
    int pageFaults = 0;          // �������~�ƶq
    int queue[MAX_FRAMES];       // �Ω� FIFO �����C�A�x�s�V������
    int front = 0, rear = 0;     // FIFO ���w�Afront ���̦����J����m�Arear ���U�@�Ӵ��J����m
    int numPagesInFrames = 0;    // ��e�V���������ƶq
    bool found;                 // �аO�����O�_���
    bool modified[MAX_FRAMES] = {false}; // �Ω�аO�C�ӭ����O�_�Q�ק�
    int interrupt = 0;           // ���_�p��
    int disk_writes = 0;         // �g�J�ϺЭp��

    // ��l�ƶ��C�]�]�m�� -1�A��ܥ��ϥΡ^
    for (int i = 0; i < MAX_FRAMES; i++) {
        queue[i] = -1;
       
    }

    // �M���C�ӰѦҦr�Ŧ�
    for (int i = 0; i < length; i++) {
        found = false;  // �C�����N���m�� false

        // �ˬd�����O�_�b�V��
        for (int j = 0; j < numPagesInFrames; j++) {
            if (frames[queue[j]].pageNumber == referenceString[i]) {
                found = true;  // ��쭶��                 
            	frames[queue[j]].modified = true;  // �аO�ӭ������w�ק�       	 	               
                break;  // ���������`��
            }
        }

        if (!found) {  // �p�G�������b�V���A�h�������~
            pageFaults++;  // �W�[�������~�p��
            interrupt++;   //Page Fault Interrupt
            

            // �p�G�V�����A�h�������J����
            if (numPagesInFrames < frameCount) {
                frames[numPagesInFrames].pageNumber = referenceString[i];
                // �s���J�������O�_���ק�]���]���ƭ����O�g�ާ@�^       
            	frames[numPagesInFrames].modified = false; // �S���ק�
        	
                queue[rear] = numPagesInFrames;  // �O�����J��������m
                rear = (rear + 1) % frameCount;  // ��s rear ���w
                numPagesInFrames++;  // �W�[�V���������ƶq
            } else {
                // �p�G�V�w���A�����̦��i�J������
                int replaceIndex = queue[front];  // ���̦�������

                // �ˬd�O�_�ݭn�g�J�Ϻ�
                if (frames[replaceIndex].modified) {
                    disk_writes++;  // �W�[�g�J�ϺЭp��
                }

                interrupt++;  // Page Replacement Interrupt
                
                frames[replaceIndex].pageNumber = referenceString[i];  // ��������
                frames[replaceIndex].modified = (rand() % 2 == 0); // �Q���������������v�Q�ק�
                interrupt++;
                queue[front] = replaceIndex;  // ��s���C
                front = (front + 1) % frameCount;  // ��s front ���w
                rear = (rear + 1) % frameCount;  // ��s rear ���w
            }
        }
    }

   
    printf("FIFO PageFaults: %d\n", pageFaults);   // �������~����
    printf("FIFO Interrupts: %d\n", interrupt);    // ���_����
    printf("FIFO Disk Writes: %d\n", disk_writes);  // �Ϻмg�J����
    printf("\n");
    
     return 0; // ��^ 0 ��ܥ��`����
      
}

// Optimal ��k����@
int optimal(Page frames[], int frameCount, int referenceString[], int length) {
    int pageFaults = 0;  // �������~�ƶq
    int interrupt = 0;   // ���_�p��
    int disk_writes = 0; // �Ϻмg�J�p��

    // ��l�ƭ����ج[
    for (int i = 0; i < frameCount; i++) {
        frames[i].pageNumber = -1;  // �]�m�������X���š]��l��-1�^
        frames[i].modified = false; // ��l�����]�m�����ק�
    }

    // �M���C�ӰѦҦr�Ŧ�
    for (int i = 0; i < length; i++) {
        bool found = false;  // �ΨӼаO�����O�_�w�g�b�ج[�����

        // �ˬd�ѦҦr�ꤤ�������O�_�b�V��
        for (int j = 0; j < frameCount; j++) {
            if (frames[j].pageNumber == referenceString[i]) {
                found = true;  // �p�G���A�Nfound�]�m��true
                frames[j].modified = true; // ���������Q�ק�A�]�m��true
                break;
            }
        }

        // �p�G�����S�����A�o�ͭ������~
        if (!found) {
            pageFaults++;  // �W�[�������~�ƶq
            interrupt++;
            // �d��O�_���ŴV
            bool emptyFrameFound = false;
            for (int j = 0; j < frameCount; j++) {
                if (frames[j].pageNumber == -1) {  // ���ŴV
                    frames[j].pageNumber = referenceString[i];  // �N�s������J�Ӯج[
                    frames[j].modified = false;  // �]�m���ק�L�]���]�s�����Q�g�J�^
                    emptyFrameFound = true;  // �]�m���ŴV���лx
                    interrupt++;   // �s�������J�޵o I/O ���_
                    break;
                }
            }

            // �p�G�S���ŴV�A�h�ݭn�i�歶������
            if (!emptyFrameFound) {
                int indexToReplace = -1;  // �n��������������
                int farthest = -1;        // �̻��ϥΪ�������m

                // ���ݭn�Q�����������]�Y���ӳ̱ߨϥΪ������^
                for (int j = 0; j < frameCount; j++) {
                    int nextUse = -1;  // �O���U�@���ϥθӭ������ɶ�

                    // �d��ӭ����U�@���b�ѦҦr�Ŧꤤ���ϥήɶ�
                    for (int k = i + 1; k < length; k++) {
                        if (frames[j].pageNumber == referenceString[k]) {
                            nextUse = k;  // ���U�@���ϥΪ���m
                            break;
                        }
                    }

                    // �p�G�ӭ������A�ϥΡA�h������ܳo�ӭ�������
                    if (nextUse == -1) {
                        indexToReplace = j;
                        break;
                    }

                    // ��s�̻��ϥΪ�����
                    if (nextUse > farthest) {
                        farthest = nextUse;
                        indexToReplace = j;
                    }
                }

                // �ˬd�Q�����������O�_�Q�ק�L�A�p�G�O�h�W�[disk_writes
                if (frames[indexToReplace].modified) {
                    disk_writes++;  // �p�G�ӭ����Q�ק�L�A�h�ݭn�g�J�Ϻ�
                }
                
                interrupt++;
                // ��������
                frames[indexToReplace].pageNumber = referenceString[i];  // ��������
                frames[indexToReplace].modified = (rand() % 2 == 0);  // ���]�s���J�������Q�ק�
                interrupt++;   // ���������|�޵oI/O���_
            }
        }
    }

    // ��X���G
    printf("Optimal PageFaults: %d\n", pageFaults);   // �������~����
    printf("Optimal Interrupts: %d\n", interrupt);    // ���_����
    printf("Optimal Disk Writes: %d\n", disk_writes);  // �Ϻмg�J����
    printf("\n");

    return 0;  // ��^0��ܵ{�����`����
}
   


// Enhanced Second-Chance ��k����@
int enhanced_second_chance(Page frames[], int frameCount, int referenceString[], int length) {
    int pageFaults = 0;  // �������~�ƶq
    int interrupt = 0;     // ���_�p��
    int disk_writes = 0;   // �g�J�ϺЭp��
    int pointer = 0;  // ���V��e�ˬd������
    int filledFrames = 0; // ��e�񺡪��V��
    
    for (int j = 0; j < frameCount; j++) {
    	frames[j].modified = false; // ��l�����ק�
    	frames[j].referenced = false; // ��l�����ק�
        frames[j].pageNumber = -1;  // ���m�������X
    }
    
    

    // �M���C�ӰѦҦr�Ŧ�
    for (int i = 0; i < length; i++) {
        bool found = false;  // �аO�����O�_���
        // �ˬd�����O�_�b�V��
        for (int j = 0; j < frameCount; j++) {
            if (frames[j].pageNumber == referenceString[i]) {
                found = true;  // ��쭶��
                frames[j].referenced = true;  // ��s�ѦҦ쬰 true
                frames[j].modified = true;     // ���]�����Q�ק�
                break;  // ���������`��
            }
        }

        if (!found) {  // �p�G�������b�V���A�h�������~
            pageFaults++;  // �W�[�������~�p��
            interrupt++;   //Page Fault Interrupt
            if (filledFrames < frameCount) {
                // �p�G�V�����A������J�s����
                frames[filledFrames].pageNumber = referenceString[i]; // �N�s������J�V��
                frames[filledFrames].referenced = false; // ��l�ѦҦ�]�� false
                frames[filledFrames].modified = false; // ��l�ק��]�� false
                filledFrames++; // �W�[�w�񺡴V��
            } else {
                // �p�G�V�w���A�h�ݭn��������
                while (true) {  // �`��������i�H����������
                    if (!frames[pointer].referenced) {
                        // �p�G�ѦҦ쬰 false�A�h�i�H����
                        
                         if (frames[pointer].modified) {
                        	// �g�^�Ϻ�
                        	disk_writes++;  // �W�[�g�J�ϺЭp��
					}
                            
                            
                        
                        
                        interrupt++; // Page Replacement Interrupt
                        frames[pointer].pageNumber = referenceString[i];  // �N�s������J�V��
                        frames[pointer].referenced = false; // ��l�ѦҦ�]�� false
                        frames[pointer].modified = (rand() % 2 == 0); // �Q������ 
                        
						interrupt++;
                        pointer = (pointer + 1) % frameCount;  // ��s���w�H���V�U�@�ӭ���
                        break;  // �h�X�`��
                    } else {
                        // �p�G�ѦҦ쬰 true�A�h�����ĤG�����|�A�ñN�ѦҦ�]�� false
                        frames[pointer].referenced = false; 
                        pointer = (pointer + 1) % frameCount;  // ��s���w�H���V�U�@�ӭ���
                    }

                    
                }
            }
        }
    }
    printf("ESC PageFaults: %d\n", pageFaults);   // �������~����
    printf("ESC Interrupts: %d\n", interrupt);    // ���_����
    printf("ESC Disk Writes: %d\n", disk_writes);  // �Ϻмg�J����
    printf("\n");
    
    return 0; // ��^ 0 ��ܥ��`����
    
}
    // Customize ��k����@ || LFU-LRU Hybrid Replacement 
int customize(Page frames[], int frameCount, int referenceString[], int length) {
    int pageFaults = 0;    // �O���������~�ƶq
    int interrupt = 0;     // ���_�p��
    int disk_writes = 0;   // �Ϻмg�J�p��
    int currentTime = 0;   // �����ɶ�
    int usedFrames = 0;    // ��e�w�ϥΪ��V�ƶq

    // ��l�ƴV
    for (int i = 0; i < frameCount; i++) {
        frames[i].pageNumber = -1; // �ŴV
        frames[i].frequency = 0;    // �X���W�v
        frames[i].lastAccessTime = -1; // �̪�X�ݮɶ�
        frames[i].modified = false;  // ��l�Ƭ����ק�
    }

    for (int i = 0; i < length; i++) {
        bool found = false;
        currentTime++; // �C�����N�ɶ��W�[

        // �ˬd�����O�_�b�V��
        for (int j = 0; j < usedFrames; j++) {
            if (frames[j].pageNumber == referenceString[i]) {
                found = true; // �����b�V��
                frames[j].modified = true; // ���]�����Q�ק�
                frames[j].frequency++; // �W�[�ӭ������X���W�v
                frames[j].lastAccessTime = currentTime; // ��s�̪�X�ݮɶ�
                break;
            }
        }

        if (!found) { // �p�G�������b�V���A�o�ͭ������~
            pageFaults++; // �W�[�������~�p��
            interrupt++; // Page Fault Interrupt

            if (usedFrames < frameCount) {
                // �p�G�V�����A�����K�[����
                frames[usedFrames].pageNumber = referenceString[i];
                frames[usedFrames].frequency = 1; // ��l���W�v
                frames[usedFrames].lastAccessTime = currentTime; // ��s�X�ݮɶ�
                usedFrames++; // �W�[�w�ϥδV��
            } else {
                // �V�w���A�i�����
                int indexToReplace = 0;
                int minFrequency = frames[0].frequency;
                int oldestTime = frames[0].lastAccessTime;

                // ����W�v�̧C�B�̪�̤֨ϥΪ�����
                for (int j = 1; j < frameCount; j++) {
                    if (frames[j].frequency < minFrequency ||
                        (frames[j].frequency == minFrequency && frames[j].lastAccessTime < oldestTime)) {
                        indexToReplace = j;
                        minFrequency = frames[j].frequency;
                        oldestTime = frames[j].lastAccessTime;
                    }
                }

                // �ˬd�O�_�ݭn�g�J�Ϻ�
                if (frames[indexToReplace].modified) {
                    disk_writes++; // �W�[�Ϻмg�J�p��
                }

                interrupt++; // Page Replacement Interrupt
                frames[indexToReplace].pageNumber = referenceString[i]; // ��������
                interrupt++;  //I/O Interrupt  
                frames[indexToReplace].modified = (rand() % 2 == 0);
                frames[indexToReplace].frequency = 1; // ��l���W�v
                frames[indexToReplace].lastAccessTime = currentTime; // ��s�X�ݮɶ�
            }
        }
    }

    // ��X���G
    printf("Customize PageFaults: %d\n", pageFaults);   // �������~����
    printf("Customize Interrupts: %d\n", interrupt);    // ���_����
    printf("Customize Disk Writes: %d\n", disk_writes);  // �Ϻмg�J����
    printf("\n");

    return 0; // ��^ 0 ��ܥ��`����
}


