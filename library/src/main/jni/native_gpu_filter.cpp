//
// Created by ben622 on 2019/9/12.
//
#ifndef NATIVE_GPU_FILTER_CPP
#define NATIVE_GPU_FILTER_CPP

#include "include/jni/JniHelpers.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>

using namespace ben::jni;

class GPUFilter : public JavaClass {
public:
    GPUFilter() : JavaClass() {
    }

    GPUFilter(JNIEnv *env) : JavaClass(env) {
        initialize(env);
    }

    ~GPUFilter() override {

    }

    void initialize(JNIEnv *env) override {
        addNativeMethod("start", (void *) start, kTypeVoid, NULL);
    }

    void mapFields() override {

    }

    const char *getCanonicalName() const override {
        return "com/ben/android/library/Test";
    }

    static void start(JNIEnv *env, jobject javaThis) {

        const char *url = "/sdcard/Download/test.jpg";

        FILE *fp = fopen(url, "rb");
        if (!fp) {
            LOGE("%s", " open file %s failed !", url);
            return;
        }


        LOGE("%s", "open url is %s", url);

        //------------------------
        //EGL
        //1  display 显示
        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            LOGE("%s", "get display failed!");
            return;
        }
        //初始化 后面两个参数是版本号
        if (EGL_TRUE != eglInitialize(display, 0, 0)) {
            LOGE("%s", "eglInitialize failed!");
            return;
        }

