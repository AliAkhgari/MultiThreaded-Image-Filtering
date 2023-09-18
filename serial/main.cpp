#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

struct Pixel
{
    int red;
    int green;
    int blue;
};
vector < vector <Pixel> > RGB;

int rows;
int cols;

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
    auto start_time = std::chrono::high_resolution_clock::now();
    std::ifstream file(fileName);

    if (file)
    {
        file.seekg(0, std::ios::end);
        std::streampos length = file.tellg();
        file.seekg(0, std::ios::beg);

        buffer = new char[length];
        file.read(&buffer[0], length);

        PBITMAPFILEHEADER file_header;
        PBITMAPINFOHEADER info_header;

        file_header = (PBITMAPFILEHEADER)(&buffer[0]);
        info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
        rows = info_header->biHeight;
        cols = info_header->biWidth;
        bufferSize = file_header->bfSize;
        auto end_time = std::chrono::high_resolution_clock::now();
        auto time = end_time - start_time;
        cout << "fillAndAllocate() Time -->  " << time/std::chrono::milliseconds(1) << "ms" << endl;
        return 1;
    }
    else
    {
        cout << "File" << fileName << " doesn't exist!" << endl;
        return 0;
    }
}

void getPixlesFromBMP24(int end, int rows, int cols, char* fileReadBuffer)
{
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
            for (int k = 0; k < 3; k++)
            {
                switch (k)
                {
                case 0:
                    if(fileReadBuffer[end - count] < 0)
                        RGB[i][j].red = fileReadBuffer[end - count] + 256;
                    else
                        RGB[i][j].red = fileReadBuffer[end - count];
                    break;

                case 1:
                    if(fileReadBuffer[end - count] < 0)
                        RGB[i][j].green = fileReadBuffer[end - count] + 256;
                    else
                        RGB[i][j].green = fileReadBuffer[end - count];
                    break;

                case 2:
                    if(fileReadBuffer[end - count] < 0)
                        RGB[i][j].blue = fileReadBuffer[end - count] + 256;
                    else
                        RGB[i][j].blue = fileReadBuffer[end - count];
                    break;
                // go to the next position in the buffer
                }
                count++;
            }
    }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize)
{
    std::ofstream write(nameOfFileToCreate);
    if (!write)
    {
        cout << "Failed to write " << nameOfFileToCreate << endl;
        return;
    }
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
            for (int k = 0; k < 3; k++)
            {
                switch (k)
                {
                case 0:
                    fileBuffer[bufferSize - count] = RGB[i][j].red;
                break;
                case 1:
                    fileBuffer[bufferSize - count] = RGB[i][j].green;
                break;
                case 2:
                    fileBuffer[bufferSize - count] = RGB[i][j].blue;
                break;
                // go to the next position in the buffer
                }
                count++;
            }
    }
    
    write.write(fileBuffer, bufferSize);
}

void smoothingFilter()
{
    int sum[3] = {0, 0, 0};
    
    for(int i = 1; i < RGB.size() - 1; i++)
    {
        for(int j = 1; j < RGB[i].size() - 1; j++)
        {
            for(int n = 0; n < 3; n++)
                sum[n] = 0;
            for(int k = i - 1; k < i + 2; k++)
                for(int l = j - 1; l < j + 2; l++)
                    for(int m = 0; m < 3; m++)
                    {
                        if(m == 0)
                            sum[m] += RGB[k][l].red;
                        else if(m == 1)
                            sum[m] += RGB[k][l].green;
                        else    
                            sum[m] += RGB[k][l].blue;
                    }
            for(int o = 0; o < 3; o++)
            {
                if(o == 0)
                    RGB[i][j].red = sum[o]/9;
                else if(o == 1)
                    RGB[i][j].green = sum[o]/9;
                else    
                    RGB[i][j].blue = sum[o]/9;
            }
        }
    }
}

void sepiaFilter()
{
    for(int i = 0; i < RGB.size(); i++)
    {
        for(int j = 0; j < RGB[i].size(); j++)
        {
            int R = RGB[i][j].red * 0.393 + RGB[i][j].green * 0.769 + RGB[i][j].blue * 0.189;
            int G = RGB[i][j].red * 0.349 + RGB[i][j].green * 0.686 + RGB[i][j].blue * 0.168;
            int B = RGB[i][j].red * 0.272 + RGB[i][j].green * 0.534 + RGB[i][j].blue * 0.131;
            RGB[i][j].red = min(R, 255);
            RGB[i][j].green = min(G, 255);
            RGB[i][j].blue = min(B, 255);
        }
    }
}

