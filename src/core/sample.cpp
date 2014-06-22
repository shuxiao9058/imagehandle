#include "Image.h"

// #pragma comment(lib, "fltkjpeg.lib")
// #pragma comment(lib, "fltkpng.lib")
// #pragma comment(lib, "fltkzlib.lib")

// #ifndef _DEBUG
// #pragma comment(lib, "EasyBMP.lib")
// #else
// #pragma comment(lib, "EasyBMP_debug.lib")
// #endif

int main(int argc, char **argv)
{
    if (2 != argc)
        return -1;
    UCImage img1, img2;
    img1.imread(argv[1]);
    // img1.loadgray(argv[1]);
    img1.GaussianSmoothing(img1, 0.78, 2);
    img1.imresize(img2, 0.5);
    img2.imwrite("out.bmp");
    return 0;
}