        //2  surface （关联原始窗口）
        //surface 配置
        //输出配置
        EGLConfig config;
        EGLint configNum;
        //输入配置
        EGLint configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE,
                EGL_PBUFFER_BIT, //缓冲区
                EGL_NONE
        };

        if (EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &configNum)) {
            LOGE("%s", "eglChooseConfig failed!");
            return;
        }
        //创建surface （关联原始窗口）
        //EGLSurface winSurface = eglCreateWindowSurface(display, config, nwin, 0);
        EGLSurface winSurface = eglCreatePbufferSurface(display, config, 0);

        if (winSurface == EGL_NO_SURFACE) {
            LOGE("%s", "eglCreateWindowSurface failed!");
            return;
        }

        //3  context 创建关联上下文
        const EGLint ctxAttr[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
        };

        EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
        if (context == EGL_NO_CONTEXT) {
            LOGE("%s", "eglCreateContext failed!");
            return;
        }
        //egl 关联 openl
        if (EGL_TRUE != eglMakeCurrent(display, winSurface, winSurface, context)) {
            LOGE("%s", "eglMakeCurrent failed!");
            return;
        }
        LOGE("%s", "EGL Init Success!");

        //顶点和片元shader初始化
        //顶点
        GLint vsh = InitShader(vertexShader, GL_VERTEX_SHADER);
        //片元yuv420
        GLint fsh = InitShader(fragYUV420P, GL_FRAGMENT_SHADER);


        //////////////////////////////////////////////////////////
        //创建渲染程序
        GLint program = glCreateProgram();
        if (program == 0) {
            LOGE("%s", "glCreateProgram failed!");
            return;
        }
        //向渲染程序中加入着色器
        glAttachShader(program, vsh);
        glAttachShader(program, fsh);
        //链接程序
        glLinkProgram(program);
        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status != GL_TRUE) {
            LOGE("%s", "glLinkProgram failed!");
            return;
        }
        //激活渲染程序
        glUseProgram(program);
        LOGE("%s", "glLinkProgram success!");
        //////////////////////////////////////////////////////////

        //加入三维顶点数据 两个三角形组成正方形
        static float vers[] = {
                1.0f, -1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
        };
        //获取shader中的顶点变量
        GLuint apos = (GLuint) glGetAttribLocation(program, "aPosition");
        glEnableVertexAttribArray(apos);
        //传递顶点
        /*
         * apos 传到哪
         * 每一个点有多少个数据
         * 格式
         * 是否有法线向量
         * 一个数据的偏移量
         * 12 顶点有三个值（x,y，z）float存储 每个有4个字节 每一个值的间隔是 3*4 =12
         * ver 顶点数据
         * */
        glVertexAttribPointer(apos, 3, GL_FLOAT, GL_FALSE, 12, vers);

        //加入材质坐标数据
        static float txts[] = {
                1.0f, 0.0f,//右下
                0.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 1.0f
        };
        GLuint atex = (GLuint) glGetAttribLocation(program, "aTexCoord");
        glEnableVertexAttribArray(atex);
        glVertexAttribPointer(atex, 2, GL_FLOAT, GL_FLOAT, 8, txts);

        //材质纹理初始化

        int width = 1024;//实际应用中从ffmpeg 获取
        int height = 768;


        //设置纹理层
        glUniform1i(glGetUniformLocation(program, "yTexture"), 0);//对于纹理第1层
        glUniform1i(glGetUniformLocation(program, "uTexture"), 1);//对于纹理第2层
        glUniform1i(glGetUniformLocation(program, "vTexture"), 2);//对于纹理第3层

        //创建opengl纹理
        /*
         * 创建多少个纹理
         * 存放数组
         *
         * */
        GLuint texts[3] = {0};
        glGenTextures(3, texts); //创建3个纹理
        //设置纹理属性
        glBindTexture(GL_TEXTURE_2D, texts[0]);//绑定纹理，下面的属性针对这个纹理设置
        /*
         * GL_TEXTURE_2D 2D材质
         * GL_TEXTURE_MIN_FILTER 缩小的过滤
         * GL_LINEAR 线性差值 当前渲染像素最近的4个纹理做加权平均值
         *
         * */
        //缩小放大过滤器
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //设置纹理的格式和大小

        /*
         * Y
         * GL_TEXTURE_2D
         * 显示细节的级别
         * 内部gpu 格式 亮度 灰度图
         * 宽
         * 高
         * 边框
         * 数据的像素格式
         * 像素的数据类型
         * 纹理数据
         * */
        glTexImage2D(GL_TEXTURE_2D,
                     0,//默认
                     GL_LUMINANCE,
                     width, height, //尺寸要是2的次方  拉升到全屏
                     0,
                     GL_LUMINANCE,//数据的像素格式，要与上面一致
                     GL_UNSIGNED_BYTE,// 像素的数据类型
                     NULL
        );


        //U
        glBindTexture(GL_TEXTURE_2D, texts[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,
                     0,//默认
                     GL_LUMINANCE,
                     width / 2, height / 2, //尺寸要是2的次方  拉升到全屏
                     0,
                     GL_LUMINANCE,//数据的像素格式，要与上面一致
                     GL_UNSIGNED_BYTE,// 像素的数据类型
                     NULL
        );

        //V
        glBindTexture(GL_TEXTURE_2D, texts[2]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,
                     0,//默认
                     GL_LUMINANCE,
                     width / 2, height / 2, //尺寸要是2的次方  拉升到全屏
                     0,
                     GL_LUMINANCE,//数据的像素格式，要与上面一致
                     GL_UNSIGNED_BYTE,// 像素的数据类型
                     NULL
        );


        //////////////////////////////////////////////////////////
        //纹理的修改和显示 把显示的纹理放入内存buf中



        unsigned char *buf[3] = {0};
        buf[0] = new unsigned char[width * height];
        buf[1] = new unsigned char[width * height / 4];
        buf[2] = new unsigned char[width * height / 4];

        // 420p yyyyyy uu vv
        if (feof(fp) == 0) {//判断是否读到结尾
            fread(buf[0], 1, width * height, fp);
            fread(buf[1], 1, width * height / 4, fp);
            fread(buf[2], 1, width * height / 4, fp);
        }

        //激活第1层纹理 绑定到创建的opengl纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texts[0]);//绑定纹理

        //替换纹理内容
        /*
         * GL_TEXTURE_2D
         * 细节级别
         * 偏移位置yoffset
         * 偏移位置xoffset
         * 宽
         * 高
         * 数据格式
         * 存储搁置
         * 纹理数据写入buf中
         *
         * */
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                        buf[0]);


        //激活第2层纹理 绑定到创建的opengl纹理
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texts[1]);//绑定纹理
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                        GL_UNSIGNED_BYTE, buf[1]);



        //激活第1层纹理 绑定到创建的opengl纹理
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texts[2]);//绑定纹理
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                        GL_UNSIGNED_BYTE, buf[2]);

        //三维绘制
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);//从0顶点开始 一共4个顶点

        //窗口显示
        eglSwapBuffers(display, winSurface);//交换buf

        GLint size;
        size = 1024 * 768 * 8;
        GLubyte *data = (GLubyte*)malloc(size);
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(0,0,1024,768,GL_RGB,GL_UNSIGNED_BYTE,data);
        bmp_write(data, 1024, 768, "gpu");

    }



    //顶点着色器glsl