void washedOutFilter()
{
    int R_sum = 0, G_sum = 0, B_sum = 0;
    int count = 0;
    for(int i = 0; i < RGB.size(); i++)
    {
        for(int j = 0; j < RGB[i].size(); j++)
        {
            R_sum += RGB[i][j].red;
            G_sum += RGB[i][j].green;
            B_sum += RGB[i][j].blue;
            count++;
        }
    }

    R_sum = R_sum / count;
    G_sum = G_sum / count;
    B_sum = B_sum / count;
    for(int i = 0; i < RGB.size(); i++)
    {
        for(int j = 0; j < RGB[i].size(); j++)
        {
            RGB[i][j].red = RGB[i][j].red * 0.4 + R_sum * 0.6;
            RGB[i][j].green = RGB[i][j].green * 0.4 + G_sum * 0.6;
            RGB[i][j].blue = RGB[i][j].blue * 0.4 + B_sum * 0.6;
        }
    }
}

void addCross()
{
    int j = 0;
    for(int i = 0; i < rows; i++)
    {
        RGB[i][j].red = 255;
        RGB[i][j].green = 255;
        RGB[i][j].blue = 255;
        if(j + 1 < cols)
        {
            RGB[i][j+1].red = 255;
            RGB[i][j+1].green = 255;
            RGB[i][j+1].blue = 255;
        }
        if(j + 2 < cols)
        {
            RGB[i][j+2].red = 255;
            RGB[i][j+2].green = 255;
            RGB[i][j+2].blue = 255;
        }
        j++;
    }
    j = cols - 1;
    for(int i = 0; i < rows; i++)
    {
        RGB[i][j].red = 255;
        RGB[i][j].green = 255;
        RGB[i][j].blue = 255;
        if(j - 1 >= 0)
        {
            RGB[i][j-1].red = 255;
            RGB[i][j-1].green = 255;
            RGB[i][j-1].blue = 255;
        }
        if(j - 2 >= 0)
        {
            RGB[i][j-2].red = 255;
            RGB[i][j-2].green = 255;
            RGB[i][j-2].blue = 255;
        }
        j--;
    }
}

std::chrono::time_point<std::chrono::high_resolution_clock> current_time()
{
    return std::chrono::high_resolution_clock::now();
}

void print_time(string name, std::chrono::time_point<std::chrono::high_resolution_clock> start,
                std::chrono::time_point<std::chrono::high_resolution_clock> end)
{
    cout << name << (end - start) / std::chrono::milliseconds(1) << "ms" << endl;
}

int main(int argc, char *argv[])
{
    auto start_time = std::chrono::high_resolution_clock::now();
    char *fileBuffer;
    int bufferSize;
    char *fileName = argv[1];
    if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
    {
        cout << "File read error" << endl;
        return 1;
    }
    RGB.resize(rows, vector<Pixel>(cols));

    // read input file
    auto filter_start_time = current_time();
    getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer);
    auto filter_end_time = current_time();
    print_time("getPixlesFromBMP24()   -->  ", filter_start_time, filter_end_time);

    // apply filters
    filter_start_time = current_time();
    smoothingFilter();
    filter_end_time = current_time();
    print_time("smoothingFilter()      -->  ", filter_start_time, filter_end_time);

    filter_start_time = current_time();
    sepiaFilter();
    filter_end_time = current_time();
    print_time("sepiaFilter()          -->  ", filter_start_time, filter_end_time);

    filter_start_time = current_time();
    washedOutFilter();
    filter_end_time = current_time();
    print_time("washedOutFilter()      -->  ", filter_start_time, filter_end_time);

    filter_start_time = current_time();
    addCross();
    filter_end_time = current_time();
    print_time("addCross()             -->  ", filter_start_time, filter_end_time);

    // write output file
    filter_start_time = current_time();
    writeOutBmp24(fileBuffer, "output.bmp", bufferSize);
    filter_end_time = current_time();
    print_time("writeOutBmp24()        -->  ", filter_start_time, filter_end_time);
    
    auto end_time = current_time();
    print_time("main()                 -->  ", start_time, end_time);

    return 0;
}