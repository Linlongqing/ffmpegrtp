#include <stdint.h>

enum HEADERTYPE
{
    KRJPEG = 0x0f0f0f0f,
    KRH264
};

//文件头结构体
typedef struct SImageHeader
{
    uint32_t headerType;        //数据类型标志位
    uint32_t width;             //图片的宽度
    uint32_t height;            //图片的高度
    uint32_t serverType;        //请求的服务类型
    uint32_t dataOffset;        //图片数据头的位置
    uint32_t dataSize;          //图片大小
    uint32_t headerCheckCode;   //校验码
} SImageHeader;