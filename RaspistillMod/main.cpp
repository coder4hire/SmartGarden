#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <dirent.h>
#include <cstdlib>
#include <vector>

int countFiles(const char* dirPath)
{
    DIR* dir;
    struct dirent* ent;
    int count = 0;
    if ((dir = opendir(dirPath)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG)
            {
                count++;
            }
        }
        closedir(dir);
    }
    return count;
}

int main(int argc, char* argv[])
{
    //--- Getting output filename
    char* outFileName = (char*)"tempcam.jpg";
    int timeout = 0;
    for (int i = 0; i < argc-1; i++)
    {
        if (!strcmp(argv[i], "-o"))
        {
            outFileName = argv[i + 1];
            i++;
        }

        if (!strcmp(argv[i], "-t"))
        {
            timeout = std::atoi(argv[i + 1]);
            i++;
        }
    }

    if (timeout)
    {
        sleep(timeout);
    }

    int image_num = countFiles("/tmp/cam0");
    
    //--- Reading last shown frame, calculating new one
    int curFrame = 0;
    FILE* fpNum = fopen("/tmp/cam0/curFrame.txt", "rb");
    if (fpNum)
    {
        fscanf(fpNum,"%02d", &curFrame);
        fclose(fpNum);
    }
    curFrame++;

    //--- Accessing image
    char imgName[] = "/tmp/cam0/00.jpg";
	imgName[10] = '0' + (curFrame % 100) / 10;
    imgName[11] = '0' + curFrame % 10;

    FILE* fpImg = fopen(imgName, "rb");
    if (!fpImg)
    {
        curFrame = 0;
        imgName[10] = '0';
        imgName[11] = '0';
        fpImg = fopen(imgName, "rb");
    }
    if (!fpImg)
    {
        printf("REMOTE CAMERA: No captured files\n");
        return -1;
    }

    //--- Storing frame num
    fpNum = fopen("/tmp/cam0/curFrame.txt", "wb");
    if (fpNum)
    {
        fprintf(fpNum, "%02d", curFrame);
        fclose(fpNum);
    }

    //--- Copying image
    fseek(fpImg, 0, SEEK_END);
    long sz = ftell(fpImg);
    fseek(fpImg, 0, SEEK_SET);

    try
    {
        std::vector<unsigned char> buf(sz);
        if (fread(&buf[0], sz, 1, fpImg))
        {
            FILE* fpOut = fopen(outFileName, "wb");
            if (fpOut)
            {
                fwrite(buf.data(), sz, 1, fpOut);
                fclose(fpOut);
            }
        }
    }
    catch (...)
    {
    }
    fclose(fpImg);
    return 0;
}