#define GET_STR(x) #x
    static const char *vertexShader = GET_STR(
            attribute
            vec4 aPosition;//顶点坐标
            attribute
            vec2 aTexCoord;//材质顶点坐标
            varying
            vec2 vTexCoord;//输出的材质坐标 输出给片元着色器
            void main() {
                vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
                gl_Position = aPosition;//显示顶点
            }
    );
    //片元着色器 软解码和部分x86硬解码 YUV420P
    static const char *fragYUV420P = GET_STR(
            precision
            mediump float;//精度
            varying
            vec2 vTexCoord;//顶点着色器传递的坐标
            uniform
            sampler2D yTexture;//输入材质参数（不透明灰度,单像素）
            uniform
            sampler2D uTexture;//输入材质参数
            uniform
            sampler2D vTexture;//输入材质参数
            void main() {
                vec3 yuv;
                vec3 rgb;
                yuv.r = texture2D(yTexture, vTexCoord).r;
                yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
                yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;
                rgb = mat3(1.0, 1.0, 1.0,
                           0.0, -0.39425, 2.03211,
                           1.13983, -0.5806, 0.0) * yuv;
                //输出像素颜色
                gl_FragColor = vec4(rgb, 1.0);
            }


    );


    //初始化着色器
    static GLint InitShader(const char *code, GLint type) {
        //创建shader
        GLint sh = glCreateShader(type);
        if (sh == 0) {
            LOGE("%s", " glCreateShader  %d failed ! ", type);
            return 0;
        }
        //加载shader
        glShaderSource(
                sh,
                1,//shader数量
                &code,//shader代码
                0);//代码长度 传0自动查找计算

        //编译shader 显卡
        glCompileShader(sh);

        //获取编译情况
        GLint status;
        glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
        if (status == 0) {
            LOGE("%s", " GL_COMPILE_STATUS   failed ! ");
            return 0;
        }
        LOGE("%s", " init shader   success ! ");
        return sh;
    }


   static int bmp_write(unsigned char *image, int xsize, int ysize, char *filename)
    {
        unsigned char header[54] =
                {
                        0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
                        54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0
                };
        long file_size = (long)xsize * (long)ysize * 3 + 54;
        long width, height;
        char fname_bmp[128];
        FILE *fp;

        header[2] = (unsigned char)(file_size &0x000000ff);
        header[3] = (file_size >> 8) & 0x000000ff;
        header[4] = (file_size >> 16) & 0x000000ff;
        header[5] = (file_size >> 24) & 0x000000ff;

        width = xsize;
        header[18] = width & 0x000000ff;
        header[19] = (width >> 8) &0x000000ff;
        header[20] = (width >> 16) &0x000000ff;
        header[21] = (width >> 24) &0x000000ff;

        height = ysize;
        header[22] = height &0x000000ff;
        header[23] = (height >> 8) &0x000000ff;
        header[24] = (height >> 16) &0x000000ff;
        header[25] = (height >> 24) &0x000000ff;

        sprintf(fname_bmp, "/sdcard/Download/%s.bmp", filename);

        if (!(fp = fopen(fname_bmp, "wb")))
            return -1;

        // switch the image data from RGB to BGR
        for(unsigned long imageIdx = 0; imageIdx < file_size; imageIdx+=3)
        {
            unsigned char tempRGB = image[imageIdx];
            image[imageIdx] = image[imageIdx + 2];
            image[imageIdx + 2] = tempRGB;
        }

        fwrite(header, sizeof(unsigned char), 54, fp);
        fwrite(image, sizeof(unsigned char), (size_t)(long)xsize * ysize * 3, fp);

        fclose(fp);
        return 0;
    }
};

#endif //NATIVE_GPU_FILTER_CPP

